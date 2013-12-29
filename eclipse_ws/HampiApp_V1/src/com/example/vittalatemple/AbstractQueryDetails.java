package com.example.vittalatemple;

import java.io.File;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.Vector;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.util.Pair;
import android.view.Menu;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TabHost;
import android.widget.TabHost.TabContentFactory;

import com.example.vittalatemple.tabs.AR;
import com.example.vittalatemple.tabs.Annotation;
import com.example.vittalatemple.tabs.Audio;
import com.example.vittalatemple.tabs.Video;
import com.example.vittalatemple.util.GeoLocationManger;
import com.example.vittalatemple.util.WAdapter;

/**
 * 
 */
public class AbstractQueryDetails extends FragmentActivity implements TabHost.OnTabChangeListener, ViewPager.OnPageChangeListener {

	public final static String IMGPATH2 = "com.wazzat.activity.IMGPATH2";
	public final static String VALUE = "com.wazzat.activity.VALUE";
	private Resources resources;
	byte[] imgdata;
	boolean isAnnotated;
	
	public static String annotaion_text="";
	public static int audio_mode = 1;
	public static String audio_path = "";
	public static String is_augmented = "";
	public static String video_path = "";
	public Bitmap bmp;
	String res;
	Boolean[] feature = new Boolean[5];
	public static File pictureFile;
	GeoLocationManger gl;

	private TabHost mTabHost;
	private ViewPager mViewPager;
	private HashMap<String, TabInfo> mapTabInfo = new HashMap<String, TabInfo>();
	public WAdapter mPagerAdapter;

	private Bundle bunder_;

	protected Vector<Pair<String, Class<? extends Fragment>>> fragmentList =  new Vector<Pair<String,Class<? extends Fragment>>>();

	/**
	 *
	 * @author mwho
	 * Maintains extrinsic info of a tab's construct
	 */
	private class TabInfo {
		private String tag;
		private Class<?> clss;
		private Bundle args;
		private Fragment fragment;
		TabInfo(String tag, Class<?> clazz, Bundle args) {
			this.tag = tag;
			this.clss = clazz;
			this.args = args;
		}

	}
	/**
	 * A simple factory that returns dummy views to the Tabhost
	 * @author mwho
	 */
	class TabFactory implements TabContentFactory {

		private final Context mContext;

		/**
		 * @param context
		 */
		public TabFactory(Context context) {
			mContext = context;
		}

		/** (non-Javadoc)
		 * @see android.widget.TabHost.TabContentFactory#createTabContent(java.lang.String)
		 */
		public View createTabContent(String tag) {
			View v = new View(mContext);
			v.setMinimumWidth(0);
			v.setMinimumHeight(0);
			return v;
		}

	}
	/** (non-Javadoc)
	 * @see android.support.v4.app.FragmentActivity#onCreate(android.os.Bundle)
	 */
	@SuppressLint("NewApi")
	@SuppressWarnings("deprecation")
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// Inflate the layout
		setContentView(R.layout.activity_abstract_query_details);

		Log.i("Query_Details", "in constructor");

		gl = new GeoLocationManger(getApplicationContext());

		init();

		fragmentList = getFragmentList();

