#ifndef _SETTLEMENT_BY_CUSTOMER_FUNC_H_
#define _SETTLEMENT_BY_CUSTOMER_FUNC_H_

#include <common.h>
#include <common_orm.h>
#include <functional>
#include <TemplateManager.h>
#include <SettlementByCustomer.h>



typedef struct {
    BlnkTemplateManager* blnkTemplateManager;
    BDate closing_day;
} SettlementByCustomerStruct;

void settlementByCustomerFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif