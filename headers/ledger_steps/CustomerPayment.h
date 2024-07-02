#ifndef _CUSTOMER_PAYMENT_H_
#define _CUSTOMER_PAYMENT_H_

#include <LedgerClosureStep.h>
#include <PSQLAbstractQueryIterator.h>
#include <payments_loanorder_primitive_orm.h>
#include <loan_app_loan_primitive_orm.h>
#include <payments_loanorderheader_primitive_orm.h>
#include <payments_paymentmethod_primitive_orm.h>
#include <payments_paymentprovider_primitive_orm.h>
#include <PSQLUpdateQuery.h>
#include <ledger_entry_primitive_orm.h>


class CustomerPayment : LedgerClosureStep {
    private:
    loan_app_loan_primitive_orm* lal_orm;
    payments_loanorder_primitive_orm* plo_orm;
    payments_loanorderheader_primitive_orm* ploh_orm;
    BDate closing_day;
    int template_id;
    int first_loan_order_id;

    public:
        CustomerPayment(loan_app_loan_primitive_orm* _lal_orm, payments_loanorder_primitive_orm* _plo_orm, payments_loanorderheader_primitive_orm* _ploh_orm, BDate _closing_day, int _template_id, int _first_loan_order_id);
        map<string, funcPtr> funcMap;
        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        loan_app_loan_primitive_orm* get_loan_app_loan();
        payments_loanorder_primitive_orm* get_payments_loanorder();
        payments_loanorderheader_primitive_orm* get_payments_loanorderheader();
        int get_first_loan_order_id();
        LedgerAmount * _init_ledger_amount();
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        static void update_step();
        static LedgerAmount* _get_order_income(LedgerClosureStep* customerPaymentStep);
        static LedgerAmount* _get_order_total_amount(LedgerClosureService* customerPaymentStep);
        static LedgerAmount* _get_remaining_escrow(LedgerClosureService* customerPaymentStep);
        void stampORMS(ledger_entry_primitive_orm* _entry);
};

#endif