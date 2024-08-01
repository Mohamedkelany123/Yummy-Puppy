#ifndef _SETTLEMENTLOANSWITHMERCHANT_H_
#define _SETTLEMENTLOANSWITHMERCHANT_H_

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>



class SettlementLoansWithMerchant : public LedgerClosureStep
{
    private:
        double amount;
        int loan_id, payment_request_id, merchant_id, customer_id, category, activation_user_id;
        bool is_settlement, is_reverse;

    public:
        map<string, funcPtr> funcMap;
        SettlementLoansWithMerchant();
        SettlementLoansWithMerchant(double _amount, int _loan_id, int _payment_request_id, int _merchant_id, int _customer_id, int _category, int _activation_user_id, bool _is_reverse);        

        LedgerAmount * _init_ledger_amount();

        void stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms);
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        // //static methods
        static LedgerAmount* get_amount(LedgerClosureStep* settlementLoansWithMerchant);
        static void unstampLoans();
        static LedgerAmount* _settle_with_merchant(LedgerClosureStep* settlementLoansWithMerchant);

        bool checkAmounts();

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string, string _start_fiscal_year);

        static void update_step(); 

        ~SettlementLoansWithMerchant();
};

string calculateAmountSum(string _account_name, string loan_id, string _start_fiscal_year,string _closing_day);



#endif