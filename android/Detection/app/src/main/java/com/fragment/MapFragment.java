package com.fragment;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.BitmapFactory;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.bean.Record;
import com.detection.R;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.utils.DbSqliteHelper;

import java.util.List;

/**
 *
 */
public class MapFragment extends Fragment implements OnMapReadyCallback{

    private GoogleMap mMap;
    private SupportMapFragment mSupportMapFragment;
    private DbSqliteHelper dbSqliteHelper;
    private List<Record> list;
    PopupWindow popupWindow;

    public MapFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        mSupportMapFragment = (SupportMapFragment) getChildFragmentManager().findFragmentById(R.id.map);
        if (mSupportMapFragment == null) {
            FragmentManager fragmentManager = getChildFragmentManager();
            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            mSupportMapFragment = SupportMapFragment.newInstance();
            fragmentTransaction.replace(R.id.map, mSupportMapFragment).commit();
        }
        if (mSupportMapFragment != null) {
            mSupportMapFragment.getMapAsync(new OnMapReadyCallback() {
                @Override
                public void onMapReady(GoogleMap googleMap) {
                    if (googleMap != null) {
                        googleMap.getUiSettings().setAllGesturesEnabled(true);
                        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                            return;
                        }
                        googleMap.setMyLocationEnabled(true);
                        LatLng sydney = new LatLng(-34, 151);
                        googleMap.addMarker(new MarkerOptions().position(sydney).title("Marker in Sydney"));
                        googleMap.moveCamera(CameraUpdateFactory.newLatLng(sydney));
                    }
                }
            });
        }
        dbSqliteHelper = new DbSqliteHelper(getActivity());
        View view = inflater.inflate(R.layout.fragment_map, container, false);
        return view;
    }

    private void initData(){
        list=dbSqliteHelper.getAllRecords();
    }

    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        initData();
        // Add a marker in Sydney and move the camera
        int i = 0;
        for(Record r:list) {
            String latlng = r.getPlace();
            String lls[]=latlng.split(" ");
            LatLng one = new LatLng(Double.parseDouble(lls[0]), Double.parseDouble(lls[1]));
            BitmapDescriptor oneimg = BitmapDescriptorFactory.fromBitmap(BitmapFactory.decodeResource(getResources(), R.mipmap.marker));
            mMap.addMarker(new MarkerOptions().position(one).title(r.getPm()).snippet("" + i).icon(oneimg).draggable(false));
            ++i;
        }

        mMap.setOnInfoWindowClickListener(new GoogleMap.OnInfoWindowClickListener() {
            @Override
            public void onInfoWindowClick(@NonNull Marker marker) {
                Record record = list.get(Integer.parseInt(marker.getSnippet()));
                View contentView = LayoutInflater.from(getActivity()).inflate(R.layout.view_map_infowindow, null);
                popupWindow = new PopupWindow(contentView, LinearLayout.LayoutParams.WRAP_CONTENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT, true);
                popupWindow.setContentView(contentView);
                TextView tvEquip = contentView.findViewById(R.id.tv_equipment);
                tvEquip.setText("Equipment : "+record.getEquipment());
                TextView tvPm = contentView.findViewById(R.id.tv_pm);
                tvPm.setText("PM 2.5 : "+record.getPm());
                TextView tvTime = contentView.findViewById(R.id.tv_date);
                tvTime.setText("Time : "+record.getTime());
                popupWindow.showAtLocation(getView(), Gravity.CENTER, 0, 0);
            }
        });

        mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {
            @Override
            public void onMapClick(@NonNull LatLng latLng) {
                if(popupWindow.isShowing()){
                    popupWindow.dismiss();
                }
            }
        });
    }
}