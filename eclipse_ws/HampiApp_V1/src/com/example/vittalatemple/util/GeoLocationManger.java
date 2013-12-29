package com.example.vittalatemple.util;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

public class GeoLocationManger extends Service implements LocationListener
{
    private final Context context_;

    //flag for GPS Status
    boolean isGPSEnabled_ = false;

    //flag for network status
    boolean isNetworkEnabled_ = false;

    boolean canGetLocation_ = false;

    Location location_;
    double current_latitude_;
    double current_longitude_;

    //The minimum distance to change updates in meters
    private static final long MIN_DISTANCE_CHANGE_FOR_UPDATES = 10; //10 meters

    //The minimum time between updates in milliseconds
    private static final long MIN_TIME_BW_UPDATES = 1000 * 60 * 1; // 1 minute

    protected LocationManager locationManager_;

    public GeoLocationManger(Context context) 
    {
        this.context_ = context;
        getLocation();
    }

    public Location getLocation()
    {
        try
        {
            locationManager_ = (LocationManager) context_.getSystemService(LOCATION_SERVICE);

            //getting GPS status
            isGPSEnabled_ = locationManager_.isProviderEnabled(LocationManager.GPS_PROVIDER);

            //getting network status
            isNetworkEnabled_ = locationManager_.isProviderEnabled(LocationManager.NETWORK_PROVIDER);

            if (!isGPSEnabled_ && !isNetworkEnabled_)
            {
                Log.e("GeloLocation", "GPS or Network is not enabled. Can not get location");
                this.canGetLocation_ = false;
            }
            else
            {
                this.canGetLocation_ = true;

                //First get location from Network Provider
                if (isNetworkEnabled_)
                {
                    locationManager_.requestLocationUpdates(
                            LocationManager.NETWORK_PROVIDER,
                            MIN_TIME_BW_UPDATES,
                            MIN_DISTANCE_CHANGE_FOR_UPDATES, this);

                    Log.d("Network", "Network");

                    if (locationManager_ != null)
                    {
                        location_ = locationManager_.getLastKnownLocation(LocationManager.NETWORK_PROVIDER);
                        updateGPSCoordinates();
                    }
                }

                //if GPS Enabled get lat/long using GPS Services
                if (isGPSEnabled_)
                {
                    if (location_ == null)
                    {
                        locationManager_.requestLocationUpdates(
                                LocationManager.GPS_PROVIDER,
                                MIN_TIME_BW_UPDATES,
                                MIN_DISTANCE_CHANGE_FOR_UPDATES, this);

                        Log.d("GPS Enabled", "GPS Enabled");

                        if (locationManager_ != null)
                        {
                            location_ = locationManager_.getLastKnownLocation(LocationManager.GPS_PROVIDER);
                            updateGPSCoordinates();
                        }
                    }
                }
            }
        }
        catch (Exception e)
        {
            Log.e("Error : Location", "Impossible to connect to LocationManager", e);
        }

        return location_;
    }

    public void updateGPSCoordinates()
    {
        if (location_ != null)
        {
            current_latitude_ = location_.getLatitude();
            current_longitude_ = location_.getLongitude();
        }
    }

    /**
     * Stop using GPS listener
     * Calling this function will stop using GPS in your app
     */

    public void stopUsingGPS()
    {
        if (locationManager_ != null)
        {
            locationManager_.removeUpdates(GeoLocationManger.this);
        }
    }

    /**
     * Function to get latitude
     */
    public double getLatitude()
    {
        if (location_ != null)
        {
            current_latitude_ = location_.getLatitude();
        }

        return current_latitude_;
    }

    /**
     * Function to get longitude
     */
    public double getLongitude()
    {
        if (location_ != null)
        {
            current_longitude_ = location_.getLongitude();
        }

        return current_longitude_;
    }

    /**
     * Function to check GPS/wifi enabled
     */
    public boolean canGetLocation()
    {
        return this.canGetLocation_;
    }

    @Override
    public void onLocationChanged(Location location) 
    {   
    }

    @Override
    public void onProviderDisabled(String provider) 
    {   
    }

    @Override
    public void onProviderEnabled(String provider) 
    {   
    }

    @Override
    public void onStatusChanged(String provider, int status, Bundle extras) 
    {   
    }

    @Override
    public IBinder onBind(Intent intent) 
    {
        return null;
    }
}