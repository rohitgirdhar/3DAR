package com.example.vittalatemple;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;

import com.example.vittalatemple.util.SystemUiHider;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 * 
 * @see SystemUiHider
 */
public class LaunchActivity extends Activity {
	/**
	 * Whether or not the system UI should be auto-hidden after
	 * {@link #AUTO_HIDE_DELAY_MILLIS} milliseconds.
	 */
	private static final boolean AUTO_HIDE = false;

	/**
	 * If {@link #AUTO_HIDE} is set, the number of milliseconds to wait after
	 * user interaction before hiding the system UI.
	 */
	private static final int AUTO_HIDE_DELAY_MILLIS = 3000;

	/**
	 * If set, will toggle the system UI visibility upon interaction. Otherwise,
	 * will show the system UI visibility upon interaction.
	 */
	private static final boolean TOGGLE_ON_CLICK = true;

	/**
	 * The flags to pass to {@link SystemUiHider#getInstance}.
	 */
	private static final int HIDER_FLAGS = SystemUiHider.FLAG_HIDE_NAVIGATION;

	/**
	 * The instance of the {@link SystemUiHider} for this activity.
	 */
	private SystemUiHider mSystemUiHider;

	
	public static int lang_id = 1 ;//The id of the language (English - 1, Hindi - 0)
	public static View queryButton, feedbackButton;
	public static View p1, logo_img,frm;
	public static CheckBox gps_ch;
	public static boolean GPS_ENABLED = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_launch);

		final View controlsView = findViewById(R.id.fullscreen_content_controls);
		final View contentView = findViewById(R.id.fullscreen_content);

		// Set up an instance of SystemUiHider to control the system UI for
		// this activity.
		mSystemUiHider = SystemUiHider.getInstance(this, contentView,
				HIDER_FLAGS);
		mSystemUiHider.setup();
		mSystemUiHider
				.setOnVisibilityChangeListener(new SystemUiHider.OnVisibilityChangeListener() {
					// Cached values.
					int mControlsHeight;
					int mShortAnimTime;

					@Override
					@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
					public void onVisibilityChange(boolean visible) {
						if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
							// If the ViewPropertyAnimator API is available
							// (Honeycomb MR2 and later), use it to animate the
							// in-layout UI controls at the bottom of the
							// screen.
							if (mControlsHeight == 0) {
								mControlsHeight = controlsView.getHeight();
							}
							if (mShortAnimTime == 0) {
								mShortAnimTime = getResources().getInteger(
										android.R.integer.config_shortAnimTime);
							}
							controlsView
									.animate()
									.translationY(visible ? 0 : mControlsHeight)
									.setDuration(mShortAnimTime);
						} else {
							// If the ViewPropertyAnimator APIs aren't
							// available, simply show or hide the in-layout UI
							// controls.
							controlsView.setVisibility(visible ? View.VISIBLE
									: View.GONE);
						}

						if (visible && AUTO_HIDE) {
							// Schedule a hide().
							delayedHide(AUTO_HIDE_DELAY_MILLIS);
						}
					}
				});

		// Set up the user interaction to manually show or hide the system UI.
		contentView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				if (TOGGLE_ON_CLICK) {
//					mSystemUiHider.toggle();
				} else {
//					mSystemUiHider.show();
				}
			}
		});

		// Upon interacting with UI controls, delay any scheduled hide()
		// operations to prevent the jarring behavior of controls going away
		// while interacting with the UI.		
		
		if(savedInstanceState == null){
			
			gps_ch = (CheckBox) findViewById(R.id.gps_ck);
			
			gps_ch.setOnCheckedChangeListener(new OnCheckedChangeListener() {
				
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					// TODO Auto-generated method stub
					if(gps_ch.isChecked()){
						GPS_ENABLED = true;
					}
					else if(gps_ch.isChecked()){
						GPS_ENABLED = false;
					}
					
				}
			}); 
			
			logo_img = (ImageView)findViewById(R.id.logo);
			frm = (FrameLayout) findViewById(R.id.frm_bg);
	    	p1 = (ProgressBar)findViewById(R.id.LoadingImage);
