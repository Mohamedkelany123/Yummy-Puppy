#ifndef UPDATINGPROVISIONS_FUNC_H
#define UPDATINGPROVISIONS_FUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <UpdatingProvisions.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    string closingDate;
    string startDate;
    string endDate;
} UpdatingProvisionsStruct;


void UpdatingProvisionsFunc(map<string,PSQLAbstractORM*>*orms,int partition_number, mutex *shared_lock, void * extras);

#endif