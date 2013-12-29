package com.example.vittalatemple;

import java.io.IOException;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
	 
    private SurfaceHolder mSurfaceHolder;
    private Camera mCamera;
 
    //Constructor that obtains context and camera
    public CameraPreview(Context context, Camera camera) {
        super(context);
        this.mCamera = camera;
        Log.i("CameraPreview","constructor");       
        this.mSurfaceHolder = this.getHolder();
        this.mSurfaceHolder.addCallback(this); // we get notified when underlying surface is created and destroyed
        this.mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS); //this is a deprecated method, is not requierd after 3.0
    }
 
    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        try {
        	Log.i("CameraPreview","creating surface");
        	if( mCamera == null){
//        		mCamera = Camera.open();
        		Log.d("CameraPreview","fucking camera still null");
        	}
            mCamera.setPreviewDisplay(surfaceHolder);
            mCamera.startPreview();
            //mCamera.autoFocus(autoFocusCallback);
        } catch (IOException e) {
        }
 
    }
     
    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        	 if (mCamera != null ) {
                     mCamera.release();
                     mCamera = null;
                     Log.i("CAMERA","Camera Release");
             }
        
    }
 
    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format,
            int width, int height) {
        // start preview with new settings
        try {
            mCamera.setPreviewDisplay(surfaceHolder);
            mCamera.startPreview();
            mCamera.autoFocus(autoFocusCallback);
        } catch (Exception e) {
            
        }
    }
    
    AutoFocusCallback autoFocusCallback = new AutoFocusCallback() {
    	  @Override
    	  public void onAutoFocus(boolean success, Camera camera) {
    	   ;
    	  }
    	};
     
}