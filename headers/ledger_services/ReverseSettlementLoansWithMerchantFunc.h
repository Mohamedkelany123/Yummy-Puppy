#ifndef REVERSESETTELMENTLOANSWITHMERCHANTSFUNC_H
#define REVERSESETTELMENTLOANSWITHMERCHANTSFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
} ReverseSettlementLoansWithMerchantStruct;

void reversesettlementLoansWithMerchantFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif 
