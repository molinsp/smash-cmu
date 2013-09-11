package edu.cmu.edu.madara.android;

public class Thermal {
	
	private double lat;
	private double lon;
	private long hits;
	
	public Thermal(double lat, double lon, long hits){
		this.lat = lat;
		this.lon = lon;
		this.hits = hits;
	}
	
	public long getHits() {
		return hits;
	}
	public void setHits(long hits) {
		this.hits = hits;
	}
	
	public double getLat() {
		return lat;
	}
	public void setLat(double lat) {
		this.lat = lat;
	}
	public double getLon() {
		return lon;
	}
	public void setLon(double lon) {
		this.lon = lon;
	}
}
