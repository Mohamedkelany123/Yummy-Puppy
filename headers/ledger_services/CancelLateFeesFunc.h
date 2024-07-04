#ifndef CANCELLATEFEESFUNC_H
#define CANCELLATEFEESFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <CancelLateFees.h>
#include <common_orm.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;

} CancelLateFeesStruct;

void CancelLateFeesFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif 
