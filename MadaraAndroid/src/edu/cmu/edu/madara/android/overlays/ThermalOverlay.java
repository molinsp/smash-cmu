package edu.cmu.edu.madara.android.overlays;

import java.util.HashMap;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import edu.cmu.edu.madara.android.R;
import edu.cmu.edu.madara.android.model.Thermal;

public class ThermalOverlay {
	
	private HashMap<String, Thermal> thermals;
	private HashMap<String, Marker> thermalMarkers;
	private BitmapDescriptor icon = BitmapDescriptorFactory.fromResource(R.drawable.asset_marker_distress2);
	
	public ThermalOverlay(){
		thermals = new HashMap<String, Thermal>();
		thermalMarkers = new HashMap<String, Marker>();
	}
	
	public synchronized void updateThermalOverlays(String key, Thermal thermal, GoogleMap map){
		
		thermals.put(key, thermal);
		
		if(!thermalMarkers.containsKey(key)){
			Marker marker = map.addMarker(new MarkerOptions()
			.title(thermal.getHits()+"")
			.snippet("")
			.position(new LatLng(thermal.getLat(), thermal.getLon()))
			.anchor(0.5f, 0.5f));
			marker.setIcon(icon);
			thermalMarkers.put(key, marker);
		}
		else{
			Marker marker = thermalMarkers.get(key);
			marker.setPosition(new LatLng(thermal.getLat(), thermal.getLon()));
		}
		
	}

}
