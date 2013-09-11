package edu.cmu.edu.madara.android.ui;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.google.android.gms.internal.di;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.Projection;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Polygon;
import com.google.android.gms.maps.model.PolygonOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
import com.google.android.gms.maps.model.TileOverlayOptions;

import edu.cmu.edu.madara.android.CustomMapTileProvider;
import edu.cmu.edu.madara.android.Drone;
import edu.cmu.edu.madara.android.MadaraMapFragment;
import edu.cmu.edu.madara.android.MadaraMapTouchListener;
import edu.cmu.edu.madara.android.R;
import edu.cmu.edu.madara.android.Thermal;
import edu.cmu.edu.madara.android.overlays.DroneOverlay;
import edu.cmu.edu.madara.android.overlays.ThermalOverlay;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnMultiChoiceClickListener;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;

public class MadaraMapActivity extends MadaraServiceActivity implements OnClickListener{

	public static final String TAG = MadaraMapActivity.class.getSimpleName();

	private MadaraMapFragment mapFragment;
	private GoogleMap mapView;
	private MadaraReaderThread madaraReaderThread;
	private DroneOverlay droneOverlay;
	private ThermalOverlay thermalOverlay;
	private MadaraMapTouchListener mapTouchListener;

	private EditText madaraEditText;
	private Button sendButton;

	private Button dronesButton;
	private Button actionsButton;

	private int firstx;
	private int firsty;
	private Polygon polygon;

	private List<String> droneIds;
	private List<String> selectedDrones;
	
	private AlertDialog alertDialog;
	
	private TextView latLngTextView;
	private LatLng mapCenter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.map_activity);
		
		latLngTextView = (TextView)findViewById(R.id.latlng_textView);

		madaraEditText = (EditText)findViewById(R.id.madara_message);
		sendButton = (Button)findViewById(R.id.send_button);
		sendButton.setOnClickListener(this);
		dronesButton = (Button)findViewById(R.id.drones_button);
		dronesButton.setOnClickListener(this);
		actionsButton = (Button)findViewById(R.id.actions_button);
		actionsButton.setOnClickListener(this);

		droneIds = new ArrayList<String>();
		selectedDrones = new ArrayList<String>();
		droneOverlay = new DroneOverlay();
		thermalOverlay = new ThermalOverlay();

		mapFragment = ((MadaraMapFragment)getFragmentManager().findFragmentById(R.id.map));
		mapView = mapFragment.getMap();
		mapView.setMyLocationEnabled(true);

		mapView.setMapType(GoogleMap.MAP_TYPE_NONE);
		TileOverlayOptions options = new TileOverlayOptions().tileProvider(new CustomMapTileProvider()).zIndex(0);
		mapView.addTileOverlay(options);


		mapFragment.addOnTouchListener( new MadaraMapTouchListener() {
			@Override
			public boolean onTouch(MotionEvent ev) {
				
				Display display = getWindowManager().getDefaultDisplay();
				Point size = new Point();
				display.getSize(size);
				Projection projection = mapView.getProjection();
				mapCenter = projection.fromScreenLocation(new Point(size.x/2, size.y/2));
				latLngTextView.setText(mapCenter.latitude+","+mapCenter.longitude);

				final int x = (int)ev.getX();
				final int y = (int)ev.getY();

				switch(ev.getAction()){

				case MotionEvent.ACTION_UP:
					Log.e(TAG, "up");
					return true;
				case MotionEvent.ACTION_DOWN:
					firstx = x;
					firsty = y;
					return true;
				case MotionEvent.ACTION_MOVE:

					if(polygon!=null){
						polygon.remove();

						polygon = mapView.addPolygon(new PolygonOptions()
						.add(mapView.getProjection().fromScreenLocation(new Point(firstx, firsty)),
								mapView.getProjection().fromScreenLocation(new Point(firstx, firsty+y)),
								mapView.getProjection().fromScreenLocation(new Point(x,y)),
								mapView.getProjection().fromScreenLocation(new Point(firstx+x,firsty)),
								mapView.getProjection().fromScreenLocation(new Point(firstx,firsty)))
								.strokeColor(Color.RED)
								.fillColor(Color.BLUE).zIndex(10));
					}

					return true;
				}

				return false;
			}
		});

		mapFragment.drawOn();

		/*		Polygon polygon = mapView.addPolygon(new PolygonOptions()
	     .add(new LatLng(0, 0), new LatLng(0, 5), new LatLng(3, 5), new LatLng(0, 0))
	     .strokeColor(Color.RED)
	     .fillColor(Color.BLUE));*/

		madaraReaderThread = new MadaraReaderThread();
		madaraReaderThread.start();


				Polygon polygon = mapView.addPolygon(new PolygonOptions()
	     .add(new LatLng(40.441182, -79.947039),new LatLng(40.441161, -79.946625),new LatLng(
	    		 40.440790, -79.946607),new LatLng(
	    				 40.440811, -79.947098),new LatLng(40.441182, -79.947039))
	     .strokeColor(Color.argb(100, 0, 0, 255))
	     .fillColor(Color.argb(50, 0, 0, 255)).strokeWidth(5f).zIndex(10));
		
