#ifndef _DUETOOVERDUEFUNC_H_
#define _DUETOOVERDUEFUNC_H_

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <DueToOverdue.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    BDate closing_day;
} DueToOverdueStruct;

void InstallmentBecomingOverdueFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif 
