#ifndef INITIALLOANINTERESTACCRUALFUNC_H
#define INITIALLOANINTERESTACCRUALFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <InitialLoanInterestAccrual.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    bool is_first_date;    
} InitialLoanInterestAccrualStruct;

void InitialLoanInterestAccrualFunc (loan_app_loan_primitive_orm * lal_orm, int partition_number, mutex* shared_lock,void * extras);

#endif 
