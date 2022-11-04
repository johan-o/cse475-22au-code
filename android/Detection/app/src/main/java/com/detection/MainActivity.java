package com.detection;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import androidx.fragment.app.Fragment;
import androidx.viewpager.widget.ViewPager;
import com.google.android.material.tabs.TabLayout;
import com.adapter.ViewPagerAdapter;
import com.fragment.*;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private ViewPager vp;
    private TabLayout mTabLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        vp = findViewById(R.id.view_pager);
        mTabLayout = findViewById(R.id.tab_layout);
        List<String> mTitles = new ArrayList<>();
        mTitles.add("Home");
        mTitles.add("Map");
        mTitles.add("Set");
        List<Fragment> mFragments = new ArrayList<>();
        mFragments.add(new HomeFragment());
        mFragments.add(new MapFragment());
        mFragments.add(new SetFragment());
        ViewPagerAdapter adapter = new ViewPagerAdapter(getSupportFragmentManager(), mFragments, mTitles);
        vp.setAdapter(adapter);
        mTabLayout.setupWithViewPager(vp);
    }
}