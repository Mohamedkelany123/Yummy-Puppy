#ifndef CANCELFUNC_H
#define CANCELFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <CancelLoans.h>
#include <common_orm.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager_cancel;
    BlnkTemplateManager *blnkTemplateManager_reverse;

} CancelLoanStruct;

void CancelLoanFunc (loan_app_loan_primitive_orm * _lal_orm, int partition_number, mutex* shared_lock,void * extras);

#endif 
