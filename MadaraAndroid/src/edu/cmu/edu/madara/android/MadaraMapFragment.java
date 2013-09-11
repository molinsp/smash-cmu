package edu.cmu.edu.madara.android;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.google.android.gms.maps.MapFragment;

public class MadaraMapFragment extends MapFragment {

	public static final String TAG = MadaraMapFragment.class.getSimpleName();

	private View mOriginalContentView;
	private TouchableWrapper mTouchView;
	private MadaraMapTouchListener onTouchListener;

	@Override
	public View onCreateView( LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState ) {

		mOriginalContentView = super.onCreateView(inflater, container, savedInstanceState);

		mTouchView = new TouchableWrapper(getActivity());
		mTouchView.addView(mOriginalContentView);

		return mTouchView;
	}

	private class TouchableWrapper extends FrameLayout {

		public TouchableWrapper(Context context) {
			super(context);
		}

		@Override
		public boolean dispatchTouchEvent(MotionEvent ev) {
			if( onTouchListener!=null ){
				onTouchListener.onTouch(ev);
				return true;
			}
			return super.dispatchTouchEvent(ev);
		}
	}

	public void addOnTouchListener(MadaraMapTouchListener listener){
		this.onTouchListener = listener;
	}

	public void removeOnTouchListener(){
		this.onTouchListener = null;
	}

}