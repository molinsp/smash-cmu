/**
 *	Copyright (c) 2013 Carnegie Mellon University.
 *	All Rights Reserved.
 *	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following acknowledgments and disclaimers.
 *	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *	3. The names “Carnegie Mellon University,” "SEI” and/or “Software Engineering Institute" shall not be used to endorse or promote products derived from this software without prior written permission. For written permission, please contact permission@sei.cmu.edu.
 *	4. Products derived from this software may not be called "SEI" nor may "SEI" appear in their names without prior written permission of permission@sei.cmu.edu.
 *	5. Redistributions of any form whatsoever must retain the following acknowledgment:
 *	This material is based upon work funded and supported by the Department of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University for the operation of the Software Engineering Institute, a federally funded research and development center.
 *	Any opinions, findings and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the United States Department of Defense.
 *	NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN “AS-IS” BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *	This material has been approved for public release and unlimited distribution.
 *	DM-0000245
 */

package edu.cmu.sei.madara;



import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.List;

public class Madara
{
	private String host;
	private String port;
	
	private String domain = "KaRL";
	private String madaraId = "KaRL1.1";
	
	private long clock = 10000;
	
	private DatagramSocket s;
	private InetAddress group;
	
	public static class MadaraType
	{
		public static final int INTEGER = 0;
		public static final int STRING = 1;
		public static final int DOUBLE = 2;
	}
	
	public Madara(String host, String port) throws Exception
	{
		this.host = host;
		this.port = port;
		group = InetAddress.getByName("228.5.6.7");
		s = new DatagramSocket();
	}
	
	public MadaraMessage createMessage()
	{
		return new MadaraMessage();
	}
	
	
	private boolean listenerRunning = false;
	private Thread listenerThread;
	
	public void startListener(final MadaraCallback mc)
	{
		if (!listenerRunning)
		{
			listenerThread = new Thread()
			{

				private MulticastSocket socket;
				
				@Override
				public void interrupt()
				{
					super.interrupt();
					if (socket != null)
						socket.close();
					listenerRunning = false;
				}

				@Override
				public void run()
				{
					try
					{
						byte[] buf = new byte[2048];
						int p = Integer.parseInt(port);
						socket = new MulticastSocket(p);
						socket.joinGroup(group);
						while (listenerRunning)
						{
							
							DatagramPacket packet = new DatagramPacket(buf, buf.length);
							socket.receive(packet);
							
							if (packet.getLength() != 0 && packet.getLength() > 131) //madara header is 131 bytes
							{
								MadaraMessage mm = new MadaraMessage();
								byte[] msg = new byte[packet.getLength()];
								System.arraycopy(packet.getData(), 0, msg, 0, packet.getLength());
								try
								{
									mm.read(new ByteArrayInputStream(msg));
									//Ignore messages from us
									if (!getOriginator().equals(mm.header.originator))
									{
										if ("KaRL".equals(mm.header.domain) && mm.header.multiassign == 2 && mm.header.updates > 0)
										{
											clock = mm.header.clock + 100;
											mc.callback(mm);
										}
									}
								}
								catch (Exception e)
								{
									e.printStackTrace();
								}
								
							}
						}
					}
					catch (SocketException se)
					{
						if (!this.isInterrupted())
							se.printStackTrace();
					}
					catch (Exception e)
					{
						e.printStackTrace();
					}
				}
				
			};
			listenerRunning = true;
			listenerThread.start();
		}
	}
	
	public void stopListener()
	{
		listenerThread.interrupt();
	}
	
	public void send(MadaraMessage msg) throws Exception
	{
		byte[] packet = msg.write();
		DatagramPacket dp = new DatagramPacket(packet, packet.length, group, 5500);
		s.send(dp);
	}
	
	public class MadaraMessage implements Readable, Writable
	{
		public MessageHeader header;
		public List<KnowledgeUpdate> updates;
		private MadaraMessage()
		{
			updates = new ArrayList<KnowledgeUpdate>();
		}

		@Override
		public byte[] write() throws IOException
		{
			ByteArrayOutputStream ret = new ByteArrayOutputStream();
			header = new MessageHeader(updates.size(), clock);
			ret.write(header.write());
			clock += 100;
			for (int x = 0; x < updates.size(); x++)
				ret.write(updates.get(x).write());
			
			byte[] _ret = ret.toByteArray();
			byte[] t_len = uintX(_ret.length, 64);
			System.arraycopy(t_len, 0, _ret, 0, t_len.length);
			
			return _ret;
		}
		
		@Override
		public void read(InputStream is) throws IOException
		{
			header = new MessageHeader();
			header.read(is);
			if (header.multiassign != 2 || !domain.equals(header.domain))
				return;
			
			for (int x = 0; x < header.updates; x++)
			{
				KnowledgeUpdate ku = new KnowledgeUpdate();
				ku.read(is);
				updates.add(ku);
			}
		}
		
		public void addUpdate(String varName, int type, Object value)
		{
			KnowledgeUpdate ku = new KnowledgeUpdate();
			ku.varName = varName;
			ku.type = type;
			ku.value = value;
			updates.add(ku);
		}
		
	}
	
	private static String readString(InputStream is, long len) throws IOException
	{
		byte[] buffer = new byte[(int)len];
		is.read(buffer);
		return new String(buffer).split("\0")[0];
	}
	
