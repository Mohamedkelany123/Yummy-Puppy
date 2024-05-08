#ifndef UTILS_H
#define UTILS_H

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

#define TIME_ZONE_OFFEST 3
#define DL_SAVING        0



class BDate
{

    private:
        struct tm tm; 
        bool is_null;
    public:
        void set_date (string date_string="");
        BDate(string date_string="");
        BDate (struct tm & _tm);
        time_t operator () ();
        void inc_month ();
        void dec_month ();
        void inc_months (int months=1);
        void dec_months (int months=1);
        void dec_day ();
        void inc_day ();
        void inc_days (int days=1);
        void dec_days (int days=1);
        string getDateString();
        ~BDate ();
};



#endif