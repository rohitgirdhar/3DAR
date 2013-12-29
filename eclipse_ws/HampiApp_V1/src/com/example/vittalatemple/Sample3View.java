package com.example.vittalatemple;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;



import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.widget.SlidingDrawer;

class Sample3View extends SampleViewBase {
	
	private int mFrameSize;
	private Bitmap mBitmap;
	private int[] mRGBA;
	public static byte mArr[];
	Context parentActivity;
	static public String res;
	static public Bitmap bmp;
	public static int AugFlag;
	
	ArrayList<SearchOverInterface> listeners = new ArrayList<SearchOverInterface>();


    public Sample3View(Context context) {
        super(context);
    	parentActivity = context;
    	res = "!";
    	AugFlag = 0;
    }

	@Override
	protected void onPreviewStarted(int previewWidtd, int previewHeight) {
		if(mBitmap != null) {
			mBitmap.recycle();
			mBitmap = null;
		}
		mFrameSize = previewWidtd * previewHeight;
		mRGBA = new int[mFrameSize];
		mBitmap = Bitmap.createBitmap(previewWidtd, previewHeight, Bitmap.Config.ARGB_8888);
	}

	@Override
	protected void onPreviewStopped() {
		if(mBitmap != null) {
//			mBitmap.recycle();
//			mBitmap = null;
		}
		mRGBA = null;
	
		
		
	}

    @Override
    protected Bitmap processFrame(byte[] data) {
        int[] rgba = mRGBA;
        bmp = mBitmap; 

        
        if( mBooleanIsPressed == true ){
        	res = Search(getFrameWidth(), getFrameHeight(), data,rgba);
        	Log.e("processFrame-search called",res);
        	
    		Sample3View.mArr = data;
            bmp.setPixels(rgba, 0/* offset */, getFrameWidth() /* stride */, 0, 0, getFrameWidth(), getFrameHeight());
//        	getOverwrittenFile(data);            
    		for( SearchOverInterface listener:listeners){
    			listener.onSearchingDone(true);
    		}
    		activityStopped = true;
    	    
        }else{
        	
            FindFeatures(getFrameWidth(), getFrameHeight(), data, rgba);
            bmp.setPixels(rgba, 0/* offset */, getFrameWidth() /* stride */, 0, 0, getFrameWidth(), getFrameHeight());
        }
  

        return bmp;
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
		}catch (IOException e) {
		}

		return testimgfile.getAbsolutePath();
	}

    
	public void setSearchOverInterface( SearchOverInterface listener){
		listeners.add(listener);
	}
    
    public native String Search(int width, int height, byte yuv[], int[] rgba);
    public native void FindFeatures(int width, int height, byte yuv[], int[] rgba);
    public static native int CheckAugmented();

    
    static {
        System.loadLibrary("native_sample");
    }


	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		activityStopped = true;
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		activityStopped = false;

	}
}
