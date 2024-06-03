#include <utils.h>

void BDate::set_date (string date_string)
{
    is_null = false;
    if (date_string != "")
    {
        date_string += " 00:00:00";
        strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
        tm.tm_hour +=TIME_ZONE_OFFEST;
        //local time zone and daylight saving time settings
        tm.tm_isdst = DL_SAVING;
    }
    else
    {
        is_null = true;
        date_string = "1970-01-01 00:00:00";
        strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
        tm.tm_hour +=TIME_ZONE_OFFEST;
        //local time zone and daylight saving time settings
        tm.tm_isdst = DL_SAVING;
    }
}
BDate::BDate(string date_string)
{
    set_date(date_string);
}
BDate::BDate (struct tm & _tm)
{
    tm = _tm;
}
time_t BDate::operator () ()
{
    return mktime(&tm);
}
void BDate::inc_month ()
{
    tm.tm_mon ++;
    (*this)();
}
void BDate::dec_month ()
{
    tm.tm_mon --;
    (*this)();
}
void BDate::inc_months (int months)
{
    tm.tm_mon+= months;
    (*this)();
}
void BDate::dec_months (int months)
{
    tm.tm_mon-= months;
    (*this)();
}
void BDate::dec_day ()
{
    tm.tm_mday --;
    (*this)();
}
void BDate::inc_day ()
{
    tm.tm_mday ++;
    (*this)();
}
void BDate::inc_days (int days)
{
    tm.tm_mday+= days;
    (*this)();
}
void BDate::dec_days (int days)
{
    tm.tm_mday-= days;
    (*this)();
}
int BDate::get_day()
{
    return tm.tm_mday;
}
string BDate::getDateString()
{
    char buf[255];
    memset ( buf,0,255);
    if ( !is_null)
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    string date_string = buf;
    return date_string;
}
BDate::~BDate () {}