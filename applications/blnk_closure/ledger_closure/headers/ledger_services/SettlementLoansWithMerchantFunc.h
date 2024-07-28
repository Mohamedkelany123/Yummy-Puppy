#ifndef SETTELMENTLOANSWITHMERCHANTSFUNC_H
#define SETTELMENTLOANSWITHMERCHANTSFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <common_orm.h>
#include "SettlementLoansWithMerchant.h"

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
} SettlementLoansWithMerchantStruct;

void settleLoansWithMerchantFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);
#endif 
