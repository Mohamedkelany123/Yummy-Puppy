#ifndef DUEFORSETTLEMENTWITHMERCHANT_H
#define DUEFORSETTLEMENTWITHMERCHANT_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>


class DueForSettlement : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        
    public:
        map<string, funcPtr> funcMap;
        DueForSettlement(map<string, PSQLAbstractORM*>* _orms);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);

        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();


        LedgerAmount * _init_ledger_amount();

        // //static methods
        static LedgerAmount * _get_merchant_commission_expense(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_merchant_merchandise_value(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_cashier_commission_expense(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_merchant_deferred_commission_expense(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_merchant_repayment_fee_expense(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_merchant_notes_payable(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_collection_of_cash_in_transit(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_collection_of_commission_income(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_collection_of_upfront_fees(LedgerClosureStep *dueForSettlement);
        static LedgerAmount * _get_collection_of_transaction_investigation_fee(LedgerClosureStep *dueForSettlement);


        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string, int _agg_number=0);
        static void update_step(); 


    ~DueForSettlement();
};

#endif
