package com.utils;

import android.annotation.SuppressLint;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.content.Context;

import com.bean.Record;

import java.util.ArrayList;
import java.util.List;

public class DbSqliteHelper extends SQLiteOpenHelper {
    /**
     * 声明一个AndroidSDK自带的数据库变量db
     */
    private SQLiteDatabase db;

    /**
     * 写一个这个类的构造函数，参数为上下文context，所谓上下文就是这个类所在包的路径
     */
    public DbSqliteHelper(Context context){
        super(context,"db_test",null,1);
        db = getReadableDatabase();
    }

    /**
     * 重写两个必须要重写的方法，因为class DBOpenHelper extends SQLiteOpenHelper
     * 而这两个方法是 abstract 类 SQLiteOpenHelper 中声明的 abstract 方法
     * 所以必须在子类 DBOpenHelper 中重写 abstract 方法
     * @param db
     */
    @Override
    public void onCreate(SQLiteDatabase db){
        db.execSQL("CREATE TABLE IF NOT EXISTS records(" +
                "_id INTEGER PRIMARY KEY AUTOINCREMENT," +
                "equipment TEXT," +
                "pm TEXT," +
                "place TEXT," +
                "time TEXT)");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion){
        db.execSQL("DROP TABLE IF EXISTS users");
        onCreate(db);
    }

    //添加
    public boolean addRecord(String equipment,String pm,String place,String time){
        ContentValues values = new ContentValues();
        values.put("equipment", equipment);
        values.put("pm", pm);
        values.put("place",place);
        values.put("time", time);
        return db.insert("records","_id",values)>0;
    }

    //获取所有
    @SuppressLint("Range")
    public List<Record> getAllRecords(){
        List<Record> list=new ArrayList<>();
        Cursor cursor = db.query("records",null,null,null,null,null,null);
        while (cursor.moveToNext()) {
            Record data = new Record();
            data.setId(cursor.getInt(cursor.getColumnIndex("_id")));
            data.setEquipment(cursor.getString(cursor.getColumnIndex("equipment")));
            data.setPm(cursor.getString(cursor.getColumnIndex("pm")));
            data.setPlace(cursor.getString(cursor.getColumnIndex("place")));
            data.setTime(cursor.getString(cursor.getColumnIndex("time")));
            list.add(data);
        }
        return list;
    }
}
