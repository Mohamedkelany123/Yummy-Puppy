#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <algorithm>
#include <libpq-fe.h>
#include <time.h>
#include <mutex>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <dlfcn.h>
#include <stdarg.h>
#include <json.hpp>
#include <bitset>
#include <future>
#include <list>
#include <utils.h>

using namespace std;
using json = nlohmann::json;

#define ORM(T,O) ((T##_primitive_orm *)((*O)[#T]))

enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME_STEP1,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY,PREPAID_TRANSACTION };

enum blnk_buckets { NONE, CURRENT, BUCKET1, BUCKET2, BUCKET3, BUCKET4, SETTLED, WRITEOFF, SETTLED_PAID_OFF, BUCKET5,BUCKET6,BUCKET7,CANCELLED, CANCELLED_PARTIAL_REFUND,PARTIAL_SETTLED_CHARGE_OFF,SETTLED_CHARGE_OFF };

#define TIME_ZONE_OFFEST 2


#define force_inline 


#endif