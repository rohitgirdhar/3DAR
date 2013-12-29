package com.example.vittalatemple;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Window;

public class Sample3Native extends Activity implements SearchOverInterface{
    private static final String TAG = "Sample::Activity";
    private Sample3View cameraView;

    public Sample3Native() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        cameraView = new Sample3View(this);
        cameraView.setSearchOverInterface(this);
        cameraView.activityStopped = false;
        setContentView(cameraView);
    }
    
	@Override
	public boolean onTouchEvent(MotionEvent event){
	    if(event.getAction() == MotionEvent.ACTION_DOWN) {
	        // Execute your Runnable after 5000 milliseconds = 5 seconds.
	        cameraView.mBooleanIsPressed = true;
	    }

	    if(event.getAction() == MotionEvent.ACTION_UP) {
	        if(cameraView.mBooleanIsPressed) {
	            cameraView.mBooleanIsPressed = false;

	        }
	    }
		return super.onTouchEvent(event);

	}
	
	@Override
	public void onSearchingDone(boolean success) {
		// TODO Auto-generated method stub
		Log.i("MainActivity","OnDataLoaded");
		if( success == true){
			Intent intent = new Intent(Sample3Native.this, AbstractQueryDetails.class);

			Log.i("MainActivity","Redirecting to new activity");
			startActivity(intent);
		}
	}
	
	@Override
	protected void onPause(){
		super.onPause();
		cameraView.onPause();
	}
	
	@Override
	protected void onResume(){
		super.onResume();
		cameraView.onResume();
	}
}
