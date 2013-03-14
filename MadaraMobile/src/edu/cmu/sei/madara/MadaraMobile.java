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

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentHashMap;
import android.app.Application;
import android.os.AsyncTask;
import android.util.Log;
import edu.cmu.sei.madara.LogViewer.LogViewerHandler;
import edu.cmu.sei.madara.Madara.KnowledgeUpdate;
import edu.cmu.sei.madara.Madara.MadaraCallback;
import edu.cmu.sei.madara.Madara.MadaraMessage;
import edu.cmu.sei.madara.Madara.MadaraType;
import edu.cmu.sei.madara.MainActivity.MainActivityHandler;

public class MadaraMobile extends Application {

	private Madara madara;
	private MadaraHandler handler;
	private List<String> log;
	private ConcurrentHashMap<String, String> varMap;
	private Timer heartBeatTimer;
	private ConcurrentHashMap<Integer, DroneObject> droneMap;

	private MainActivityHandler mainActivityHandler;
	private LogViewerHandler logViewerHandler;

	@Override
	public void onCreate() {
		super.onCreate();

		droneMap = new ConcurrentHashMap<Integer, DroneObject>();
		log = new ArrayList<String>();
		varMap = new ConcurrentHashMap<String, String>();
		handler = new MadaraHandler();
		new StartMadaraAsyncTask().execute();

		heartBeatTimer = new Timer();
		heartBeatTimer.schedule(new TimerTask() {
			@Override
			public void run() {
				new SendMadaraMessage().execute(new String[]{"heartbeat","0","1"});
			}
		}, 0, 10000);
	}

	public class MadaraHandler{

		public void sendMadaraUpdate(String var, int type, String val){
			new SendMadaraMessage().execute(new String[]{var,type+"",val});
		}

		public ConcurrentHashMap<String, String> getMap(){
			return varMap;
		}

		public List<DroneObject> getDrones(){
			return new ArrayList<DroneObject>(droneMap.values());
		}
		
		public List<String> getLogs(){
			return log;
		}

		public void addMainActivityHandler(MainActivityHandler handler){
			mainActivityHandler = handler;
		}

		public void addLogViewerHandler(LogViewerHandler handler){
			logViewerHandler = handler;
		}
	}

	public MadaraHandler getHandler(){
		return this.handler;
	}

	private class StartMadaraAsyncTask extends AsyncTask<Void, Void, Void>{
		@Override
		protected Void doInBackground(Void... params) {
			try {
				madara = new Madara("228.5.6.7", "5500");
				//Start the listener and set the callback so we can see incoming packets
				madara.startListener(new MadaraCallback()
				{
					@Override
					public void callback(MadaraMessage msg)
					{
						List<KnowledgeUpdate> updates = msg.updates;
						for (KnowledgeUpdate update : updates){
							String updateString = System.currentTimeMillis() +"-RECIEVED: " +  update.varName + " = " + update.value;
							log.add(updateString);
							varMap.put(update.varName, update.value.toString());
							Log.d(MadaraMobile.class.getSimpleName(), updateString);
							if( logViewerHandler!=null )
								logViewerHandler.onNewLog(updateString);

							if(update.varName.contains("dr.")){
								int id = Integer.parseInt(update.varName.substring(3, 4));
								DroneObject drone = droneMap.get(id);
								if( drone == null ){
									drone = new DroneObject(id);
									droneMap.put(id, drone);
									if( mainActivityHandler!=null )
										mainActivityHandler.onNewDrone(drone);
								}
								drone.setValue(update.varName, update.value.toString());
							}
						}
					}
				});

			} catch (Exception e) {
				e.printStackTrace();
			}
			return null;
		}
	}

	private class SendMadaraMessage extends AsyncTask<String, Void, Void>{
		@Override
		protected Void doInBackground(String... params) {
			try {
				MadaraMessage msg = madara.createMessage();

				String varName = params[0];
				int type = Integer.parseInt(params[1]);
				if(type == MadaraType.DOUBLE)
					msg.addUpdate(varName, MadaraType.DOUBLE, Double.parseDouble(params[2]));
				else if(type == MadaraType.INTEGER)
					msg.addUpdate(varName, MadaraType.INTEGER, Integer.parseInt(params[2]));
				else
					msg.addUpdate(varName, MadaraType.STRING, params[2]);
				madara.send(msg);
				String updateString = System.currentTimeMillis() +"-SENT: " +  params[0] + " = " + params[2];
				log.add(updateString);
				Log.d(MadaraMobile.class.getSimpleName(), updateString);
				if( logViewerHandler!=null )
					logViewerHandler.onNewLog(updateString);
			} catch (Exception e) {
				e.printStackTrace();
			}
			return null;
		}
	}

}
