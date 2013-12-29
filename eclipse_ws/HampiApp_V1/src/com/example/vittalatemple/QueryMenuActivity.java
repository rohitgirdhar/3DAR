package com.example.vittalatemple;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.drawable.BitmapDrawable;
import android.media.AudioManager;
import android.media.AudioManager.OnAudioFocusChangeListener;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class QueryMenuActivity extends Activity {
	
	public final static String IMGPATH2 = "com.example.heritagecam.IMGPATH2";
	public final static String VALUE = "com.example.heritagecam.VALUE";
	private MenuItem English;
    private MenuItem Hindi;
	RelativeLayout rl ;
	private MediaPlayer mplayer;
	private Resources resources;
	Button tv1;
	byte[] imgdata;
	boolean isAnnotated;
	String res;
	Bitmap bmp;
	Boolean[] feature = new Boolean[5];
	public static File pictureFile;
	String str;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		Log.d("MenuActivity","Starting menu-activity");
		super.onCreate(savedInstanceState);
		Intent intent = getIntent();
//		imgdata = intent.getByteArrayExtra(MainActivity.BYTEARRAY);
		imgdata = QueryActivity.mArr;
		isAnnotated = intent.getBooleanExtra(QueryActivity.ANNOTATED, false);
		res = QueryActivity.res ;
		
		
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		setContentView(R.layout.activity_querymenu);
		rl = (RelativeLayout) findViewById (R.id.relative_layout_menu);
		resources = getResources();
		
		bmp = BitmapFactory.decodeByteArray(imgdata , 0, imgdata .length);
		BitmapDrawable bdr = new BitmapDrawable(bmp);
		rl.setBackgroundDrawable(bdr);
		
		resources = getResources();
		fillFeature();
		for(int i=0 ;i < feature.length ; i++){
			if(feature[i]==false){
				oFFButton(i);
			}

		//Setting up the text view
		tv1 = (Button) findViewById(R.id.AnnotationPart);
       	tv1.getBackground().setAlpha(380);
       

//		tv1.getBackground().setAlpha(380);
		str = res;
		String line="";
		BufferedReader br;
		//Setting the language
 /*       switch(LaunchActivity.lang_id){
        case 0:
        	br = new BufferedReader(new InputStreamReader(resources.openRawResource(R.raw.hinditext)));
        	break ;
		case 1:
        	br = new BufferedReader(new InputStreamReader(resources.openRawResource(R.raw.engtext)));
			break ;
		default:
			br = new BufferedReader(new InputStreamReader(resources.openRawResource(R.raw.engtext)));
        }
        try{
        	while((line=br.readLine())!=null)
        		str+=""+line;
        }
        catch(Exception e){;} */
		}
		
		//Sets up the buttons
		final Button result = (Button) findViewById(R.id.result);
		if(isAnnotated==false){
			result.setBackgroundResource(R.drawable.red);
			tv1.setText("No Annotation Found");
		}
		else{
			result.setBackgroundResource(R.drawable.gree);
			tv1.setText(str);
		}
		
		final Button bt_tag = (Button) findViewById(R.id.tag);
        bt_tag.setBackgroundResource(R.drawable.text_icon2);
        bt_tag.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	tv1.setText(str);
                // Perform action on click
            }
        });
        
        final Button bt_htag = (Button) findViewById(R.id.htag);
        bt_htag.setBackgroundResource(R.drawable.hlink_icon2);
        bt_htag.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	tv1.setText("This is the text for htag");
                // Perform action on click
            }
        });
        

        final Button bt_audio = (Button) findViewById(R.id.audio);
        bt_audio.setBackgroundResource(R.drawable.audio2);
        bt_audio.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	mplayer=MediaPlayer.create(QueryMenuActivity.this,R.raw.hundred);
    			Log.d("ShowTag","Media player will play");
    			if(mplayer!=null)mplayer.start();
                // Perform action on click
            }
        });
        
        final Button bt_back = (Button) findViewById(R.id.back);
        bt_back.setBackgroundResource(R.drawable.backicon2);
        bt_back.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	if(mplayer!=null)mplayer.release();
            	bmp.recycle();
            	finish();
                // Perform action on click
            }
        });
        
        final Button bt_save = (Button) findViewById(R.id.save);
        bt_save.setBackgroundResource(R.drawable.save2);
        bt_save.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	savePicture();//This saves the actual pic with tag
                // Perform action on click
            }
        });
	}
	
	OnAudioFocusChangeListener afChangeListener = new OnAudioFocusChangeListener() {
		public void onAudioFocusChange(int focusChange) {
			if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK) {
				mplayer.setVolume(1.0f,1.0f);
				// Lower the volume
			} else if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
				mplayer.setVolume(1.0f,1.0f);
			}
		}
	};

	private void fillFeature() {
		// TODO Auto-generated method stub
		for(int i = 0 ; i < feature.length ; i++){
			feature[i]=true;
		}
		feature[3]=false ;
	}

	private void savePicture(){
		
		pictureFile = getOutputMediaFile();
        if (pictureFile == null){
            return;
        }
        Bitmap alteredbmp = Bitmap.createBitmap(bmp.getWidth(), bmp.getHeight(), bmp.getConfig());
        Canvas canvas = new Canvas(alteredbmp);
        Paint paint = new Paint();
        canvas.drawBitmap(bmp, 0, 0, paint);
        paint.setColor(Color.BLACK); 
        paint.setTextSize(80); 
        canvas.drawText(str, 10, 25, paint);
        
        try {
            FileOutputStream fos = new FileOutputStream(pictureFile);
            alteredbmp.compress(Bitmap.CompressFormat.JPEG, 100, fos);
            fos.close();
            Log.d("Menu","Image saved with text");
        } catch (FileNotFoundException e) {
        	finish();
        } catch (IOException e) {
        	finish();
        }
	}
	
	private void oFFButton(int i) {
		Button button;
		
		switch(i){
		case 0:
			button = (Button) findViewById(R.id.tag);
			button.setClickable(false);
			break ;
		case 1:
			button = (Button) findViewById(R.id.htag);
			button.setClickable(false);
			break ;
		case 2:
			button = (Button) findViewById(R.id.audio);
			button.setClickable(false);
			break ;
		case 3:
			button = (Button) findViewById(R.id.ar);
			button.setClickable(false);
			break;
		case 4:
			button = (Button) findViewById(R.id.back);
			button.setClickable(false);
			break;
		}

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
	
	private static File getOutputMediaFile(){

		//Obtains the file path where the taken image will be saved
		File mediaStorageDir = new File(Environment.getExternalStoragePublicDirectory(
				Environment.DIRECTORY_PICTURES), "HeritageCam");

		if (! mediaStorageDir.exists()){
			if (! mediaStorageDir.mkdirs()){
				Log.e("MyCameraApp", "failed to create directory");
				return null;
			}
		}

		// Create a media file name
		String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		File mediaFile;
		String fpath = mediaStorageDir.getPath() + File.separator +
				"IMG_"+ timeStamp +".jpg";
		mediaFile = new File(fpath);

		return mediaFile;
	}

}