#include <utils.h>

void BDate::set_date (string date_string)
{
    is_null = false;
    if (date_string != "")
    {
        date_string += " 00:00:00";
        strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S %Z",&tm);    
        // tm.tm_hour +=TIME_ZONE_OFFEST;
        //local time zone and daylight saving time settings
        tm.tm_isdst = DL_SAVING;

    }
    else
    {
        is_null = true;
        date_string = "1970-01-01 00:00:00 UTC";
        strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S %Z",&tm);    
        // tm.tm_hour +=TIME_ZONE_OFFEST;
        //local time zone and daylight saving time settings
        tm.tm_isdst = DL_SAVING;

    }
}
BDate::BDate()
{
    init_current_date();
}

BDate::BDate(string date_string)
{
    is_null = false;
    set_date(date_string);
}

BDate::BDate (struct tm & _tm)
{
    is_null = false;
    tm = _tm;
}
BDate::BDate(time_t t)
{
    is_null = false;
    tm = *localtime(&t);
}
time_t BDate::operator()()
{
    setenv("TZ", "", 1);
    tzset();
    time_t t = mktime(&tm);
    return t;
}

bool BDate::operator > (BDate & bdate)
{
    return ((*this) () > bdate());
}
bool BDate::operator < (BDate & bdate)
{
    return ((*this) () < bdate());
}
bool BDate::operator >= (BDate & bdate)
{
    return ((*this) () >= bdate());

}
bool BDate::operator <= (BDate & bdate)
{
    return ((*this) () <= bdate());

}
bool BDate::operator == (BDate & bdate)
{
    return ((*this) () == bdate());
}
bool BDate::operator != (BDate & bdate)
{
    return ((*this) () != bdate());
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
void BDate::set_day (int _day)
{
        if ( _day >0 &&
                ((_day < 32 && (tm.tm_mon == 1 || tm.tm_mon==3 || tm.tm_mon==5 || tm.tm_mon==7 || tm.tm_mon==8 || tm.tm_mon==10 || tm.tm_mon==12)) || _day< 31 ))
        {
                tm.tm_mday = _day;
                (*this)();
        }
}
int BDate::diff_days(BDate bdate)
{
        return ((*this)() - bdate())/(60*60*24);
}
bool BDate::is_leap_year()
{
      return (tm.tm_year % 4 == 0 && tm.tm_year % 100 != 0) || (tm.tm_year % 400 == 0);
}

int BDate::get_month_days()
{
        if ( tm.tm_mon == 2 )
                if ( is_leap_year() ) return 29; else return 28;
        else if (tm.tm_mon == 1 || tm.tm_mon==3 || tm.tm_mon==5 || tm.tm_mon==7 || tm.tm_mon==8 || tm.tm_mon==10 || tm.tm_mon==12) return 31;
        else return 30;
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
string BDate::getFullDateString()
{
    char buf[255];
    memset ( buf,0,255);
    if ( !is_null)
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    string date_string = buf;
    return date_string;
}

void BDate::printTM()
{
    printf ("Year: %d\n",tm.tm_year);
    printf ("Month: %d\n",tm.tm_mon);
    printf ("day: %d\n",tm.tm_mday);
    printf ("Hr: %d\n",tm.tm_hour);
    printf ("min: %d\n",tm.tm_min);
    printf ("sec: %d\n",tm.tm_sec);
    printf ("tm_isdst: %d\n",tm.tm_isdst);

}

void BDate::init_current_date()
{
    time_t rawtime;
    time ( &rawtime );
    is_null = false;
    tm = *localtime(&rawtime);
    tm.tm_hour +=TIME_ZONE_OFFEST;
}

string BDate::getFullateString()
{
    char buf[255];
    memset ( buf,0,255);
    if ( !is_null)
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    string date_string = buf;
    return date_string;
}

void BDate::printTM()
{
    printf ("Year: %d\n",tm.tm_year);
    printf ("Month: %d\n",tm.tm_mon);
    printf ("day: %d\n",tm.tm_mday);
    printf ("Hr: %d\n",tm.tm_hour);
    printf ("min: %d\n",tm.tm_min);
    printf ("sec: %d\n",tm.tm_sec);
    printf ("tm_isdst: %d\n",tm.tm_isdst);

}


BDate::~BDate () {}