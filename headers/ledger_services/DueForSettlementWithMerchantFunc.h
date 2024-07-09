#ifndef DUEFORSETTLEMENTWITHMERCHANTFUNC_H_
#define DUEFORSETTLEMENTWITHMERCHANTFUNC_H_

#include <common.h>
#include <common_orm.h>
#include <functional>
#include <TemplateManager.h>
#include <DueForSettlementWithMerchant.h>



typedef struct {
    map<int, BlnkTemplateManager*>* blnkTemplateManagerMap;
} DueForSettlementStruct;

void dueForSettlementWithMerchantFunc(map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

#endif