package edu.cmu.edu.madara.android;

public class Drone {
	
	private String id;
	private double latitude;
	private double longitude;
	private double altitude;
	private long mobile;
	private long battery;
	private boolean busy;
	private long bridgeId;
	private String areaCoverageRequested;
	private String nextAreaCoverageRequested;
	private long searchAreaId;
	private String movementCommand;
	
	public String getId() {
		return id;
	}
	
	public void setId(String id) {
		this.id = id;
	}
	
	public double getLatitude() {
		return latitude;
	}
	
	public void setLatitude(double latitude) {
		this.latitude = latitude;
	}
	
	public double getLongitude() {
		return longitude;
	}
	
	public void setLongitude(double longitude) {
		this.longitude = longitude;
	}
	
	public double getAltitude() {
		return altitude;
	}
	
	public void setAltitude(double altitude) {
		this.altitude = altitude;
	}
	
	public long getMobile() {
		return mobile;
	}
	
	public void setMobile(long mobile) {
		this.mobile = mobile;
	}
	
	public long getBattery() {
		return battery;
	}
	
	public void setBattery(long battery) {
		this.battery = battery;
	}
	
	public boolean isBusy() {
		return busy;
	}
	
	public void setBusy(boolean busy) {
		this.busy = busy;
	}
	
	public long getBridgeId() {
		return bridgeId;
	}
	
	public void setBridgeId(long bridgeId) {
		this.bridgeId = bridgeId;
	}
	
	public String getAreaCoverageRequested() {
		return areaCoverageRequested;
	}
	
	public void setAreaCoverageRequested(String areaCoverageRequested) {
		this.areaCoverageRequested = areaCoverageRequested;
	}
	
	public String getNextAreaCoverageRequested() {
		return nextAreaCoverageRequested;
	}
	
	public void setNextAreaCoverageRequested(String nextAreaCoverageRequested) {
		this.nextAreaCoverageRequested = nextAreaCoverageRequested;
	}
	
	public long getSearchAreaId() {
		return searchAreaId;
	}
	
	public void setSearchAreaId(long searchAreaId) {
		this.searchAreaId = searchAreaId;
	}
	
	public String getMovementCommand() {
		return movementCommand;
	}
	
	public void setMovementCommand(String movementCommand) {
		this.movementCommand = movementCommand;
	}
	
}
