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
                psqlController.setBatchMode(false);\
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

enum CustomerState { LEAD, SUBMITTED, PENDING_UNBANKED, IDLE_LIMIT, DENIED, FUNDED, NON_COMPLIANT, CLOSED, MANUAL_REJECT, IDLE_CLOSED };

enum onboardingCommissionLedgerStamp { NOT_STAMPED , STAMPED, NO_COMMISSION };

enum PartialSettlementRequestStatus { PENDING , APPROVED, REJECTED, PARTIALSETTLED, EXPIRED, CANCELLED2 };

enum blnk_buckets { NONE, CURRENT, BUCKET1, BUCKET2, BUCKET3, BUCKET4, SETTLED, WRITEOFF, SETTLED_PAID_OFF, BUCKET5,BUCKET6,BUCKET7,CANCELLED, CANCELLED_PARTIAL_REFUND,PARTIAL_SETTLED_CHARGE_OFF,SETTLED_CHARGE_OFF };

enum StatusCode {OK = 200,BADREQUEST = 400, NOTFOUND = 404};

enum BuySessionStep {STARTED, CONTRACT_SCANNED, PROMISSORY_SCANNED, OTP_INSERTED, LOAN_CREATED};

enum SessionType {BUY_SESSION, REFUND_SESSION, ONBOARDING_SESSION};

enum DocumentType { INQUIRY, AGREEMENT, PROMISSORY };

enum OtpState { FRAUD_LINK=1, SMS_OTP, NOTIFICATION_OTP, DOWNLOAD_LINK};
        
enum SmsProvider { VODAFONE=1, TWILLIO, ORANGE, VICTORY, INFOBIP };

enum CustomerTransactionType { PURCHASE, PARTIAL_REFUND, FULL_REFUND };

const string BLNKLOGOBASE64 = "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";

#define force_inline 


#endif