#ifndef LAMBDAS_H
#define LAMBDAS_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>


std::function<void(std::map<std::string, PSQLAbstractORM*>*, int, std::mutex*)> DisburseLoanFunc(BlnkTemplateManager* blnkTemplateManager, float current_provision_percentage);


#endif 
