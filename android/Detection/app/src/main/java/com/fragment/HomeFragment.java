package com.fragment;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Toast;

import com.detection.R;
import com.utils.DbSqliteHelper;

import java.util.Locale;

/**
 *
 */
public class HomeFragment extends Fragment {

    private EditText et_equipment;
    private EditText et_pm;
    private EditText et_place;
    private EditText et_date;
    private DbSqliteHelper dbSqliteHelper;

    public HomeFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_home, container, false);
        dbSqliteHelper = new DbSqliteHelper(getActivity());
        et_equipment=view.findViewById(R.id.et_equipment);
        et_pm=view.findViewById(R.id.et_pm);
        et_place=view.findViewById(R.id.et_place);
        et_date=view.findViewById(R.id.et_date);
        view.findViewById(R.id.submitBtn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toSubmit();
            }
        });
        return view;
    }
    
    private void toSubmit(){
        String equipment = et_equipment.getText().toString();
        String pm = et_pm.getText().toString();
        String place = et_place.getText().toString();
        String date = et_date.getText().toString();
        if(equipment.equals("")||pm.equals("")||place.equals("")||date.equals("")){
            Toast.makeText(getActivity(), "All information must be filled in!", Toast.LENGTH_SHORT).show();
        }else{
            boolean flag=dbSqliteHelper.addRecord(equipment,pm,place,date);
            if(flag){
                Toast.makeText(getActivity(), "Submit Success!", Toast.LENGTH_SHORT).show();
            }else{
                Toast.makeText(getActivity(), "Submit Fail!", Toast.LENGTH_SHORT).show();
            }
        }
    }
}