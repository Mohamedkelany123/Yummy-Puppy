#ifndef _ACCRUAL_INTEREST_FUNC_H_
#define _ACCRUAL_INTEREST_FUNC_H_

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <AccrualInterest.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
} AccrualInterestStruct;

void AccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);
void PartialAccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);
void SettlementAccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

#endif 
