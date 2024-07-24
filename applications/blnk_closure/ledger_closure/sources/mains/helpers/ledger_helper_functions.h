#ifndef LEDGER_HELPER_FUNCTIONS_H
#define LEDGER_HELPER_FUNCTIONS_H

#include <common_orm.h>
#include <common.h>

map<int,float> get_loan_status_provisions_percentage();
float get_iscore_nid_inquiry_fee();
float get_iscore_credit_expense_fee();
vector<string> get_start_and_end_fiscal_year();


#endif