//	       	t1 = (TextView)findViewById(R.id.LoadingText);
	       	queryButton = (Button)findViewById(R.id.queryButton);
	    		//Adding Listener to button
	   		queryButton.setOnClickListener(new View.OnClickListener() {
	    			@Override
	    		public void onClick(View v) {
	   				Intent i = new Intent(LaunchActivity.this,	Sample3Native.class);
	   				i.putExtra("GPSUSE",false);
	   				startActivityForResult(i,0);
	   			}});
	   		
	       	feedbackButton = (Button)findViewById(R.id.feedbackButton);
	       	feedbackButton.setOnClickListener(new View.OnClickListener() {
    			@Override
    			public void onClick(View v) {
    				Intent i = new Intent(LaunchActivity.this,	FeedbackActivity.class);
    				startActivityForResult(i,0);
   			}});     
	       	new LoadDataTask().execute();
	       	
       	       	
	        	
	    }
	}

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);

		// Trigger the initial hide() shortly after the activity has been
		// created, to briefly hint to the user that UI controls
		// are available.
		delayedHide(100);
	}

	/**
	 * Touch listener to use for in-layout UI controls to delay hiding the
	 * system UI. This is to prevent the jarring behavior of controls going away
	 * while interacting with activity UI.
	 */
	View.OnTouchListener mDelayHideTouchListener = new View.OnTouchListener() {
		@Override
		public boolean onTouch(View view, MotionEvent motionEvent) {
			if (AUTO_HIDE) {
				delayedHide(AUTO_HIDE_DELAY_MILLIS);
			}
			return false;
		}
	};

	Handler mHideHandler = new Handler();
	Runnable mHideRunnable = new Runnable() {
		@Override
		public void run() {
//			mSystemUiHider.hide();
		}
	};

	/**
	 * Schedules a call to hide() in [delay] milliseconds, canceling any
	 * previously scheduled calls.
	 */
	private void delayedHide(int delayMillis) {
		mHideHandler.removeCallbacks(mHideRunnable);
		mHideHandler.postDelayed(mHideRunnable, delayMillis);
	}
	
	public native void LoadData();
    
	
    static {
    	System.loadLibrary("opencv_java");
        System.loadLibrary("native_sample");
    }
   
    public class LoadDataTask extends AsyncTask<Void, Integer, Void> {
    	@Override
    	protected void onPreExecute(){

    		p1.setVisibility(View.VISIBLE);
    		logo_img.setVisibility(View.VISIBLE);
    		queryButton.setVisibility(View.INVISIBLE);
    		gps_ch.setVisibility(View.INVISIBLE);
    		feedbackButton.setVisibility(View.INVISIBLE);
    		
    	}
    	
    	@Override
       protected void onProgressUpdate(Integer... progress) {
    		LoadData();       
        }

    	@Override
        protected void onPostExecute(Void arg0) {

    		p1.setVisibility(View.INVISIBLE);
    		logo_img.setVisibility(View.INVISIBLE);
    		queryButton.setVisibility(View.VISIBLE);
    		feedbackButton.setVisibility(View.VISIBLE);
    		gps_ch.setVisibility(View.VISIBLE);
    		
    		
    		frm .setBackgroundColor(Color.argb(247, 245, 239,255));
    		

        }

    	@Override
    	protected Void doInBackground(Void... arg0) {
    		// TODO Auto-generated method stub
    		LoadData();
    		return null;
    	}    	
    	
    }
    
    @Override
    protected void onResume() {

    	gps_ch = (CheckBox) findViewById(R.id.gps_ck);
		
		gps_ch.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				// TODO Auto-generated method stub
				if(gps_ch.isChecked()){
					GPS_ENABLED = true;
				}
				else if(gps_ch.isChecked()){
					GPS_ENABLED = false;
				}
				
			}
		}); 
		
		logo_img = (ImageView)findViewById(R.id.logo);
		frm = (FrameLayout) findViewById(R.id.frm_bg);
    	p1 = (ProgressBar)findViewById(R.id.LoadingImage);
//       	t1 = (TextView)findViewById(R.id.LoadingText);
       	queryButton = (Button)findViewById(R.id.queryButton);
    		//Adding Listener to button
   		queryButton.setOnClickListener(new View.OnClickListener() {
    			@Override
    		public void onClick(View v) {
   				Intent i = new Intent(LaunchActivity.this,	Sample3Native.class);
   				i.putExtra("GPSUSE",false);
   				startActivityForResult(i,0);
   			}});
   		
       	feedbackButton = (Button)findViewById(R.id.feedbackButton);
       	feedbackButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent i = new Intent(LaunchActivity.this,	FeedbackActivity.class);
				startActivityForResult(i,0);
			}});     
    	
    	
    	
		p1.setVisibility(View.VISIBLE);
		logo_img.setVisibility(View.VISIBLE);
		queryButton.setVisibility(View.INVISIBLE);
		feedbackButton.setVisibility(View.INVISIBLE);
		gps_ch.setVisibility(View.INVISIBLE);

       	new LoadDataTask().execute();
		
    	super.onResume();
    }
    
}
