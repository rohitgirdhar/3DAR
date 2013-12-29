package com.example.vittalatemple.tabs;

import java.io.File;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.MediaController;
import android.widget.Toast;
import android.widget.VideoView;

import com.example.vittalatemple.AbstractQueryDetails;
import com.example.vittalatemple.R;

public class Video extends Fragment {
	View rootView_;
	private VideoView mVideoView;

	String path_="";
	
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		rootView_ = inflater.inflate(R.layout.activity_video, container, false);
		AbstractQueryDetails ad = (AbstractQueryDetails) getActivity();
		path_ = ad.getUserDefinedArgunments().get("videopath");
		
		File f1 =  new File(path_);				
		
		if( !f1.exists() ){
			return rootView_;
		}
		
	    mVideoView = (VideoView) rootView_.findViewById(R.id.view_video);
	    mVideoView.setVideoPath(path_);
	    mVideoView.setMediaController(new MediaController(getActivity()));
	    mVideoView.requestFocus();
	    return rootView_;
	}
	
	
	
}
