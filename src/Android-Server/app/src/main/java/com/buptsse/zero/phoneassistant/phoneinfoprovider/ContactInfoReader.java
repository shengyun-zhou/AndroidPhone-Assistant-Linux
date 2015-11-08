package com.buptsse.zero.phoneassistant.phoneinfoprovider;

import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.provider.ContactsContract;

import java.util.ArrayList;

/**
 * Created by buptsse-zero on 11/7/15.
 */
public class ContactInfoReader
{
    private ContentResolver contentResolver;
    public ContactInfoReader(Context context)
    {
        contentResolver = context.getContentResolver();
    }

    public ArrayList<ContactInfo> getAllContacts()
    {
        ArrayList<ContactInfo> contactInfos = new ArrayList<ContactInfo>();
        String index[] = { ContactsContract.CommonDataKinds.Phone.CONTACT_ID, ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME,
                ContactsContract.CommonDataKinds.Phone.NUMBER };
        Cursor cur = contentResolver.query(
                ContactsContract.CommonDataKinds.Phone.CONTENT_URI, index, null, null, null);
        while(cur.moveToNext()){
            String phoneNum = cur.getString(cur.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER));
            String displayName = cur.getString(cur.getColumnIndex(ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME));
            long contactID = cur.getLong(cur.getColumnIndex(ContactsContract.CommonDataKinds.Phone.CONTACT_ID));
            System.out.println("DisplayName=" + displayName);
            contactInfos.add(new ContactInfo(displayName, phoneNum, contactID));
        }
        return contactInfos;
    }
}
