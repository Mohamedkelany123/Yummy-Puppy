#include <utils.h>
#include <reader.h>
#include<TestManager.h>

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
    string platform = "";
    try{
        ConfigReader& conf = ConfigReader::getInstance();
        platform = conf.GetValue("app_config", "platform");

    }
    catch (exception e){
        cerr<<"No config found using regular time for BDate"<<endl;
    }
    if (platform == "TEST"){
        string date_string = TestManager::getInstance().getTestDate();
        if (date_string == ""){
            init_current_date();
            return;
        }
        is_null = false;
        set_date(date_string);
        return;
    }
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
    // setenv("TZ", "", 1);
    // tzset();
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
    // mktime(&tm);
    (*this)();
}
void BDate::dec_month ()
{
    tm.tm_mon --;
    // mktime(&tm);
    (*this)();
}
void BDate::inc_months (int months)
{
    tm.tm_mon+= months;
    // mktime(&tm);
    (*this)();
}
void BDate::dec_months (int months)
{
    tm.tm_mon-= months;
    // mktime(&tm);
    (*this)();
}
void BDate::dec_day ()
{
    tm.tm_mday --;
    // mktime(&tm);
    (*this)();
}
void BDate::inc_day ()
{
    tm.tm_mday ++;
    // mktime(&tm);
    (*this)();
}
void BDate::inc_days (int days)
{
    tm.tm_mday+= days;
    // mktime(&tm);
    (*this)();
}

void BDate::inc_year(){
    tm.tm_year++;
    // mktime(&tm);
    (*this)();
}
void BDate::dec_year(){
    tm.tm_year--;
    // mktime(&tm);
    (*this)();
}
void BDate::dec_years(int _years){
    tm.tm_year-= _years ;
    // mktime(&tm);
    (*this)();
}

void BDate::inc_years(int _years){
    tm.tm_year+= _years;
    // mktime(&tm);
    (*this)(); 
}
void BDate::dec_days (int days)
{
    tm.tm_mday-= days;
    mktime(&tm);
    (*this)();
}
int BDate::get_day()
{
    return tm.tm_mday;
}

int BDate::get_month()
{
    return tm.tm_mon+1;
}
int BDate::get_year()
{
    return tm.tm_year;
}
void BDate::set_day (int _day)
{
        if ( _day >0 &&
                ((_day < 32 && (tm.tm_mon == 0 || tm.tm_mon==2 || tm.tm_mon==4 || tm.tm_mon==6 || tm.tm_mon==7 || tm.tm_mon==9|| tm.tm_mon==11)) || _day< 31 ))
        {
                tm.tm_mday = _day;
                mktime(&tm);
                (*this)();
        }
}
int BDate::diff_days(BDate bdate){
        return ((*this)() - bdate())/(60*60*24);
}
bool BDate::is_leap_year()
{
      return ((tm.tm_year+1900) % 4 == 0 && (tm.tm_year+1900) % 100 != 0) || ((tm.tm_year+1900) % 400 == 0);
}

int BDate::get_month_days()
{
        if ( tm.tm_mon == 1 )
                if ( is_leap_year() ) return 29; else return 28;
        else if (tm.tm_mon == 0 || tm.tm_mon==2 || tm.tm_mon==4 || tm.tm_mon==6 || tm.tm_mon==7 || tm.tm_mon==9|| tm.tm_mon==11) return 31;
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

string BDate::getArabicDayOfTheWeek()
{
    const std::string arabic_weekdays[] = {"الأحد", "الإثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت"};
    return arabic_weekdays[tm.tm_wday];
}

string BDate::getDayOfTheWeek()
{
    const std::string weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    return weekdays[tm.tm_wday];
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
    printf ("Year: %d\n",tm.tm_year+1900);
    printf ("Month: %d\n",tm.tm_mon+1);
    printf ("day: %d\n",tm.tm_mday);
    printf ("Hr: %d\n",tm.tm_hour);
    printf ("min: %d\n",tm.tm_min);
    printf ("sec: %d\n",tm.tm_sec);
    printf ("tm_isdst: %d\n",tm.tm_isdst);

}

int BDate::diff_days(BDate date1, BDate date2)
{
    time_t t1 = date1();
    time_t t2 = date2();
    
    if (t1 == -1 || t2 == -1) {
        std::cerr << "Error: Invalid date encountered." << std::endl;
        return -1;
    }

    // Calculate difference in seconds
    double diff_seconds = difftime(t1, t2);
    
    // Convert seconds to days
    return static_cast<int>(std::abs(diff_seconds / (60 * 60 * 24))); // Absolute value to avoid negative days
}

void BDate::init_current_date()
{
    time_t rawtime;
    time ( &rawtime );
    is_null = false;
    tm = *localtime(&rawtime);
    tm.tm_hour +=TIME_ZONE_OFFEST;
}
BDate::~BDate () {}