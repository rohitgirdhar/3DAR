package com.example.vittalatemple;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.PictureCallback;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.example.vittalatemple.util.GeoLocationManger;

public class QueryActivity extends Activity {

	private  Camera mCamera;
	private  CameraPreview mCameraPreview;
	private FrameLayout preview;
	private static int limit = 30; //The number of cycles for which user must hold the phone still
	private static int count=limit;
	private float oldvals[] = new float[3];
	private float gravity[] = new float[3];
	private float lin_acc[] = new float[3];
	private final float LOW_THRESH = 1.6f;
	private static boolean flag = false;
	public static File pictureFile;
	GeoLocationManger gl; 
	public static String fpath;
	public final static String ANNOTATED = "com.example.heritagecam.ISANNOTATED";
	public final static String BYTEARRAY = "com.example.heritagecam.BYTEARRAY";
	public final static String TEXTANN = "com.example.heritagecam.TEXTANN";
	public static boolean released;
	public static byte mArr[];
	private MenuItem English;
	private MenuItem Hindi;
	public static String res ;
	public static boolean GPSUSE;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_query);
		Log.d("Main","Oncreate called");
		
		GPSUSE = getIntent().getExtras().getBoolean("GPSUSE");
		
	
		if(checkCameraHardware()) 
			mCamera = getCameraInstance();
		if(mCamera==null){
			Log.i("ERROR","CAMERA IS NULL");
			return ; 
		}
		Camera.Parameters cp = mCamera.getParameters();
		//cp.setPictureSize(1280,960);
		mCamera.setParameters(cp);
		mCameraPreview = new CameraPreview(this, mCamera);

		preview = (FrameLayout) findViewById(R.id.camera_preview);
		preview.addView(mCameraPreview);
		count = limit;
		released = false;
		gl =  new GeoLocationManger(getApplicationContext());
		
	}



	/**
	 * Helper method to access the camera returns null if
	 * it cannot get the camera or does not exist
	 * @return
	 */
	private Camera getCameraInstance() {
		Camera camera = null;

		try {
			camera = Camera.open();
		} catch (Exception e) {
			Log.i("Main","Cannot Open Camera");
			finish();
		}
		return camera;
	}

	protected void onResume() {
		super.onResume();
		if( mCamera == null){
			Log.d("Main", "camera creating again on resume");
			mCamera = getCameraInstance();

			mCameraPreview = new CameraPreview(this, mCamera);
			
			preview = (FrameLayout) findViewById(R.id.camera_preview);
			preview.addView(mCameraPreview);
		}
         count = 0;
         flag=false;
         Log.d("Main","Resume called");
         //mCamera.startPreview();
	}

	@Override
	protected void onPause() {
		super.onPause();
		Log.d("Main","onPause");
		if( mCamera != null ){
			 mCamera.stopPreview();
             mCamera.setPreviewCallback(null);
             mCamera.release();
     		 preview.removeView(mCameraPreview);
             mCamera = null;
             mCameraPreview=null;
		}
	}

	private boolean checkCameraHardware() {
		if (this.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)){
			// this device has a camera
			return true;
		} else {
			// no camera on this device
			return false;
		}
	}
	

	@Override
	public boolean onTouchEvent(MotionEvent event){

		mCamera.autoFocus(autoFocusCallback);
		return super.onTouchEvent(event);
	}

	AutoFocusCallback autoFocusCallback = new AutoFocusCallback() {
		@Override
		public void onAutoFocus(boolean success, Camera camera) {
			try{
				Thread.sleep(500);
			}
			catch(Exception e){;}
			mCamera.takePicture(null,null,mPicture); 
		}
	};

	PictureCallback mPicture = new PictureCallback() {

		@Override
		public void onPictureTaken(byte[] data, Camera camera) {
			count++;
			fpath = getOverwrittenFile(data);
			Log.d("Path for test",fpath);
					QueryActivity.res = "No Annotation Found";
//					res = "lol";
					QueryActivity.res = Search();
					//res = search(fpath) - native function with imgpath
					if(QueryActivity.res.equals("No Annotation Found"))
						goToNewActivity(data,false,res);
					else if(GPSUSE == true){
						double lat_hampi = 15.342272;
						double long_hampi = 76.475316;				
						Log.d("Latitude of the phone.",Double.toString(gl.getLatitude()));
						Log.d("Longitude of the phone.",Double.toString(gl.getLongitude()));
						if( !gl.canGetLocation() || haversine(lat_hampi, long_hampi, gl.getLatitude(), gl.getLongitude()) <= 0.1){
							goToNewActivity(data,true,res);
						}
						else{
							res = "No Annotation Found";
							goToNewActivity(data,false,res);
						}
					}else{
						goToNewActivity(data,true,res);
					}
		}
	};

	
	public  double haversine(
	        double lat1, double lng1, double lat2, double lng2) {
	    int r = 6371; // average radius of the earth in km
	    double dLat = Math.toRadians(lat2 - lat1);
	    double dLon = Math.toRadians(lng2 - lng1);
	    double a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
	       Math.cos(Math.toRadians(lat1)) * Math.cos(Math.toRadians(lat2)) 
	      * Math.sin(dLon / 2) * Math.sin(dLon / 2);
	    double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
	    double d = r * c;
	    return d;
	}
	
	private void goToNewActivity(byte[] data, boolean isAnnotated, String res){
		Intent intent = new Intent(QueryActivity.this, AbstractQueryDetails.class);
		Log.d("Main","Intent created");
//		intent.putExtra(BYTEARRAY, data);
		QueryActivity.mArr = data;
		intent.putExtra(ANNOTATED, isAnnotated);
		//intent.putExtra(TEXTANN, res);
		Log.d("Main","Extra put");
		Log.d("Main", "Res = " + res);
		Log.d("Main","LOL");
		startActivity(intent);
//		released = true;

	}

	public native String Search();

	static {
		System.loadLibrary("opencv_java");
		System.loadLibrary("native_sample");
	}


	private String getOverwrittenFile(byte[] data){

		//Obtains the file path where the taken image will be saved
		File testimgfile = new File("/sdcard/TestImage.jpg");

		// Create a media file name
		File mediaFile = new File(testimgfile.getAbsolutePath());
		try {
			FileOutputStream fos = new FileOutputStream(mediaFile);
			fos.write(data);
			fos.close();
			Log.d("Main","File overwritten");
		}catch (FileNotFoundException e) {
			finish();
		}catch (IOException e) {
			finish();
		}

		return testimgfile.getAbsolutePath();
	}


	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		English = menu.add("English");
		Hindi = menu.add("Hindi");
		return true;
	}

	public boolean onOptionsItemSelected(MenuItem item) {

		if(item==English){
			LaunchActivity.lang_id = 1;
		}
		else if (item==Hindi){
			LaunchActivity.lang_id = 0;
		}

		return true;
	}


}
