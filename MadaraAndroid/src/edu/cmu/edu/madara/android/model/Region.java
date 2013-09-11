package edu.cmu.edu.madara.android.model;

import com.google.android.gms.maps.model.LatLng;

public class Region {
	
	private String id;
	private long type = 0; //RECTANGLE
	private LatLng topLeft;
	private LatLng bottomRight;
	
	
	public String getId() {
		return id;
	}
	public void setId(String id) {
		this.id = id;
	}
	public long getType() {
		return type;
	}
	public void setType(long type) {
		this.type = type;
	}
	public LatLng getTopLeft() {
		return topLeft;
	}
	public void setTopLeft(LatLng topLeft) {
		this.topLeft = topLeft;
	}
	public LatLng getBottomRight() {
		return bottomRight;
	}
	public void setBottomRight(LatLng bottomRight) {
		this.bottomRight = bottomRight;
	}
	
}
