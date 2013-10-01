package edu.cmu.edu.madara.android.overlays;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import edu.cmu.edu.madara.android.R;
import edu.cmu.edu.madara.android.model.Drone;


public class DroneOverlay {
	
	private HashMap<String, Drone> droneMap;
	private HashMap<String, Marker> droneMarkers;
	private BitmapDescriptor icon = BitmapDescriptorFactory.fromResource(R.drawable.asset_marker2);
	
	public DroneOverlay(){
		droneMap = new HashMap<String, Drone>();
		droneMarkers = new HashMap<String, Marker>();
	}
	
	public synchronized void updateDroneMarkers(Drone drone, GoogleMap map){
		
/*		if(drone.getId().equals("device.3"))
			return;*/
		
		
		droneMap.put(drone.getId(), drone);
		
		if(!droneMarkers.containsKey(drone.getId())){
			Marker marker = map.addMarker(new MarkerOptions()
			.title(drone.getId())
			.snippet("")
			.position(new LatLng(drone.getLatitude(), drone.getLongitude()))
			.anchor(0.5f, 0.5f));
			marker.setIcon(icon);
			droneMarkers.put(drone.getId(), marker);
		}
		else{
			Marker marker = droneMarkers.get(drone.getId());
			marker.setPosition(new LatLng(drone.getLatitude(), drone.getLongitude()));
		}
	}
	
	public List<String> getDroneIds(){
		List<String> droneIds = new ArrayList<String>();
		for(String key: droneMap.keySet()){
			droneIds.add(key);
		}
		return droneIds;
	}
}