	private static long readLong(InputStream is, long len) throws IOException
	{
		long ret = 0;
		byte[] data = new byte[(int)(len / 8)];
		is.read(data);
		for (int x = 0; x < data.length; x++)
		{
			ret <<= 8;
			ret |= (data[x] & 0xFF);
		}
		
		return ret;
	}
	
	private static byte[] uintX(long l, int len)
	{
		int _len = len / 8;
		byte[] ret = new byte[_len];
		for (int x = 0; x < _len; x++)
		{
			ret[_len - (x + 1)] = (byte)(l & 0xFF);
			l >>= 8;
		}
		return ret;
	}
	
	private static byte[] stringToFixedLen(String s, int len)
	{
		ByteArrayOutputStream ret = new ByteArrayOutputStream();
		char[] arr = s.toCharArray();
		int slen = s.length() > len ? len : s.length();
		for (int x = 0; x < slen; x++)
			ret.write(arr[x] & 0xFF);
		ret.write('\0');
		for (int x = 0; x < len - (slen + 1); x++)
			ret.write(0x00);
		return ret.toByteArray();
	}
	
	private String getOriginator()
	{
		return host + (port == null ? "" : ":" + port);
	}
	
	private String getMadaraId()
	{
		return madaraId;
	}
	
	private String getDomain()
	{
		return domain;
	}
	
	private static byte[] stringToCStr(String s)
	{
		ByteArrayOutputStream ret = new ByteArrayOutputStream();
		char[] arr = s.toCharArray();
		for (int x = 0; x < arr.length; x++)
			ret.write(arr[x] & 0xFF);
		ret.write('\0');
		return ret.toByteArray();
	}
	
	public class MessageHeader implements Readable, Writable
	{
		public long size;
		public String transportId;
		public String domain;
		public String originator;
		public long multiassign;
		public long updates;
		public long quality;
		public long clock;
		
		private MessageHeader()
		{
			
		}
		
		private MessageHeader(long updates, long clock)
		{
			this.updates = updates;
			this.clock = clock;
		}
		
		@Override
		public byte[] write() throws IOException
		{
			ByteArrayOutputStream ret = new ByteArrayOutputStream();
			ret.write(uintX(0,64)); //placeholder for size
			ret.write(stringToFixedLen(getMadaraId(), 8)); // transport id "KaRL1.0"
			ret.write(stringToFixedLen(getDomain(), 32)); // domain "KaRL"
			ret.write(stringToFixedLen(getOriginator(), 64)); //127.0.0.1:5000
			ret.write(uintX(2, 32)); // MULTI-ASSIGN
			ret.write(uintX(updates, 32)); //NUMBER OF VAR UPDATES
			ret.write(uintX(0, 32)); // PRIORITY
			ret.write(uintX(clock, 64)); // CLOCK
			
			//ret.write(new byte[4]); //FOR 64-BIT ARCH
			
			return ret.toByteArray();
		}
		@Override
		public void read(InputStream is) throws IOException
		{
			size = readLong(is, 64);
			transportId = readString(is, 8);
			domain = readString(is, 32);
			originator = readString(is, 64);
			multiassign = readLong(is, 32);
			updates = readLong(is, 32);
			quality = readLong(is, 32);
			clock = readLong(is, 64);
			//readLong(is, 32); // read an extra var to wipe out 64-bit buffering
		}
	}
	
	public class KnowledgeUpdate implements Readable, Writable
	{
		public String varName;
		public int type;
		public Object value;
		
		private KnowledgeUpdate()
		{
			
		}
		
		@Override
		public byte[] write() throws IOException
		{
			ByteArrayOutputStream ret = new ByteArrayOutputStream();
			
			byte[] name = stringToCStr(varName);
			ret.write(uintX(name.length, 32));
			ret.write(name);
			
			ret.write(uintX(type, 32));
			
			byte[] val = null;
			
			switch (type)
			{
				case MadaraType.INTEGER:
					Integer i = (Integer)value;
					val = uintX(i.intValue(), 64);
					break;
				case MadaraType.DOUBLE: 
					Double d = (Double)value;
					val = stringToCStr("" + d.doubleValue());
					break;
				case MadaraType.STRING:
					String s = (String)value;
					val = stringToCStr(s);
					break;
				default: break;
			}
			
			ret.write(uintX(val.length, 32));
			ret.write(val);
			
			return ret.toByteArray();
		}
		
		@Override
		public void read(InputStream is) throws IOException
		{
			long name_len = readLong(is, 32);
			varName = readString(is, name_len);

			type = (int)readLong(is, 32);
			long len = readLong(is, 32);
			switch (type)
			{
				case MadaraType.INTEGER:
					value = (int)readLong(is, len * 8);
					break;
				case MadaraType.STRING:
					value = readString(is, len);
					break;
				case MadaraType.DOUBLE:
					value = Double.parseDouble(readString(is, len));
					break;
				default: break;
			}
		}
	}
	
	public interface Writable
	{
		public byte[] write() throws IOException;
	}
	
	public interface Readable
	{
		public void read(InputStream is) throws IOException;
	}
	
	public interface MadaraCallback
	{
		public void callback(MadaraMessage msg);
	}
}
