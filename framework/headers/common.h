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
#include <iterator>
#include <chrono>
#include <TestUtils.h>

using namespace std;
using json = nlohmann::json;

#define ORM(T,O) ((T##_primitive_orm *)((*O)[#T]))
#define ORMBL(T,O) ((T##_bl_orm *)((*O)[#T]))

#define ORML(T,O,I) ((T##_primitive_orm *)((*((*O)[I]))[#T]))
#define ORML_SIZE(O) (O->size())


#define   SERIALIZER_TEMPLATE     void serialize (string str) { \
            std::stringstream json_string (str); \
            json_string >> ThorsAnvil::Serialize::jsonImporter((*this)); \
        }

#define   DESERIALIZER_TEMPLATE     string deserialize () { \
            std::ostringstream ss; \
            ss << ThorsAnvil::Serialize::jsonExporter((*this)); \
            return ss.str(); \
        }


#define  SERIALIZER_RESPONSE int status_code;\
        string message;\
        vector <ResponseError> errors;

#define  SERIALIZER_RESPONSE_SETTERS_H inline void setStatusCode(int _status_code){status_code = _status_code;}\
        inline void setMessage(string _message){message = _message;}\
        inline void setErrors(vector<ResponseError> _errors){errors = _errors;}\
        inline int getStatusCode(){return status_code;}\
        inline string getMessage(){return message;}\
        inline vector<ResponseError> getErrors(){return errors;}


// #define  SERIALIZER_RESPONSE_SETTERS_CPP(T) void T::set_status_code(int _status_code){status_code = _status_code;};\
//         void T::set_message(string _message){message = _message;};\
//         void T::set_errors(vector<ResponseError> _errors){errors = _errors;};

#define   SERIALIZER_TEMPLATE_H     void serialize (string str);
#define   DESERIALIZER_TEMPLATE_H     string deserialize ();

#define   SERIALIZER_TEMPLATE_CPP(T)    void T::serialize (string str) { \
            std::stringstream json_string (str); \
            json_string >> ThorsAnvil::Serialize::jsonImporter((*this)); \
        }
#define   DESERIALIZER_TEMPLATE_CPP(T)     string T::deserialize () { \
            std::ostringstream ss; \
            ss << ThorsAnvil::Serialize::jsonExporter((*this)); \
            return ss.str(); \
        }


#define STARTPOSTENDPOINT(I,O) return new EndpointPOSTService <I,O>( \
            [=] (HTTPRequest * p_httpRequest,I * inputSerializer,O * outputSerializer) {

#define STARTGETENDPOINT(O) return new EndpointGETService <O>( \
            [=] (HTTPRequest * p_httpRequest,O * outputSerializer) {

#define INITENDPOINT extern "C" HTTPService *create_object(PSQLControllerMaster * _psqlControllerMaster, json _config)\
        {\
                psqlController.initialize(_psqlControllerMaster);\
                psqlController.addDefault("created_at","now()",true,true);\
                psqlController.addDefault("updated_at","now()",true,true);\
                psqlController.addDefault("updated_at","now()",false,true);

#define ENDENDPOINT }

/*
                bool connect = psqlController.addDataSource("main","127.0.0.1",5432,"django_ostaz_08072024","postgres","postgres");\
                if (connect){\
                        cout << "Connected to DATABASE"  << endl;\
                }\
                else{\
                        cout << "FAILED TO CONNECT TO DB "<< endl;\
                }\
*/


#define DATASOURCE_GUARD(DS) if (!psqlController.isDataSource(DS)) {\
                cout << "ISDATASOURCE:" << !psqlController.isDataSource(DS) << endl;\
                }

#define ENDGETENDPOINT });

#define ENDPOSTENDPOINT });


#define ROUND(x) (round(x*100) / 100)

enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME_STEP1,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY, CUSTOMER_WALLET, PARTIAL_SETTLEMENT_EXPIRATION, PSR_EXTRA_MONEY, PREPAID_TRANSACTION };

enum ledger_status {
    LEDGER_START = 0, DISBURSE_LOAN, FIRST_INCOME_ACCRUAL, CANCEL_LOAN, PARTIAL_INTEREST_ACCRUAL, INTEREST_ACCRUAL, SETTLEMENT_INTEREST_ACCRUAL, LEDGER_UNDUE_TO_DUE,
    LEDGER_DUE_TO_OVERDUE, MARGINALIZE_INCOME, CANCEL_LATE_FEES, RECLASSIFY_LONG_TERM, WRITE_OFF_LOAN, WALLET_PREPAID, REPAYMENT_BY_CUSTOMER, SETTLEMENT_BY_CUSTOMER, 
    REVERSE_MARGINALIZATION, UPDATE_PROVISION, REVERSE_WALLET, REVERSE_REPAYMENT, REVERSE_SETTLEMENT, FAWRY_SETTLEMENT = 30, ONBOARDING_COMMISSION, NID_ISCORE, CREDIT_ISCORE,
    SETTLEMENT_WITH_MERCHANT, REMOVE_WALLET_EXTRA_MONEY, FAILED_LMS_CLOSURE = -100
};

enum onboardingCommissionLedgerStamp { NOT_STAMPED , STAMPED, NO_COMMISSION };

enum PartialSettlementRequestStatus { PENDING , APPROVED, REJECTED, PARTIALSETTLED, EXPIRED, CANCELLED2 };

enum blnk_buckets { NONE, CURRENT, BUCKET1, BUCKET2, BUCKET3, BUCKET4, SETTLED, WRITEOFF, SETTLED_PAID_OFF, BUCKET5,BUCKET6,BUCKET7,CANCELLED, CANCELLED_PARTIAL_REFUND,PARTIAL_SETTLED_CHARGE_OFF,SETTLED_CHARGE_OFF };

enum StatusCode {OK = 200,BADREQUEST = 400};

enum BuySessionStep {STARTED, CONTRACT_SCANNED, PROMISSORY_SCANNED, OTP_INSERTED, LOAN_CREATED};

enum SessionType {BUY_SESSION, REFUND_SESSION, ONBOARDING_SESSION};

enum DocumentType { INQUIRY, AGREEMENT, PROMISSORY };
// #define TIME_ZONE_OFFEST 2


#define force_inline 


#endif