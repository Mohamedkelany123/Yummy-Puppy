#ifndef INITIALLOANINTERESTACCRUAL_H
#define INITIALLOANINTERESTACCRUAL_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>

class InitialLoanInterestAccrual : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        BDate last_installment_principal_paid_at;
        bool is_first_date;       

        loan_app_loan_primitive_orm_iterator* loans_to_get_first_accrual_agg(string _closure_date_string);
        loan_app_loan_primitive_orm_iterator* loans_to_get_second_accrual_agg(string _closure_date_string);
    public:
        map<string, funcPtr> funcMap;
        InitialLoanInterestAccrual();
        InitialLoanInterestAccrual(loan_app_loan_primitive_orm * _lal_orm,bool _is_first_date);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_is_first_date(int _is_first_date);
        void set_is_first_date(BDate _last_installment_principal_paid_at);

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        bool get_is_first_date();
        BDate get_last_installment_principal_paid_at();

        LedgerAmount * _init_ledger_amount();
        void stampORMs(ledger_entry_primitive_orm* entry);

        //static methods
        static LedgerAmount * _get_accrued_interest(LedgerClosureStep *InitialLoanInterestAccrual);

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static loan_app_loan_primitive_orm_iterator* aggregator(string _closure_date_string, int _agg_number=0);
        static void update_step(); 

    ~InitialLoanInterestAccrual();
};


#endif