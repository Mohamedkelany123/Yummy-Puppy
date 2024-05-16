#ifndef DISBURSE_LOANS_H
#define DISBURSE_LOANS_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>




class DisburseLoan : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        loan_app_loanproduct_primitive_orm* lalp_orm;
        int template_id;
        float prov_percentage;
        float short_term_principal;

        //<Leg Name VariableName(Function)>

        //init funcMap method


    public:
        map<string, funcPtr> funcMap;
        DisburseLoan(loan_app_loan_primitive_orm * _lal_orm, float short_term_principal, float long_term_principal, float percentage);

        // void generateFuncMap();
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_loan_app_loanproduct(loan_app_loanproduct_primitive_orm* _lalp_orm);
        void set_template_id(int _template_id);
        void set_provision_percentage(float _provision_percentage);
        void set_short_term_principal(float _short_term_principal);
        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_loanproduct_primitive_orm* get_loan_app_loanproduct();
        float get_provision_percentage();
        float get_short_term_principal();
        int get_template_id();


        LedgerAmount _init_ledger_amount();
        // //static methods
        static LedgerAmount _calc_short_term_receivable_balance_reschedled(LedgerClosureStep* disburseLoan);
        static LedgerAmount _calc_long_term_receivable_balance_reschedled(LedgerClosureStep* disburseLoan);
        static LedgerAmount _calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_provision_percentage(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_cashier_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_loan_upfront_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan);

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        

    ~DisburseLoan();
};






class DisburseLoanCalculator {

    public:
        
        
};



#endif