/*		Polyline polyline = mapView.addPolyline( new PolylineOptions()
		.add(new LatLng(40.441182, -79.947039),new LatLng(40.441161, -79.946625))
		.color(Color.RED)
		.width(1f)
		.zIndex(10)
		);*/

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

	class MadaraReaderThread extends Thread {

		private boolean running;

		@Override
		public void run() {
			running = true;

			while(running){
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

				final HashMap<String, Drone> droneMap = binder.getDrones();
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						for(final Drone drone: droneMap.values()){
							Log.e(TAG, drone.getId());
							droneOverlay.updateDroneMarkers(drone, mapView);
						}
					}
				});
				final HashMap<String, Thermal> thermals = binder.getThermals();
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						for(final String key: thermals.keySet()){
							Thermal thermal = thermals.get(key);
							thermalOverlay.updateThermalOverlays(key, thermal, mapView);
						}
					}
				});
			}
		}

		public boolean isRunning(){
			return running && isAlive();
		}

		public void stopRunning(){
			running = false;
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		madaraReaderThread = new MadaraReaderThread();
		madaraReaderThread.start();
	}

	@Override
	protected void onPause() {
		super.onPause();
		if(madaraReaderThread!=null && madaraReaderThread.isRunning()){
			madaraReaderThread.stopRunning();
		}
		if(alertDialog!=null && alertDialog.isShowing())
			alertDialog.dismiss();
	}

	public class TouchableWrapper extends FrameLayout {

		public TouchableWrapper(Context context) {
			super(context);
		}

		@Override
		public boolean dispatchTouchEvent(MotionEvent ev) {
			switch (ev.getAction()) {
			case MotionEvent.ACTION_DOWN:
				Log.e(TAG, "down");
				break;

			case MotionEvent.ACTION_UP:
				Log.e(TAG, "up");
				break;
			}

			return super.dispatchTouchEvent(ev);
		}
	}

	@Override
	public void onClick(View v) {

		if(v.equals(sendButton)){
			binder.sendMadaraMessage(madaraEditText.getText().toString());
		}
		else if(v.equals(actionsButton)){
			
			CharSequence[] items = new CharSequence[]{"Takeoff", "Land", "Go to GPS", "Area Coverage", "Bridging"};
			
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setTitle("Select an Action");
			builder.setItems(items, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					switch(which){
					case 0: // TAKEOFF
						for(String droneId: selectedDrones ){
							binder.sendMadaraMessage(droneId+".movement_command=\"takeoff\"");
						}
						break;
					case 1: //LAND
						for(String droneId: selectedDrones ){
							binder.sendMadaraMessage(droneId+".movement_command=\"land\"");
						}
						break;
					case 2: //GO TO GPS
						break;
					case 3: //AREA COVERAGE
						
						dialog.dismiss();
						
						CharSequence[] items = new CharSequence[]{"Random", "Cell Decomposition"};
						
						AlertDialog.Builder builder = new AlertDialog.Builder(MadaraMapActivity.this);
						builder.setTitle("Choose an algorithm");
						builder.setItems(items, new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								
								switch( which ){
								case 0: //RANDOM
									break;
								case 1: //CELL DECOMP	
									break;
								}
							}
						});
						alertDialog = builder.create();
						alertDialog.show();
						
						break;
					case 4: // BRIDGING
						break;
					}
				}
			});
			alertDialog = builder.create();
			alertDialog.show();
		}
		else if(v.equals(dronesButton)){
			
			droneIds = droneOverlay.getDroneIds();
			AlertDialog.Builder builder = new AlertDialog.Builder(MadaraMapActivity.this);
			CharSequence[] items = new CharSequence[droneIds.size()];
			boolean[] checkedItems = new boolean[droneIds.size()];
			for(int i=0; i< droneIds.size(); i++){
				items[i] = droneIds.get(i);
				if(selectedDrones.contains(droneIds.get(i)))
					checkedItems[i] = true;
				else
					checkedItems[i] = false;
			}
			builder.setMultiChoiceItems(items, checkedItems, new OnMultiChoiceClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which, boolean isChecked) {
					if(isChecked){
						selectedDrones.add(droneIds.get(which));
					}
					else{
						selectedDrones.remove(droneIds.get(which));
					}
				}
			});
			builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if(dialog!=null)
						dialog.dismiss();
				}
			});
			alertDialog = builder.create();
			alertDialog.show();
		}
	}
}
