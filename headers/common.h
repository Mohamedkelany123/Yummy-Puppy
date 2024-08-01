#ifndef COMMON_H
#define COMMON_H

#include <utils.h>
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
#include <math.h>
#include <cmath>
#include <set>
#include <boost/format.hpp>
#include <iterator>
#include <chrono> 





using namespace std;
using json = nlohmann::json;

#define ORM(T,O) ((T##_primitive_orm *)((*O)[#T]))
#define ORMBL(T,O) ((T##_bl_orm *)((*O)[#T]))

#define ORML(T,O,I) ((T##_primitive_orm *)((*((*O)[I]))[#T]))
#define ORML_SIZE(O) (O->size())


#define ROUND(x) (round(x*100) / 100)

enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME_STEP1,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY, CUSTOMER_WALLET, PREPAID_TRANSACTION };

enum ledger_status {
    LEDGER_START = 0, DISBURSE_LOAN, FIRST_INCOME_ACCRUAL, CANCEL_LOAN, PARTIAL_INTEREST_ACCRUAL, INTEREST_ACCRUAL, SETTLEMENT_INTEREST_ACCRUAL, LEDGER_UNDUE_TO_DUE,
    LEDGER_DUE_TO_OVERDUE, MARGINALIZE_INCOME, CANCEL_LATE_FEES, RECLASSIFY_LONG_TERM, WRITE_OFF_LOAN, WALLET_PREPAID, REPAYMENT_BY_CUSTOMER, SETTLEMENT_BY_CUSTOMER, 
    REVERSE_MARGINALIZATION, UPDATE_PROVISION, REVERSE_WALLET, REVERSE_REPAYMENT, REVERSE_SETTLEMENT, FAWRY_SETTLEMENT = 30, ONBOARDING_COMMISSION, NID_ISCORE, CREDIT_ISCORE,
    SETTLEMENT_WITH_MERCHANT, FAILED_LMS_CLOSURE = -100
};

enum onboardingCommissionLedgerStamp { NOT_STAMPED , STAMPED, NO_COMMISSION };

enum blnk_buckets { NONE, CURRENT, BUCKET1, BUCKET2, BUCKET3, BUCKET4, SETTLED, WRITEOFF, SETTLED_PAID_OFF, BUCKET5,BUCKET6,BUCKET7,CANCELLED, CANCELLED_PARTIAL_REFUND,PARTIAL_SETTLED_CHARGE_OFF,SETTLED_CHARGE_OFF };

// #define TIME_ZONE_OFFEST 2


#define force_inline 


#endif