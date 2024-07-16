#ifndef UNMARGINLIZE_H
#define UNMARGINLIZE_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>






class Unmarginalize : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        crm_app_customer_primitive_orm* cac_orm;
        // float prov_percentage;
        // float short_term_principal, long_term_principal;
        // bool is_rescheduled;
        // vector <new_lms_installmentextension_primitive_orm *> * ie_list;

        // json transaction_upfront_income_banked;
        // json transaction_upfront_income_unbanked;

        

    public:
        map<string, funcPtr> funcMap;
        Unmarginalize(vector<map <string,PSQLAbstractORM *> * > * _orms_list, float _percentage);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_template_id(int _template_id);
        // void set_provision_percentage(float _provision_percentage);
        // void set_short_term_principal(float _short_term_principal);
        // void set_is_rescheduled(bool _is_rescheduled);
        // void set_long_term_principal(float _long_term_principal);
        void set_crm_app_customer(crm_app_customer_primitive_orm *_cac_orm);

        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        crm_app_customer_primitive_orm *get_crm_app_customer();
        // json get_transaction_upfront_income_banked();
        // json get_transaction_upfront_income_unbanked();

        // float get_provision_percentage();
        // float get_short_term_principal();
        int get_template_id();
        // float get_long_term_principal();
        // bool get_is_rescheduled();



        // float _calculate_loan_upfront_fee();
        LedgerAmount * _init_ledger_amount();

        void stampORMs(ledger_entry_primitive_orm* entry, ledger_amount_primitive_orm * la_orm);

        // //static methods
        // static LedgerAmount * _calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan);
        // static LedgerAmount * _calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan);
        // static LedgerAmount * _calc_provision_percentage(LedgerClosureStep *disburseLoan);
        // static LedgerAmount * _calc_cashier_fee(LedgerClosureStep *disburseLoan);
        // static LedgerAmount * _calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan);
        // static LedgerAmount * _calc_loan_upfront_fee(LedgerClosureStep *disburseLoan);
        // static LedgerAmount * _calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan);

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        static void update_step(); 
        static map <string,map<int,pair<new_lms_installmentextension_primitive_orm*,vector<vector<new_lms_installmentlatefees_primitive_orm*> *> *> *> *> *  get_date_map(vector<map <string,PSQLAbstractORM *> * > * orms);

        

    ~Unmarginalize();
};




#endif