package edu.cmu.edu.madara.android.ui;

import edu.cmu.edu.madara.android.MadaraService;
import edu.cmu.edu.madara.android.MadaraService.MadaraServiceBinder;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

public class MadaraServiceActivity extends Activity {
	
	private ServiceConnection serviceConnection;
	protected MadaraServiceBinder binder;
	
	@Override
	protected void onResume() {
		serviceConnection = new ServiceConnection() {
			@Override
			public void onServiceDisconnected(ComponentName name) {}

			@Override
			public void onServiceConnected(ComponentName name, IBinder service) {
				binder = (MadaraServiceBinder) service;
			}
		};
		bindService(new Intent(this, MadaraService.class), serviceConnection, Context.BIND_AUTO_CREATE);
		super.onResume();
	}
	
	@Override
	protected void onPause() {
		if(serviceConnection!=null){
			unbindService(serviceConnection);
		}
		super.onPause();
	}

}
