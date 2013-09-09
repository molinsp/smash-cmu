package edu.cmu.edu.madara.android.ui;

import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;
import edu.cmu.edu.madara.android.R;
import android.os.Bundle;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;


//TODO - Create a buffer so an infinite amount of TextViews are not created


public class MadaraLogActivity extends MadaraServiceActivity {
	
	private LinearLayout logContainer;
	private ScrollView scroll;
	private CheckBox scrollLockCheckBox;
	private MadaraReaderThread madaraReaderThread;
	
	private boolean scrollLocked;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.log_activity);
		
		logContainer = (LinearLayout)findViewById(R.id.log_container);
		scroll = (ScrollView)findViewById(R.id.scrollView1);
		scrollLockCheckBox = (CheckBox)findViewById(R.id.scrollLockCheckBox);
		scrollLockCheckBox.setOnCheckedChangeListener( new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				scrollLocked=isChecked;
			}
		});
		
	};
	
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
				
				HashMap<String, Object> map = binder.getMadaraVariableMap();
				for(String key: map.keySet()){
					String log = key + "=" + map.get(key);
					final TextView textView = new TextView(MadaraLogActivity.this);
					textView.setText(log);
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							logContainer.addView(textView);
						}
					});
					if(!scrollLocked){
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								scroll.fullScroll(View.FOCUS_DOWN);
							}
						});
					}
				}
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
		if(madaraReaderThread!=null && madaraReaderThread.isRunning()){
			madaraReaderThread.stopRunning();
		}
		super.onPause();
	}
	
}
