package com.example.vittalatemple.tabs;

import java.io.IOException;
import java.util.Locale;

import android.media.MediaPlayer;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

import com.example.vittalatemple.AbstractQueryDetails;
import com.example.vittalatemple.R;

public class Audio extends Fragment  implements OnInitListener{

	
	public static enum MODE{
		TextTpSpeech,
		AudioFile
	}
	
	View rootView_;
	private TextToSpeech tts;
	private MediaPlayer player_;
	Button play_btn_;
	public String annoataion_txt_ = "No annotation found";
	public String path_ = "";
	public MODE md_ = MODE.TextTpSpeech;
	public boolean isTTSOK = false;
	

	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		rootView_ = inflater.inflate(R.layout.activity_audio, container, false);
		AbstractQueryDetails ad = (AbstractQueryDetails) getActivity();
		
		if( ad.getUserDefinedArgunments().get("mode").equals("1") ){
			md_ = MODE.TextTpSpeech;
		}
		else if( ad.getUserDefinedArgunments().get("mode").equals("2") ){
			md_ = MODE.AudioFile;
		}
		String urlOrText = ad.getUserDefinedArgunments().get("audiopath");
		switch(md_){
			case AudioFile:
				path_ = urlOrText;
				break;
			case TextTpSpeech:			
			default:
				annoataion_txt_ = urlOrText;
				break;
		}		

		tts = new TextToSpeech(rootView_.getContext(),this);

		play_btn_ = (Button) rootView_.findViewById(R.id.playAudio);
		// button on click event
		play_btn_.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View arg0) {
				if( !isTTSOK ){
					Toast.makeText(rootView_.getContext(), "Please install Text to Speech", Toast.LENGTH_LONG).show();
					return;
				}
				
				
				if(tts.isSpeaking()){
					tts.stop();
				}
				else{				
					speakOut();
				}
			}

		});

		return rootView_;
	}

	public void speakOut(){
		Log.i("Audio Player ","Speak Out");
		switch(md_){
		case AudioFile:
			MediaPlayer mediaPlayer = new MediaPlayer();
			try {
				mediaPlayer.setDataSource(path_);
				mediaPlayer.prepare();
				mediaPlayer.start();
			} catch (IllegalStateException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			break;
		case TextTpSpeech:
			tts.speak(annoataion_txt_, TextToSpeech.QUEUE_ADD, null);
		default:
			break;
		}
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if(player_ != null) {
			player_.release();
		}
		if( tts != null) {
			tts.shutdown();
		}
	}
	
	  public void onPause()
	  {
	    super.onPause();
	        if(player_ != null) {
	          player_.stop();
	        }
	    if( tts != null)
	      tts.stop();
	  }
	

	@Override
	public void onInit(int status) {
		if (status == TextToSpeech.SUCCESS) {

			int result = tts.setLanguage(Locale.US);

			if (result == TextToSpeech.LANG_MISSING_DATA
					|| result == TextToSpeech.LANG_NOT_SUPPORTED) {
				Log.e("TTS", "This Language is not supported");
				isTTSOK = false;

			} else {
				play_btn_.setEnabled(true);
				isTTSOK = true;
			}

		} else {
			isTTSOK = false;
			Log.e("TTS", "Initilization Failed!");
		}
	}

}