package com.example.vittalatemple;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;

public class UploadMenuActivity extends Activity{
	byte[] imgdata;
	Bitmap bmp;
	RelativeLayout rl ;
	private Resources resources;
	View annotationText;
	View saveButton;



	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.d("UploadActivity","Starting upload menu-activity");
		super.onCreate(savedInstanceState);
		Intent intent = getIntent();
		imgdata = UploadActivity.mArr;
		
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		setContentView(R.layout.activity_uploadmenu);
		rl = (RelativeLayout) findViewById (R.id.relative_layout_menu);
		resources = getResources();
		
		bmp = BitmapFactory.decodeByteArray(imgdata , 0, imgdata .length);
		BitmapDrawable bdr = new BitmapDrawable(bmp);
		rl.setBackgroundDrawable(bdr);
		resources = getResources();
		
		annotationText = (EditText)findViewById(R.id.annotationText);
		saveButton = (Button)findViewById(R.id.saveButton);

       	saveButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				

			}});          
	}
	

}
