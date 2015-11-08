package com.buptsse.zero.phoneassistant.phoneinfoprovider;

import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;

import java.util.ArrayList;

/**
 * Created by buptsse-zero on 11/8/15.
 */
public class SMSInfoReader
{
    private Context context;
    private final String SMS_URI_INBOX = "content://sms/inbox";

    public SMSInfoReader(Context context)
    {
        this.context = context;
    }

    public ArrayList<SMSInfo> getAllSMS()
    {
        ArrayList<SMSInfo> SMSInfoList = new ArrayList<SMSInfo>();
        ContentResolver contentResolver = context.getContentResolver();
        String[] projection = new String[] {"address","body", "date", "type"};
        Cursor cr = contentResolver.query(Uri.parse(SMS_URI_INBOX), projection, null, null, null);
        while(cr.moveToNext()){
            String phoneNumber = cr.getString(cr.getColumnIndex(projection[0]));
            String SMSBody = cr.getString(cr.getColumnIndex(projection[1]));
            long date = cr.getLong(cr.getColumnIndex(projection[2]));
            int type = cr.getInt(cr.getColumnIndex(projection[3]));
            SMSInfoList.add(new SMSInfo(date, phoneNumber, SMSBody, type));
        }
        return SMSInfoList;
    }
}
