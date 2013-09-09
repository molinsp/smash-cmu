/*package edu.cmu.edu.madara.android.ui;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.MyLocationOverlay;
import edu.cmu.edu.madara.android.MadaraService;
import edu.cmu.edu.madara.android.MadaraService.MadaraServiceBinder;
import edu.cmu.edu.madara.android.R;
import edu.cmu.edu.madara.android.overlays.DroneItemizedOverlay;
import edu.cmu.edu.madara.android.overlays.MyRectangleOverlay;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.LinearLayout;

public class MainActivity extends MapActivity implements OnClickListener{

	private MapView mapView;
	private MapController mapController;
	private MyLocationOverlay myLocOverlay;
	private OnTouchListener mapTouchListener;
	private boolean selectionMode;
	private MyRectangleOverlay rectOverlay;
	private LinearLayout controllerLayout;

	private Button deselectButton;
	private Button selectionButton;
	private Button areaCoverageButton;

	private DroneItemizedOverlay droneItemziedOverlay;

	private boolean dronesSelected;
	private boolean areaCoverageMode;

	private ServiceConnection conn;
	private MadaraServiceBinder binder;

	//private EvalSettings evalSettings;



	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		//evalSettings = new EvalSettings();

		Settings settings = new Settings();
		settings.setHosts(new String[]{"192.168.168.212:8080"});
		settings.setType(TransportType.MULTICAST_TRANSPORT);

		//free knowledge record when not using evaluateNoReturn
		//KnowledgeBase knowledge = new KnowledgeBase("genes-phone", settings);

		//knowledge.evaluateNoReturn("device.1.location='30.5432543,-79.4384'",evalSettings);


		initViews();

		mapController = mapView.getController();
		myLocOverlay = new MyLocationOverlay(this, mapView);
		myLocOverlay.enableMyLocation();
		myLocOverlay.enableCompass();
		mapView.getOverlays().add(myLocOverlay);
		mapView.setSatellite(true);

		mapController.setCenter(new GeoPoint(35717894, -120767750));
		mapController.setZoom(17);

		droneItemziedOverlay = new DroneItemizedOverlay(this){
			@Override
			public void onTapCallback() {
				dronesSelected = droneItemziedOverlay.dronesSelected();
				toggleControlLayout();
			}
		};
		//droneItemziedOverlay.addDroneOverlay( new DroneOverlay(new GeoPoint(35719592, -120769306), "", ""));

		//DroneOverlay selectedDroneOverlay = new DroneOverlay(new GeoPoint(35719026, -120766666), "", "");
		//selectedDroneOverlay.setSelected(true);
		//droneItemziedOverlay.addDroneOverlay( selectedDroneOverlay );

		//add only after adding drone overlays
		//mapView.getOverlays().add(droneItemziedOverlay);

		//startService(new Intent(this, MadaraService.class));

		//MadaraReaderThread readerThreader = new MadaraReaderThread();
		//readerThreader.start();

	}
	
	@Override
	protected void onResume() {
		
		conn = new ServiceConnection() {
			@Override
			public void onServiceDisconnected(ComponentName name) {

			}

			@Override
			public void onServiceConnected(ComponentName name, IBinder service) {
				binder = (MadaraServiceBinder) service;
			}
		};

		bindService(new Intent(this, MadaraService.class), conn, Context.BIND_AUTO_CREATE);
		
		
		super.onResume();
	}
	
	@Override
	protected void onPause() {
		
		if(conn!=null){
			unbindService(conn);
		}
		super.onPause();
	}

	public void freeEvalSettings(){
		//evalSettings.free();
	}

	@Override
	protected void onDestroy() {
		freeEvalSettings();
		super.onDestroy();
	}

	public void initViews(){
		mapView = (MapView)findViewById(R.id.mapview);
		controllerLayout = (LinearLayout)findViewById(R.id.controller_layout);
		deselectButton = (Button)findViewById(R.id.deselect_button);
		deselectButton.setOnClickListener(this);
		selectionButton = (Button)findViewById(R.id.select_button);
		selectionButton.setOnClickListener(this);
		areaCoverageButton = (Button)findViewById(R.id.areacoverage_button);
		areaCoverageButton.setOnClickListener(this);
		initMapTouchListener();
		mapView.setOnTouchListener(mapTouchListener);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		
		switch(item.getItemId()){
		
		case R.id.action_settings:
			startActivity(new Intent(this, MadaraSettingsActivity.class));
			return true;
			
		case R.id.action_log:
			startActivity(new Intent(this, MadaraLogActivity.class));
			return true;
		
		}
		
		return super.onMenuItemSelected(featureId, item);
	}

	@Override
	protected boolean isRouteDisplayed() {
		return false;
	}

	public void initMapTouchListener(){
		mapTouchListener =  new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				if( selectionMode ){
					final int EVENT_ACTION = event.getAction();
					final int x = (int)event.getX();
					final int y = (int)event.getY();
					final GeoPoint geoPoint = mapView.getProjection().fromPixels(x, y);
					switch(EVENT_ACTION){
					case MotionEvent.ACTION_DOWN: 
						if( rectOverlay != null )
							mapView.getOverlays().remove( rectOverlay );

						rectOverlay = new MyRectangleOverlay( geoPoint, geoPoint );
						mapView.getOverlays().add( rectOverlay );
						mapView.invalidate();
						return true;
					case MotionEvent.ACTION_MOVE:
						if( rectOverlay != null ){
							rectOverlay.setBottomRight( geoPoint );
							mapView.invalidate();
						}
						return true;
					case MotionEvent.ACTION_UP:
						Log.e("rect", rectOverlay.getTopLeft() +"," + rectOverlay.getBottomRight() );
						droneItemziedOverlay.setSelectedDrones(rectOverlay.getTopLeft(), rectOverlay.getBottomRight(), mapView);
						mapView.getOverlays().remove( rectOverlay );
						selectionMode = false;
						dronesSelected = droneItemziedOverlay.dronesSelected();
						toggleControlLayout();
						return true;
					default:
						return false;
					}
				}
				if( areaCoverageMode ){
					final int EVENT_ACTION = event.getAction();
					final int x = (int)event.getX();
					final int y = (int)event.getY();
					final GeoPoint geoPoint = mapView.getProjection().fromPixels(x, y);
					switch(EVENT_ACTION){
					case MotionEvent.ACTION_DOWN: 
						if( rectOverlay != null )
							mapView.getOverlays().remove( rectOverlay );

						rectOverlay = new MyRectangleOverlay( geoPoint, geoPoint );
						mapView.getOverlays().add( rectOverlay );
						mapView.invalidate();
						return true;
					case MotionEvent.ACTION_MOVE:
						if( rectOverlay != null ){
							rectOverlay.setBottomRight( geoPoint );
							mapView.invalidate();
						}
						return true;
											case MotionEvent.ACTION_UP:
						Log.e("rect", rectOverlay.getTopLeft() +"," + rectOverlay.getBottomRight() );
						droneItemziedOverlay.setSelectedDrones(rectOverlay.getTopLeft(), rectOverlay.getBottomRight(), mapView);
						mapView.getOverlays().remove( rectOverlay );
						selectionMode = false;
						dronesSelected = droneItemziedOverlay.dronesSelected();
						toggleControlLayout();
						return true;
					default:
						return false;
					}
				}
				else
					return false;
			}
		};
	}

	public void toggleControlLayout(){
		if(dronesSelected){
			controllerLayout.setVisibility(View.VISIBLE);
		}
		else{
			controllerLayout.setVisibility(View.GONE);
		}
	}

	class MadaraReaderThread extends Thread{
		@Override
		public void run() {

			while(true){

				try {
					Thread.sleep(1000);
					Log.e("TAG", "binder is " + binder);
					if(binder!=null){
						runOnUiThread( new Runnable() {
							@Override
							public void run() {
								mapView.getOverlays().remove(droneItemziedOverlay);
								droneItemziedOverlay.clear();


								final ConcurrentHashMap<String, Object> madaraVariables = binder.getMadaraVariableMap();
								Log.e("TAG", "madara var size = " + madaraVariables.size());
								for(String key: madaraVariables.keySet()){
									if(key.contains(".location")&&!key.contains("sim")){
										String loc = madaraVariables.get(key);
										if(loc!=null){
											String[] split = loc.split(",");
											double lat = Double.parseDouble(split[0]);
											double lon = Double.parseDouble(split[1]);
											int latInt = (int)(lat*1E6);
											int lonInt = (int)(lon*1E6);
											Log.e("TAG", key + " " + latInt + ", " + lonInt);
											DroneOverlay overlay = new DroneOverlay(new GeoPoint(latInt, lonInt), "", "");
											droneItemziedOverlay.addDroneOverlay(overlay);
										}
									}
								}
								if(droneItemziedOverlay.size()!=0)
									mapView.getOverlays().add(droneItemziedOverlay);
							}
						});
					}

				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}

	}

	@Override
	public void onClick(View v) {

		if(v.getId()==deselectButton.getId()){
			droneItemziedOverlay.deselectAllDrones();
			dronesSelected = false;
			toggleControlLayout();
			mapView.invalidate();
		}
		else if(v.getId()==selectionButton.getId()){
			areaCoverageMode = false;
			selectionMode = true;
		}
		else if(v.getId()==areaCoverageButton.getId()){
			selectionMode = false;
			areaCoverageMode = true;
		}
	}


}
*/