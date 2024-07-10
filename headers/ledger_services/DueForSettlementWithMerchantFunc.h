#ifndef DUEFORSETTLEMENTWITHMERCHANTFUNC_H_
#define DUEFORSETTLEMENTWITHMERCHANTFUNC_H_

#include <common.h>
#include <common_orm.h>
#include <functional>
#include <TemplateManager.h>
#include <DueForSettlementWithMerchant.h>



typedef struct {
    BlnkTemplateManager*  blnkTemplateManager;
} DueForSettlementStruct;

void dueForSettlementWithMerchantFunc(loan_app_loan_primitive_orm * _lal_orm, int partition_number, mutex* shared_lock,void * extras);

#endif