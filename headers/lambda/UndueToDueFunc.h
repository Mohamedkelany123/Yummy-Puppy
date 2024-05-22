#ifndef UNDUETODUEFUNC_H
#define UNDUETODUECUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    float current_provision_percentage;    
} DisburseLoanStruct;

void InstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

#endif 
