package com.example.vittalatemple.tabs;

import com.example.vittalatemple.AbstractQueryDetails;
import com.example.vittalatemple.R;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;


public class Annotation extends Fragment {
	
	public String annotation_ = "";	

	View rootView_;
	
	 public View onCreateView(LayoutInflater inflater, ViewGroup container,
             Bundle savedInstanceState) {
         rootView_ = inflater.inflate(com.example.vittalatemple.R.layout.activity_annotation, container, false);
         
         AbstractQueryDetails ad = (AbstractQueryDetails) getActivity();
         annotation_ = ad.getUserDefinedArgunments().get("annotation");
         TextView at = (TextView) rootView_.findViewById(R.id.annotation_txt);
         at.setText(annotation_);
         //at.settextcol
         return rootView_;
     }
}
