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

        int merchant_commission_expense;
        int merchant_merchandise_value;
        int cashier_commission_expense;
        int merchant_deferred_commission_expense;
        int merchant_repayment_fee_expense;
        int merchant_notes_payable;
        int collection_of_cash_in_transit;
        int collection_of_commission_income;
        int collection_of_upfront_fees;
        int collection_of_transaction_investigation_fee;

    public:
        map<string, funcPtr> funcMap;
        DueForSettlement(loan_app_loan_primitive_orm *_lal_orm);
        ~DueForSettlement();
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);

        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        int getMerchantCommissionExpense();
        int getMerchantMerchandiseValue();
        int getCashierCommissionExpense();
        int getMerchantDeferredCommissionExpense();
        int getMerchantRepaymentFeeExpense();
        int getMerchantNotesPayable();
        int getCollectionOfCashInTransit();
        int getCollectionOfCommissionIncome();
        int getCollectionOfUpfrontFees();
        int getCollectionOfTransactionInvestigationFee();

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
        void stampORMS(ledger_entry_primitive_orm* _entry);



        static loan_app_loan_primitive_orm_iterator* aggregator(QueryExtraFeilds * query_fields,  string _start_fiscal_year);
        static void update_step(); 


};

#endif
