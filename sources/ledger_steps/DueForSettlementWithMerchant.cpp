#include "DueForSettlementWithMerchant.h"

DueForSettlement::DueForSettlement(map<string, PSQLAbstractORM *> *_orms)
{

}

DueForSettlement::~DueForSettlement(){}


void DueForSettlement::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
}

loan_app_loan_primitive_orm *DueForSettlement::get_loan_app_loan()
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_init_ledger_amount()
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_merchant_commission_expense(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_merchant_merchandise_value(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_cashier_commission_expense(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_merchant_deferred_commission_expense(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_merchant_repayment_fee_expense(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_merchant_notes_payable(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_collection_of_cash_in_transit(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_collection_of_commission_income(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_collection_of_upfront_fees(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

LedgerAmount *DueForSettlement::_get_collection_of_transaction_investigation_fee(LedgerClosureStep *dueForSettlement)
{
    return nullptr;
}

void DueForSettlement::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
}

loan_app_loan_primitive_orm_iterator *DueForSettlement::aggregator(string _closure_date_string, string _start_fiscal_year)
{
    loan_app_loan_primitive_orm_iterator * dueForSettlementIterator = new loan_app_loan_primitive_orm_iterator("main");

      dueForSettlementIterator->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.settlement_to_merchant_date",lte,_closure_date_string),
            
            // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::SETTLEMENT_WITH_MERCHANT-1)),
            new UnaryOperator ("loan_app_loan.settlement_to_merchant_ledger_entry_id" , isnull,"",true),

            new UnaryOperator ("loan_app_loan.id",nin, 
                "SELECT loan_id \
                    FROM loan_app_loanstatushistroy lal2 \
                    WHERE lal2.status_id IN (12, 13)\
                        AND lal2.status_type = 0\
                        AND lal2.day <= loan_app_loan.settlement_to_merchant_date"
            )
        )
    );

    dueForSettlementIterator->addExtraFromField("WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Impairment provisions, On-balance sheet'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id and le.entry_date between " + _start_fiscal_year['0'] + " and " + _closure_date_string,"impairment_provisions_balance");
        
    return dueForSettlementIterator;



    return nullptr;
}

void DueForSettlement::update_step()
{
}

