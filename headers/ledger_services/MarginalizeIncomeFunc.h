#ifndef _MARGINALIZEINCOMEFUNC_H_
#define _MARGINALIZEINCOMEFUNC_H_

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <MarginalizeIncome.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    BDate marginalization_day;
} MarginalizeIncomeStruct;

void MarginalizeIncomeFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif 
