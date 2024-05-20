#ifndef LAMBDAS_H
#define LAMBDAS_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    float current_provision_percentage;    
} DisburseLoanStruct;

void DisburseLoanFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

// std::function<void(std::map<std::string, PSQLAbstractORM*>*, int, std::mutex*)> DisburseLoanFunc(BlnkTemplateManager* blnkTemplateManager, float current_provision_percentage);


#endif 
