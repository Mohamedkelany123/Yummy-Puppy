#ifndef CANCEL_LOANS_H
#define CANCEL_LOANS_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>


class CancelLoan : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        int template_id;

    public:
        map<string, funcPtr> funcMap;
        CancelLoan(loan_app_loan_primitive_orm* _orms);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_template_id(int _template_id);

        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        int get_template_id();


        LedgerAmount * _init_ledger_amount();

        // //static methods
        static LedgerAmount * _get_upfront_fee(LedgerClosureStep *disburseLoan);


        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string, int _agg_number=0);
        static void update_step(); 


    ~CancelLoan();
};

#endif
