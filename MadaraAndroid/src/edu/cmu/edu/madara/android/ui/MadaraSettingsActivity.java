package edu.cmu.edu.madara.android.ui;

import edu.cmu.edu.madara.android.MadaraService;
import edu.cmu.edu.madara.android.R;
import android.content.Intent;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.widget.Toast;

public class MadaraSettingsActivity extends PreferenceActivity {
	
	private PrefsFragment prefsFragment;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		prefsFragment = new PrefsFragment();

		getFragmentManager().beginTransaction().replace(android.R.id.content,
				prefsFragment).commit();
	}
	
	@Override
	public void onBackPressed() {
		//If the preferences have changed then restart the Madara Service
		if( prefsFragment.preferenceChanged() ){
			stopService(new Intent(this, MadaraService.class));
			startService(new Intent(this, MadaraService.class));
		}
		super.onBackPressed();
	}


	public static class PrefsFragment extends PreferenceFragment implements OnPreferenceChangeListener{

		private EditTextPreference deviceIdPreference;
		private EditTextPreference ipAddressPreference;
		private EditTextPreference portNumberPreference;
		private ListPreference transportTypePreference;
		private boolean preferenceChanged;

		@Override
		public void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			addPreferencesFromResource(R.xml.preferences);

			deviceIdPreference = (EditTextPreference) getPreferenceScreen().findPreference("device_id");
			deviceIdPreference.setOnPreferenceChangeListener(this);
			deviceIdPreference.setSummary(deviceIdPreference.getText());
			ipAddressPreference = (EditTextPreference) getPreferenceScreen().findPreference("ip_address");
			ipAddressPreference.setOnPreferenceChangeListener(this);
			ipAddressPreference.setSummary(ipAddressPreference.getText());
			portNumberPreference = (EditTextPreference) getPreferenceScreen().findPreference("port_number");
			portNumberPreference.setOnPreferenceChangeListener(this);
			portNumberPreference.setSummary(portNumberPreference.getText());
			transportTypePreference = (ListPreference) getPreferenceScreen().findPreference("transport_type");
			transportTypePreference.setOnPreferenceChangeListener(this);
			transportTypePreference.setSummary(transportTypePreference.getEntry());

		}

		@Override
		public boolean onPreferenceChange(Preference preference, Object newValue) {
			if(preference.equals(transportTypePreference)){
				for(int i=0; i < transportTypePreference.getEntryValues().length; i++){
					if(transportTypePreference.getEntryValues()[i].equals(newValue)){
						transportTypePreference.setSummary(transportTypePreference.getEntries()[i]);
						break;
					}
				}
			}
			else
				preference.setSummary(newValue.toString());
			
			preferenceChanged = true;

			return true;
		}
		
		public boolean preferenceChanged(){
			return preferenceChanged;
		}
	}

}
