package edu.cmu.edu.madara.android;

import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.madara.EvalSettings;
import com.madara.KnowledgeBase;
import com.madara.KnowledgeMap;
import com.madara.KnowledgeRecord;
import com.madara.KnowledgeType;
import com.madara.transport.Settings;
import com.madara.transport.TransportType;
import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Binder;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.util.Log;

//TODO - Create a MadaraWriterThread that will update the Madara knowledge base with its own status
//TODO - Support every type of Madara variable
//TODO - Figure out why knowledge map continuously gets returned even after the drones stop sending updates

public class MadaraService extends Service {

	public static final String TAG = MadaraService.class.getSimpleName();

	private String ipAddress;
	private int port;
	private String hostName;
	private TransportType transportType;

	private int madaraReadInterval = 1000;

	private Settings madaraSettings;
	private KnowledgeBase knowledge;

	private HashMap<String, Object> madaraVariables;
	private HashMap<String, Drone> drones;
	private Binder madaraServiceBinder;
	private MadaraReaderThread madaraReaderThread;

	@Override
	public void onCreate() {
		super.onCreate();

		Log.d(TAG, "oncreate called");
		madaraServiceBinder = new MadaraServiceBinder();

	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {

		madaraVariables = new HashMap<String, Object>();
		drones = new HashMap<String, Drone>();

		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);

		hostName = prefs.getString("device_id", "device.3");
		ipAddress = prefs.getString("ip_address", "239.255.0.1");
		Log.d(TAG, "oncreate called ip address = " + ipAddress);
		port = Integer.parseInt(prefs.getString("port_number", "4150"));
		int transportTypeInt = Integer.parseInt(prefs.getString("transport_type", "5"));
		transportType = TransportType.getType(transportTypeInt);

		if(madaraSettings!=null){
			//madaraSettings.free();
		}
		madaraSettings = new Settings();
		madaraSettings.setHosts(new String[]{ipAddress+":"+port});
		madaraSettings.setType(transportType);

		if(knowledge!=null){
			knowledge.free();
		}

		knowledge = new KnowledgeBase(hostName, madaraSettings);

		if(madaraReaderThread!=null && madaraReaderThread.isRunning()){
			madaraReaderThread.stopRunning();
		}

		madaraReaderThread = new MadaraReaderThread();
		madaraReaderThread.start();

		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		if(madaraReaderThread!=null)
			madaraReaderThread.stopRunning();

		//TODO - free knowledge base and madaraSettings
		synchronized (knowledge) {
			knowledge.free();
		}
		synchronized (madaraSettings) {
			//madaraSettings.free();
		}
		super.onDestroy();
	}


	@Override
	public IBinder onBind(Intent intent) {
		return madaraServiceBinder;
	}

	@Override
	public boolean onUnbind(Intent intent) {
		return super.onUnbind(intent);
	}


	private class MadaraReaderThread extends Thread {
		private boolean running;
		@Override
		public void run() {
			running = true;

			Pattern pattern = Pattern.compile("(device\\.[0-9]+)\\.(.*)");

			while(running){
				try {
					Thread.sleep(madaraReadInterval);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

				synchronized (knowledge) {

					KnowledgeMap knowledgeMap = knowledge.toKnowledgeMap("*");
					for(String key: knowledgeMap.keySet()){

						//Grab the current record from the knowledge map
						KnowledgeRecord record = knowledgeMap.get(key);

						//Update the Madara variables map
						switch( record.getType() ){
						case INTEGER:
							madaraVariables.put(key, record.toLongValue());
							Log.d(TAG, key+"="+record.toLongValue());
							break;
						case DOUBLE:
							madaraVariables.put(key, record.toDoubleValue());
							Log.d(TAG, key+"="+record.toDoubleValue());
							break;
						case STRING:
							madaraVariables.put(key, record.toStringValue());
							Log.d(TAG, key+"="+record.toStringValue());
							break;
							//TODO - support for other data types
						default:
							break;
						}

						//Update the drones map
						//Assume that all "device.{X}" items are drones for now
						if(key.startsWith("device.")){
							Matcher matcher = pattern.matcher(key);
							matcher.matches();
							String droneId = null;
							String variable = null;
							if(matcher.groupCount()==2){
								droneId = matcher.group(1);
								variable = matcher.group(2);
							}
							else{
								continue;
							}

							Drone drone = drones.get(droneId);
							if(drone==null){
								drone = new Drone();
								drone.setId(droneId);
								drones.put(droneId, drone);
							}

							//TODO - location will be an array and not a string
							if(variable.equals(MadaraConstants.LOCATION)){
								String split[] = record.toStringValue().split(",");
								drone.setLatitude(Double.parseDouble(split[0]));
								drone.setLongitude(Double.parseDouble(split[1]));
							}
							else if(variable.equals(MadaraConstants.MOBILE)){
								drone.setMobile(record.toLongValue());
							}
							else if(variable.equals(MadaraConstants.BATTERY)){
								drone.setBattery(record.toLongValue());
							}
							else if(variable.equals(MadaraConstants.BUSY)){
								drone.setBusy( record.toLongValue() == 1 );
							}
							else if(variable.equals(MadaraConstants.BRIDGE_ID)){
								drone.setBridgeId(record.toLongValue());
							}
							else if(variable.equals(MadaraConstants.AREA_COVERAGE_REQUESTED)){
								drone.setAreaCoverageRequested(record.toStringValue());
							}
							else if(variable.equals(MadaraConstants.NEXT_AREA_COVERAGE_REQUESTED)){
								drone.setNextAreaCoverageRequested(record.toStringValue());
							}
							else if(variable.equals(MadaraConstants.SEARCH_AREA_ID)){
								drone.setSearchAreaId(record.toLongValue());
							}
							else if(variable.equals(MadaraConstants.MOVEMENT_COMMAND)){
								drone.setMovementCommand(record.toStringValue());
							}
						}
						else{
							//do nothing right now
						}
					}
					knowledgeMap.free();
				}
			}
		}

		public boolean isRunning(){
			return running && isAlive();
		}

		public void stopRunning(){
			this.running = false;
		}
	}

	public class MadaraServiceBinder extends Binder{

		public HashMap<String, Object> getMadaraVariableMap(){
			return madaraVariables;
		}

		public HashMap<String, Drone> getDrones(){
			return drones;
		}
		
		public void sendMadaraMessage(String message){
			EvalSettings evalSettings = new EvalSettings();
			knowledge.evaluateNoReturn(message, evalSettings);
			evalSettings.free();
		}
	}
}
