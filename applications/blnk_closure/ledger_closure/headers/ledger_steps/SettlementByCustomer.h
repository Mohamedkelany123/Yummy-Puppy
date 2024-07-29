#ifndef _SETTLEMENT_BY_CUSTOMER_H_
#define _SETTLEMENT_BY_CUSTOMER_H_

#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>

class SettlementByCustomer : public LedgerClosureStep{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        loan_app_installment_primitive_orm* lai_orm;
        new_lms_installmentextension_primitive_orm* nli_orm;
        payments_loanorder_primitive_orm* plo_orm;
        vector <new_lms_installmentlatefees_primitive_orm*>* lf_orms;

        int unmarginalization_template_id;
        int last_status;
        BDate closing_day;
        BDate* settlement_day;
        float paid_securitization_amount;

        //Cash in escrow fields 
        float cash_in_escrow;
        
        // Leg IDs for stamping
        int principal_legs[8] = {6,8,10,11,18,20,22,23}; 
        int interest_legs[6] = {5,7,9,17,19,21}; 
        int early_legs[2] = {12,24}; 
        int lf_legs[2] = {6,16}; 
        int unmarg_lf_leg = 1;
        int unmarg_interest_leg = 2;

        int securitized_legs[9] = {16,17,18,19,20,21,22,23,24}; 

    public:
        SettlementByCustomer();
        ~SettlementByCustomer();
        SettlementByCustomer(loan_app_loan_primitive_orm * _lal_orm, payments_loanorder_primitive_orm* _plo_orm,
            loan_app_installment_primitive_orm* _lai_orm,  new_lms_installmentextension_primitive_orm* _nli_orm,
                vector <new_lms_installmentlatefees_primitive_orm*>* _lf_orms, int _unmarginalization_template_id, BDate _closing_day, string _settlement_day, int _last_status,
                    float _starting_cash_in_escrow);
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        LedgerAmount * _init_ledger_amount();

        //Getters

        loan_app_loan_primitive_orm *get_loan_app_loan();
        payments_loanorder_primitive_orm* get_payments_loanorder();
        loan_app_installment_primitive_orm *get_loan_app_installment();
        new_lms_installmentextension_primitive_orm* get_new_lms_installmentextension();
        vector<new_lms_installmentlatefees_primitive_orm*>* get_new_lms_installmentlatefees();

        //Field getters
        int get_unmarginalization_template_id();
        int get_last_status();
        BDate* get_settlement_day();
        BDate get_closing_day();
        float get_cash_in_escrow();
        float get_paid_sec_amount();


        //Field setters
        void set_cash_in_escrow(float amount);
        void set_paid_sec_amount(float amount);

        //Calculators
        static LedgerAmount* _skip_leg(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_marginalized_late_fees(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_marginalized_interest(LedgerClosureStep* settlementByCustomer);
        //////////////////////////////////////////////////////////////////////////////////////////
        static LedgerAmount* _get_late_fees_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_overdue_interest_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_overdue_principal_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_due_interest_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_due_principal_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_undue_interest_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_undue_principal_paid(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_principal_long_term(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_early_repayment_fee_income(LedgerClosureStep* settlementByCustomer);
        //////////////////////////////////////////////////////////////////////////////////////////
        static LedgerAmount* _get_late_fees_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_overdue_interest_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_overdue_principal_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_due_interest_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_due_principal_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_undue_interest_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_undue_principal_paid_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_principal_long_term_sec(LedgerClosureStep* settlementByCustomer);
        static LedgerAmount* _get_early_repayment_fee_income_sec(LedgerClosureStep* settlementByCustomer);
        ///////////////////////////////////////////////////////////////////////////////////////////

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        void stampORMs(map <string,LedgerCompositLeg*> * leg_amounts);
        static void update_step();
        // static PSQLJoinQueryIterator* aggregator22(string _closure_date_string, int agg_num, string processed_order_ids);

        // Helpers
        float get_principal_paid(loan_app_loan_primitive_orm* lal_orm, loan_app_installment_primitive_orm* lai_orm,new_lms_installmentextension_primitive_orm* nli_orm,payments_loanorder_primitive_orm* pl_orm);
        float get_interest_paid(LedgerClosureStep *settlementByCustomer,loan_app_loan_primitive_orm* lal_orm, loan_app_installment_primitive_orm* lai_orm,new_lms_installmentextension_primitive_orm* nli_orm,payments_loanorder_primitive_orm* pl_orm);
        static bool intArrayIncludes(int arr[], int size, int value);
};

#endif