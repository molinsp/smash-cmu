package edu.cmu.edu.madara.android;

import android.app.Application;
import android.content.Intent;

public class MadaraAndroidApplication extends Application {
	
	@Override
	public void onCreate() {
		super.onCreate();
		
		startService(new Intent(this, MadaraService.class));
		
	}

}
