#ifndef DISBURSE_LOANS_H
#define DISBURSE_LOANS_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>






class DisburseLoan : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        crm_app_customer_primitive_orm* cac_orm;
        float prov_percentage;
        float short_term_principal, long_term_principal;
        bool created_from_rescheduling;
        vector <new_lms_installmentextension_primitive_orm *> * ie_list;

        json transaction_upfront_income_banked;
        json transaction_upfront_income_unbanked;

        

    public:
        map<string, funcPtr> funcMap;
        DisburseLoan(vector<map <string,PSQLAbstractORM *> * > * _orms_list, float _percentage);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_template_id(int _template_id);
        void set_provision_percentage(float _provision_percentage);
        void set_short_term_principal(float _short_term_principal);
        void set_created_from_rescheduling(bool _created_from_rescheduling);
        void set_long_term_principal(float _long_term_principal);
        void set_crm_app_customer(crm_app_customer_primitive_orm *_cac_orm);

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        crm_app_customer_primitive_orm *get_crm_app_customer();
        json get_transaction_upfront_income_banked();
        json get_transaction_upfront_income_unbanked();

        float get_provision_percentage();
        float get_short_term_principal();
        int get_template_id();
        float get_long_term_principal();
        bool get_created_from_rescheduling();

        float get_upfront_income_cash();
        LedgerAmount * _init_ledger_amount();

        void stampORMs(ledger_entry_primitive_orm* entry, ledger_amount_primitive_orm * la_orm);

        // //static methods
        static LedgerAmount * _calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan);
        static LedgerAmount * _calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount * _calc_provision_percentage(LedgerClosureStep *disburseLoan);
        static LedgerAmount * _calc_cashier_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount * _calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount * _calc_loan_upfront_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount * _calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan);

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(QueryExtraFeilds * query_fields);
        static void update_step(); 

        

    ~DisburseLoan();
};




#endif