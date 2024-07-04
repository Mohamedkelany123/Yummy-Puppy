#ifndef ONBOARDINGCOMMISSIONFUNCFUNC_H
#define ONBOARDINGCOMMISSIONFUNCFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <OnboardingCommission.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
} OnboardingCommissionStruct;

void OnboardingCommissionFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

#endif 
