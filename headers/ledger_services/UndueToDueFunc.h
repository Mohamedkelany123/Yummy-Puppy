#ifndef UNDUETODUEFUNC_H
#define UNDUETODUEFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <UndueToDue.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    BDate closing_day;
} UndueToDueStruct;

void InstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);
void StickyInstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

#endif 
