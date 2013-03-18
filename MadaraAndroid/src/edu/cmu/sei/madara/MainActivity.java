/**
 *	Copyright (c) 2013 Carnegie Mellon University.
 *	All Rights Reserved.
 *	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following acknowledgments and disclaimers.
 *	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *	3. The names �Carnegie Mellon University,� "SEI� and/or �Software Engineering Institute" shall not be used to endorse or promote products derived from this software without prior written permission. For written permission, please contact permission@sei.cmu.edu.
 *	4. Products derived from this software may not be called "SEI" nor may "SEI" appear in their names without prior written permission of permission@sei.cmu.edu.
 *	5. Redistributions of any form whatsoever must retain the following acknowledgment:
 *	This material is based upon work funded and supported by the Department of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University for the operation of the Software Engineering Institute, a federally funded research and development center.
 *	Any opinions, findings and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the United States Department of Defense.
 *	NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN �AS-IS� BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *	This material has been approved for public release and unlimited distribution.
 *	DM-0000245
 */

package edu.cmu.sei.madara;

import java.util.ArrayList;
import java.util.List;
import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.MyLocationOverlay;
import com.google.android.maps.Projection;
import edu.cmu.sei.madara.Madara.MadaraType;
import edu.cmu.sei.madara.MadaraMobile.MadaraHandler;
import android.os.Bundle;
import android.content.Intent;
import android.graphics.Point;
import android.view.Display;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends MapActivity implements OnClickListener{

	private EditText madaraVarEditText;
	private EditText madaraValEditText;
	private Spinner madaraSpinner;
	private Button sendMadaraMessageButton;
	private TextView latLongTextView;
	private Button takeOffButton;
	private Button landButton;
	private Button upButton;
	private Button downButton;
	private Button rightButton;
	private Button leftButton;
	private Button forwardButton;
	private Button backwardButton;
	private Spinner droneSpinner;
	private ArrayAdapter<DroneObject> droneSpinnerAdapter;
	private MadaraHandler madaraHandler;
	private GeoPoint mapCenter;
	private MapView mapView;
	private MapController mapController;
	private MyLocationOverlay myLocOverlay;
	private OnTouchListener mapTouchListener;
	private boolean drawMode;
	private MyRectangleOverlay rectOverlay;
	private DronesOverlay dronesOverlay;
	private ImageView crossHair;
	private MainActivityHandler mainActivityHandler;
	private List<DroneObject> drones;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		madaraHandler = ((MadaraMobile)getApplication()).getHandler();

		mapView = (MapView)findViewById(R.id.mapview);
		mapView.setBuiltInZoomControls(false);
		mapController = mapView.getController();
		myLocOverlay = new MyLocationOverlay( this, mapView );
		myLocOverlay.enableMyLocation();
		mapView.getOverlays().add(myLocOverlay);

		latLongTextView =(TextView)findViewById(R.id.latlontextview);
		madaraValEditText = (EditText)findViewById(R.id.madara_val_edittext);
		madaraSpinner = (Spinner)findViewById(R.id.madara_spinner);
		madaraVarEditText = (EditText)findViewById(R.id.madara_var_edittext);
		sendMadaraMessageButton = (Button)findViewById(R.id.send_button);
		sendMadaraMessageButton.setOnClickListener(this);
		crossHair = (ImageView)findViewById(R.id.cross_hair);
		
		takeOffButton = (Button)findViewById(R.id.takeoff_button);
		takeOffButton.setOnClickListener(this);
		landButton = (Button)findViewById(R.id.land_button);
		landButton.setOnClickListener(this);
		upButton = (Button)findViewById(R.id.up_button);
		upButton.setOnClickListener(this);
		downButton = (Button)findViewById(R.id.down_button);
		downButton.setOnClickListener(this);
		rightButton = (Button)findViewById(R.id.right_button);
		rightButton.setOnClickListener(this);
		leftButton = (Button)findViewById(R.id.left_button);
		leftButton.setOnClickListener(this);
		forwardButton = (Button)findViewById(R.id.forward_button);
		forwardButton.setOnClickListener(this);
		backwardButton = (Button)findViewById(R.id.backwars_button);
		backwardButton.setOnClickListener(this);
		
		droneSpinner = (Spinner)findViewById(R.id.drone_spinner);

		String[] madaraTypes = {"Madara.INTEGER", "Madara.STRING", "Madara.DOUBLE"};
		ArrayAdapter<String> spinnerAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, madaraTypes);
		spinnerAdapter.setDropDownViewResource(android.R.layout.simple_dropdown_item_1line);
		madaraSpinner.setAdapter( spinnerAdapter );
		
		initMapTouchListener();
		mapView.setOnTouchListener(mapTouchListener);
		
		dronesOverlay = new DronesOverlay(madaraHandler);
		mapView.getOverlays().add(dronesOverlay);
		
		drones = new ArrayList<DroneObject>();
		
		drones.add(new AllDronesObject());
		
		//dummy test
		drones.add(new DroneObject(1));
		drones.add(new DroneObject(2));
		drones.add(new DroneObject(3));
		drones.add(new DroneObject(4));
		
		drones.addAll(madaraHandler.getDrones());
		
		
		droneSpinnerAdapter = new ArrayAdapter<DroneObject>(this, android.R.layout.simple_spinner_item, drones);
		droneSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_dropdown_item_1line);
		droneSpinner.setAdapter(droneSpinnerAdapter);
		
		
		mainActivityHandler = new MainActivityHandler() {
			@Override
			public void onNewDrone(DroneObject drone) {
				drones.clear();
				drones.add(new AllDronesObject());
				drones.addAll(madaraHandler.getDrones());
				droneSpinnerAdapter.notifyDataSetChanged();
			}
		};
		madaraHandler.addMainActivityHandler(mainActivityHandler);
	}

	@Override
	public boolean dispatchTouchEvent(MotionEvent ev) {
		Display display = getWindowManager().getDefaultDisplay();
		Point size = new Point();
		display.getSize(size);
		Projection p = mapView.getProjection();
		mapCenter = p.fromPixels(size.x/2, size.y/2);
		updateLatLongTextView( mapCenter );
		return super.dispatchTouchEvent(ev);
	}

	protected void updateLatLongTextView( GeoPoint geoPoint ){
		double lat = geoPoint.getLatitudeE6() / 1E6;
		double lon = geoPoint.getLongitudeE6() / 1E6;
		latLongTextView.setText( lat + ", " + lon );
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if(item.getItemId()==R.id.center_map){
			centerMap();
			return true;
		}
		else if(item.getItemId()==R.id.satellite){
			if(!item.isChecked()){
				item.setChecked(true);
				mapView.setSatellite(true);
			}
			else{
				item.setChecked(false);
				mapView.setSatellite(false);
			}
			return true;
		}
		else if(item.getItemId()==R.id.bounding_box){
			if(drawMode){
				drawMode=false;
				mapView.getOverlays().remove(rectOverlay);
			}
			else{
				drawMode=true;
			}
			return true;
		}
		else if(item.getItemId()==R.id.crosshair_menu){
			if(item.isChecked()){
				crossHair.setVisibility(View.GONE);
				item.setChecked(false);
			}
			else{
				crossHair.setVisibility(View.VISIBLE);
				item.setChecked(true);
			}
		}
		else if(item.getItemId()==R.id.log_viewer){
			startActivity( new Intent(MainActivity.this, LogViewer.class));
		}
		return false;
	}

	@Override
	public void onClick(View v) {
		
		String droneString = "";
		DroneObject selected = (DroneObject)droneSpinner.getSelectedItem();
		if(selected.getId()!=-1)
			droneString = "dr." + selected.getId();
		
		if( v.equals(sendMadaraMessageButton)){
			String var = madaraVarEditText.getText().toString();
			int type = madaraSpinner.getSelectedItemPosition();
			String val = madaraValEditText.getText().toString();
			sendMadaraMessage(var, type, val);
		}
		else if(v.equals(takeOffButton)){
			sendMadaraMessage("swarm.takeoff", MadaraType.INTEGER, "1");
		}
		else if(v.equals(landButton)){
			sendMadaraMessage("swarm.land", MadaraType.INTEGER, "1");
		}
		else if(v.equals(upButton)){
			sendMadaraMessage("swarm.up", MadaraType.INTEGER, "1");
		}
		else if(v.equals(downButton)){
			sendMadaraMessage("swarm.down", MadaraType.INTEGER, "1");
		}
		else if(v.equals(rightButton)){
			sendMadaraMessage("swarm.right", MadaraType.INTEGER, "1");
		}
		else if(v.equals(leftButton)){
			sendMadaraMessage("swarm.left", MadaraType.INTEGER, "1");
		}
		else if(v.equals(forwardButton)){
			sendMadaraMessage("swarm.forward", MadaraType.INTEGER, "1");
		}
		else if(v.equals(backwardButton)){
			sendMadaraMessage("swarm.backward", MadaraType.INTEGER, "1");
		}
	}

	
	@Override
	protected boolean isRouteDisplayed() {
		// TODO Auto-generated method stub
		return false;
	}

	public void centerMap(){
		if(myLocOverlay.getMyLocation()!=null){
			mapController.animateTo(myLocOverlay.getMyLocation());
			mapController.setZoom(17);
		}
		else
			Toast.makeText(this, "No location fix.", Toast.LENGTH_SHORT).show();
	}

	public void initMapTouchListener(){
		mapTouchListener =  new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				if( drawMode ){
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
					default:
						return false;
					}
				}
			else
				return false;
		}
	};
}
	
	public void sendMadaraMessage(String var, int type, String val){
		madaraHandler.sendMadaraUpdate(var, type, val);
	}
	
	public abstract class MainActivityHandler{
		
		public abstract void onNewDrone(DroneObject drone);
		
	}
}
