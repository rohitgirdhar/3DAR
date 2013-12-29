package com.example.vittalatemple.tabs;

import android.content.res.Resources;
import android.content.*;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.example.vittalatemple.AbstractQueryDetails;
import com.example.vittalatemple.R;


public class AR extends Fragment {

	public String is_augmented = "";	

	View rootView_;

	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		rootView_ = inflater.inflate(com.example.vittalatemple.R.layout.activity_ar, container, false);

		AbstractQueryDetails ad = (AbstractQueryDetails) getActivity();

		is_augmented = ad.getUserDefinedArgunments().get("isAugmented");
		ImageView at = (ImageView) rootView_.findViewById(R.id.ar_img);
		Resources resources = ad.getResources();
		if(is_augmented.equals("true")){
			
			BitmapDrawable bdr = new BitmapDrawable(resources, "/sdcard/aug.jpg");
			//bdr.setGravity(40);
			
			at.setBackgroundDrawable(bdr);
			///
		}
		return rootView_;
	}
}
