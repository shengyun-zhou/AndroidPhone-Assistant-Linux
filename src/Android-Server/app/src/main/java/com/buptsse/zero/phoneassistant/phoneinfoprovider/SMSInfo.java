package com.buptsse.zero.phoneassistant.phoneinfoprovider;

/**
 * Created by buptsse-zero on 11/8/15.
 */
public class SMSInfo
{
    private long date;
    private String phoneNumber;
    private String SMSBody;
    private int type;

    public SMSInfo(long date, String phoneNumber, String SMSBody, int type)
    {
        this.date = date;
        this.phoneNumber = phoneNumber;
        this.SMSBody = SMSBody;
        this.type = type;
    }

    public long getDate() {
        return date;
    }

    public int getType() {
        return type;
    }

    public String getPhoneNumber() {
        return phoneNumber;
    }

    public String getSMSBody() {
        return SMSBody;
    }
}
