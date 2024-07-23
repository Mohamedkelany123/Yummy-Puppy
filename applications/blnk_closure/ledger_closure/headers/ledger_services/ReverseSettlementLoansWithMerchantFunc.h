#ifndef REVERSESETTELMENTLOANSWITHMERCHANTSFUNC_H
#define REVERSESETTELMENTLOANSWITHMERCHANTSFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    map<int, loan_app_loan_primitive_orm*>* loanMap;
} SettlementLoansWithMerchantStruct;

void processLoanOrms (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);
void getMerchantPaymentRequestLoansFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);
void settleLoansWithMerchant(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);
#endif 