		// Initialise the TabHost
		this.initialiseTabHost(savedInstanceState);
		if (savedInstanceState != null) {
			mTabHost.setCurrentTabByTag(savedInstanceState.getString("tab")); //set the tab as per the saved state
		}
		// Intialise ViewPager
		this.intialiseViewPager();
	}


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

	@SuppressLint("NewApi")
	public void init(){
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		imgdata = Sample3View.mArr;
		
		res = Sample3View.res ;		
		boolean isAnnotationCorrect = true;
		
		//if(! gl.canGetLocation() ){				
		//		Toast.makeText(getApplicationContext(), "Help us to make your search better. Switch on your GPS !", Toast.LENGTH_LONG).show();
		//}
		
				
		if(res.equals("No Annotation Found") ){			
			Log.d("Main","Structure is null");
			isAnnotationCorrect = false;
			//super.onBackPressed();
			//return;
		}
		else{
				if( LaunchActivity.GPS_ENABLED ){
					double lat_hampi = 15.342272;
					double long_hampi = 76.475316;				
					Log.d("Latitude of the phone.",Double.toString(gl.getLatitude()));
					Log.d("Longitude of the phone.",Double.toString(gl.getLongitude()));
					if(haversine(lat_hampi, long_hampi, gl.getLatitude(), gl.getLongitude()) > 0.1){
						res = "No Annotation Found";
						isAnnotationCorrect = false;
					}
				}						
		}
		/*if(!isAnnotationCorrect){			
			Toast.makeText(getApplicationContext(), "Sorry No Annotation Found! :(", Toast.LENGTH_LONG).show();
		}*/
		Log.d("Main","Structure is not null something else is null");

		annotaion_text =  res;		
		audio_mode = 1;
		audio_path = res;
		video_path= "/sdcard/v.mp4";
		if(Sample3View.CheckAugmented() == 1){
			is_augmented ="true";
		}
		else{
			is_augmented ="false";
		}
		 
		resources = getResources();
		
		Random rn = new Random();
		//String file_path = Environment.getExternalStorageDirectory().getAbsolutePath()+"/wazzat/Image"+rn.nextInt(Integer.MAX_VALUE)+".jpg";
		String file_path = "WazzatImage"+rn.nextInt(Integer.MAX_VALUE)+".jpg";
		String x= "false";
		if(isExternalStorageWritable()){
			x= "true";
		}
		Log.d("Main",x);

		/*		if( isExternalStorageWritable()){
			try {
				FileOutputStream fos = openFileOutput(file_path, Context.MODE_PRIVATE);
				fos.write(imgdata);
				fos.close();
				Log.d("Main","File written to path " +file_path);
			}catch (FileNotFoundException e) {
				Log.e("Creating file", "Received msg" + e.toString());
				finish();
			}catch (IOException e) {
				Log.e("Creating file", "Received msg" + e.toString());
				finish();
			}
		}
		 */
		//		bmp = BitmapFactory.decodeByteArray(imgdata , 0, imgdata.length);
		
		//		bmp = (Bitmap) intent.getParcelableExtra("BitmapImage");
		final Bitmap b_c = Sample3View.bmp;
		bmp =Sample3View.bmp;
		BitmapDrawable bdr = new BitmapDrawable(resources, b_c);
		bdr.setGravity(TRIM_MEMORY_BACKGROUND);
		LinearLayout ll = (LinearLayout) findViewById(R.id.query_details_layout);
		ll.setScaleX(1);
		ll.setScaleY(1);
//		ll.setBackgroundDrawable(bdr);
		ll.setBackground(bdr);
	}



	/* Checks if external storage is available for read and write */
	public boolean isExternalStorageWritable() {
		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			return true;
		}
		return false;
	}

	public HashMap<String,String>getUserDefinedArgunments(){
		HashMap<String,String> hm = new HashMap<String, String>();
		StringBuffer audio_m = new StringBuffer(audio_mode);
		hm.put("annotation", annotaion_text);
		hm.put("mode", audio_m .toString());
		hm.put("audiopath", audio_path);
		hm.put("videopath", video_path);
		hm.put("isAugmented", is_augmented);
		return hm;
	}


	public  Vector<Pair<String, Class<? extends Fragment>>> getFragmentList(){
		Log.i("AbstractQueryDetails", "getFragmentListCalled not cool");
		Vector<Pair<String, Class<? extends Fragment>>> frgList = new Vector<Pair<String,Class<? extends Fragment>>>();
		frgList.add(new Pair<String, Class<? extends Fragment>>("Annotation",Annotation.class));
		frgList.add(new Pair<String, Class<? extends Fragment>>("Audio",Audio.class));
		frgList.add(new Pair<String, Class<? extends Fragment>>("Video",Video.class));
		frgList.add(new Pair<String, Class<? extends Fragment>>("AR",AR.class));
		return frgList ;    	
	}

	/** (non-Javadoc)
	 * @see android.support.v4.app.FragmentActivity#onSaveInstanceState(android.os.Bundle)
	 */
	protected void onSaveInstanceState(Bundle outState) {
		outState.putString("tab", mTabHost.getCurrentTabTag()); //save the tab selected
		super.onSaveInstanceState(outState);
	}

	/**
	 * Initialise ViewPager
	 */
	private void intialiseViewPager() {

		List<Fragment> fragments = new Vector<Fragment>();
		for (int i = 0; i < fragmentList.size() ; i++ ){
			fragments.add(Fragment.instantiate(this, fragmentList.get(i).second.getName()));
		}
		mPagerAdapter  = new WAdapter(super.getSupportFragmentManager(), fragments);
		//
		mViewPager = (ViewPager)super.findViewById(R.id.viewpager);
		mViewPager.setAdapter(this.mPagerAdapter);
		mViewPager.setOnPageChangeListener(this);
	}

	/**
	 * Initialise the Tab Host
	 */
	private void initialiseTabHost(Bundle args) {
		mTabHost = (TabHost)findViewById(android.R.id.tabhost);
		mTabHost.setup();
		TabInfo tabInfo = null;

		for (int i = 0; i < fragmentList.size() ; i++ ){

			AbstractQueryDetails.AddTab(
					this, 
					this.mTabHost, 
					this.mTabHost.newTabSpec("Tab"+i).setIndicator(fragmentList.get(i).first), 
					( tabInfo = new TabInfo("Tab"+i, fragmentList.get(i).second, args)) 
					);



			this.mapTabInfo.put(tabInfo.tag, tabInfo);
		}
		/*        
        QueryDetails.AddTab(
        		this, 
        		this.mTabHost, 
        		this.mTabHost.newTabSpec("Tab1").setIndicator("Annotation"), 
        		( tabInfo = new TabInfo("Tab1", Annotation.class, args)) 
        		);
        this.mapTabInfo.put(tabInfo.tag, tabInfo);        
        QueryDetails.AddTab(
        		this, 
        		this.mTabHost, 
        		this.mTabHost.newTabSpec("Tab2").setIndicator("Audio"), 
        		( tabInfo = new TabInfo("Tab2", Audio.class, args))
        		);
        this.mapTabInfo.put(tabInfo.tag, tabInfo);
        QueryDetails.AddTab(
        		this, 
        		this.mTabHost, 
        		this.mTabHost.newTabSpec("Tab3").setIndicator("Video"), 
        		( tabInfo = new TabInfo("Tab3", Video.class, args))
        		);
        this.mapTabInfo.put(tabInfo.tag, tabInfo);*/

		// Default to first tab
		//this.onTabChanged("Tab1");
		//
		mTabHost.setOnTabChangedListener(this);
	}

	/**
	 * Add Tab content to the Tabhost
	 * @param activity
	 * @param tabHost
	 * @param tabSpec
	 * @param clss
	 * @param args
	 */
	private static void AddTab(AbstractQueryDetails activity, TabHost tabHost, TabHost.TabSpec tabSpec, TabInfo tabInfo) {
		// Attach a Tab view factory to the spec
		tabSpec.setContent(activity.new TabFactory(activity));
		tabHost.addTab(tabSpec);
	}

	/** (non-Javadoc)
	 * @see android.widget.TabHost.OnTabChangeListener#onTabChanged(java.lang.String)
	 */
	public void onTabChanged(String tag) {
		//TabInfo newTab = this.mapTabInfo.get(tag);
		int pos = this.mTabHost.getCurrentTab();
		mViewPager.setCurrentItem(pos);
	}

	/* (non-Javadoc)
	 * @see android.support.v4.view.ViewPager.OnPageChangeListener#onPageScrolled(int, float, int)
	 */
	@Override
	public void onPageScrolled(int position, float positionOffset,
			int positionOffsetPixels) {
		// TODO Auto-generated method stub

	}

	/* (non-Javadoc)
	 * @see android.support.v4.view.ViewPager.OnPageChangeListener#onPageSelected(int)
	 */
	@Override
	public void onPageSelected(int position) {
		// TODO Auto-generated method stub
		mTabHost.setCurrentTab(position);
	}

	/* (non-Javadoc)
	 * @see android.support.v4.view.ViewPager.OnPageChangeListener#onPageScrollStateChanged(int)
	 */
	@Override
	public void onPageScrollStateChanged(int state) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		return true;
	}
}