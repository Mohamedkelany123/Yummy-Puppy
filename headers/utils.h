#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define TIME_ZONE_OFFEST 2

class BDate
{

    private:
        struct tm tm; 
        bool is_null;
    public:
        void set_date (string date_string="")
        {
            is_null = false;
            if (date_string != "")
            {
                date_string += " 00:00:00";
                strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
                tm.tm_hour +=TIME_ZONE_OFFEST;
            }
            else
            {
                is_null = true;
                date_string = "1970-01-01 00:00:00";
                strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
                tm.tm_hour +=TIME_ZONE_OFFEST;
            }
        }
        BDate(string date_string="")
        {
            set_date(date_string);
        }
        BDate (struct tm & _tm)
        {
            tm = _tm;
        }
        time_t operator () ()
        {
            return std::mktime(&tm);
        }
        void inc_month ()
        {
            tm.tm_mon ++;
            (*this)();
        }
        void dec_month ()
        {
            tm.tm_mon --;
            (*this)();
        }
        void inc_months (int months=1)
        {
            tm.tm_mon+= months;
            (*this)();
        }
        void dec_months (int months=1)
        {
            tm.tm_mon-= months;
            (*this)();
        }
        void dec_day ()
        {
            tm.tm_mday --;
            (*this)();
        }
        void inc_day ()
        {
            tm.tm_mday ++;
            (*this)();
        }
        void inc_days (int days=1)
        {
            tm.tm_mday+= days;
            (*this)();
        }
        void dec_days (int days=1)
        {
            tm.tm_mday-= days;
            (*this)();
        }
        string getDateString()
        {
            char buf[255];
            memset ( buf,0,255);
            if ( !is_null)
                strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
            string date_string = buf;
            return date_string;
        }
        ~BDate () {}
};



#endif