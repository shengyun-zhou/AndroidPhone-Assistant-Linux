package com.buptsse.zero.phoneassistant.phoneinfoprovider;

/**
 * Created by buptsse-zero on 11/7/15.
 */
public class ContactInfo
{
    private String displayName;
    private String phoneNumber;
    private long contactID;

    public ContactInfo(String displayName, String phoneNumber, long contactID)
    {
        this.displayName = displayName;
        this.phoneNumber = phoneNumber;
        this.contactID = contactID;
    }

    public long getContactID() {
        return contactID;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getPhoneNumber() {
        return phoneNumber;
    }
}
