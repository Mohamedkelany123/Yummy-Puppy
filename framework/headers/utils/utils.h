#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


// g++ main.cpp -o my_program -D SUMMER 
// #ifdef SUMMER
//     #define TIME_ZONE_OFFEST 3
//     #define DL_SAVING        0
// #else
//     #define TIME_ZONE_OFFEST 2
//     #define DL_SAVING        -1
// #endif
//#define BDATE_TIME_ZONE "EEST"
#define BDATE_TIME_ZONE "UTC"

#define TIME_ZONE_OFFEST 2 
#define DL_SAVING        0

// #define SQLITE_OMIT_LOCALTIME 1

class BDate
{

    private:
        struct tm tm; 
        bool is_null;
    public:
        void set_date (string date_string="");
        BDate();
        BDate(string date_string);
        BDate (struct tm & _tm);
        BDate(time_t t);
        time_t operator () ();
        bool operator > (BDate & bdate);
        bool operator < (BDate & bdate);
        bool operator >= (BDate & bdate);
        bool operator <= (BDate & bdate);
        bool operator == (BDate & bdate);
        bool operator != (BDate & bdate);
        void inc_month ();
        void dec_month ();
        void inc_months (int months=1);
        void dec_months (int months=1);
        void dec_day ();
        void inc_day ();
        void inc_days (int days=1);
        void dec_days (int days=1);
        int get_day();
        void set_day(int _day);
        int get_max_month_date();
        int diff_days(BDate bdate);
        int get_month_days();
        bool is_leap_year();
        string getDateString();
        string getFullDateString();
        void init_current_date();
        void printTM();
        static int diff_days(BDate date1, BDate date2);
        ~BDate ();
};





#endif