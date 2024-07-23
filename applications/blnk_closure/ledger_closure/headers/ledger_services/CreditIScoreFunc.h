#ifndef CREDITISCOREFUNC_H
#define CREDITISCOREFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <CreditIScore.h>
#include <common_orm.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    float expense_fee;

} CreditIScoreStruct;

void CreditIScoreFunc (map<string,PSQLAbstractORM *> * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif 
