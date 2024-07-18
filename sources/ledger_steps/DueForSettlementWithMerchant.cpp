#include "DueForSettlementWithMerchant.h"



DueForSettlement::DueForSettlement(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;

    merchant_commission_expense = abs(_lal_orm->getExtraToInt("merchant_commission_expense"));
    merchant_merchandise_value = abs(_lal_orm->getExtraToInt("merchant_merchandise_value"));
    cashier_commission_expense = abs(_lal_orm->getExtraToInt("cashier_commission_expense"));
    merchant_deferred_commission_expense = abs(_lal_orm->getExtraToInt("merchant_deferred_commission_expense"));
    merchant_repayment_fee_expense = abs(_lal_orm->getExtraToInt("merchant_repayment_fee_expense"));
    merchant_notes_payable = abs(_lal_orm->getExtraToInt("merchant_notes_payable"));
    collection_of_cash_in_transit = abs(_lal_orm->getExtraToInt("collection_of_cash_in_transit"));
    collection_of_commission_income = abs(_lal_orm->getExtraToInt("collection_of_commission_income"));
    collection_of_upfront_fees = abs(_lal_orm->getExtraToInt("collection_of_upfront_fees"));
    collection_of_transaction_investigation_fee = abs(_lal_orm->getExtraToInt("collection_of_transaction_investigation_fee"));

}

DueForSettlement::~DueForSettlement(){}




void DueForSettlement::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

loan_app_loan_primitive_orm *DueForSettlement::get_loan_app_loan()
{   
    return lal_orm;
}

LedgerAmount *DueForSettlement::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    return lg;
}

LedgerAmount *DueForSettlement::_get_merchant_commission_expense(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int merchant_commission_expense = ((DueForSettlement*)dueForSettlement)->getMerchantCommissionExpense();
    ledgerAmount->setAmount(merchant_commission_expense);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_merchant_merchandise_value(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int merchant_merchandise_value = ((DueForSettlement*)dueForSettlement)->getMerchantMerchandiseValue();
    ledgerAmount->setAmount(merchant_merchandise_value);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_cashier_commission_expense(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int cashier_commission_expense = ((DueForSettlement*)dueForSettlement)->getCashierCommissionExpense();
    ledgerAmount->setAmount(cashier_commission_expense);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_merchant_deferred_commission_expense(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int merchant_deferred_commission_expense = ((DueForSettlement*)dueForSettlement)->getMerchantDeferredCommissionExpense();
    ledgerAmount->setAmount(merchant_deferred_commission_expense);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_merchant_repayment_fee_expense(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int merchant_repayment_fee_expense = ((DueForSettlement*)dueForSettlement)->getMerchantRepaymentFeeExpense();
    ledgerAmount->setAmount(merchant_repayment_fee_expense);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_merchant_notes_payable(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int merchant_notes_payable = ((DueForSettlement*)dueForSettlement)->getMerchantNotesPayable();
    ledgerAmount->setAmount(merchant_notes_payable);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_collection_of_cash_in_transit(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int collection_of_cash_in_transit = ((DueForSettlement*)dueForSettlement)->getCollectionOfCashInTransit();
    ledgerAmount->setAmount(collection_of_cash_in_transit);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_collection_of_commission_income(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int collection_of_commission_income = ((DueForSettlement*)dueForSettlement)->getCollectionOfCommissionIncome();
    ledgerAmount->setAmount(collection_of_commission_income);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_collection_of_upfront_fees(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int collection_of_upfront_fees = ((DueForSettlement*)dueForSettlement)->getCollectionOfUpfrontFees();
    ledgerAmount->setAmount(collection_of_upfront_fees);
    return ledgerAmount;
}

LedgerAmount *DueForSettlement::_get_collection_of_transaction_investigation_fee(LedgerClosureStep *dueForSettlement)
{
    LedgerAmount* ledgerAmount = ((DueForSettlement*)dueForSettlement)->_init_ledger_amount();
    int collection_of_transaction_investigation_fee = ((DueForSettlement*)dueForSettlement)->getCollectionOfTransactionInvestigationFee();
    ledgerAmount->setAmount(collection_of_transaction_investigation_fee);
    return ledgerAmount;
}

void DueForSettlement::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("Payment of merchant commission expense", _get_merchant_commission_expense);
    ledgerClosureService->addHandler("Payment of merchant merchandise value", _get_merchant_merchandise_value);
    ledgerClosureService->addHandler("Payment of cashier commission expense", _get_cashier_commission_expense);
    ledgerClosureService->addHandler("Payment of merchant deferred commissions expense", _get_merchant_deferred_commission_expense);
    ledgerClosureService->addHandler("Payment of merchant repayment fee expense", _get_merchant_repayment_fee_expense);
    ledgerClosureService->addHandler("Payment of merchant notes payable (short term)", _get_merchant_notes_payable);
    ledgerClosureService->addHandler("Collection of cash in transit from repayments at merchant", _get_collection_of_cash_in_transit);
    ledgerClosureService->addHandler("Collection of commission income", _get_collection_of_commission_income);
    ledgerClosureService->addHandler("Collection of upfront fees", _get_collection_of_upfront_fees);
    ledgerClosureService->addHandler("Collection of transaction investigation fee", _get_collection_of_transaction_investigation_fee);


}

void DueForSettlement::stampORMS(ledger_entry_primitive_orm *_entry)
{
    lal_orm->setUpdateRefernce("settlement_to_merchant_ledger_entry_id", _entry);
}

void DueForSettlement::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
    ANDOperator(
        new UnaryOperator ("loan_app_loan.id",ne,"14312"),
        new UnaryOperator ("loan_app_loan.closure_status",gte,0)
    ),
    {{"closure_status",to_string(ledger_status::SETTLEMENT_WITH_MERCHANT)}}

    );
    psqlUpdateQuery.update(); 
}

int DueForSettlement::getMerchantCommissionExpense()  {return merchant_commission_expense;}
int DueForSettlement::getMerchantMerchandiseValue()  {return merchant_merchandise_value;}
int DueForSettlement::getCashierCommissionExpense()  {return cashier_commission_expense;}
int DueForSettlement::getMerchantDeferredCommissionExpense()  {return merchant_deferred_commission_expense;}
int DueForSettlement::getMerchantRepaymentFeeExpense()  {return merchant_repayment_fee_expense;}
int DueForSettlement::getMerchantNotesPayable()  {return merchant_notes_payable;}
int DueForSettlement::getCollectionOfCashInTransit()  {return collection_of_cash_in_transit;}
int DueForSettlement::getCollectionOfCommissionIncome()  {return collection_of_commission_income;}
int DueForSettlement::getCollectionOfUpfrontFees()  {return collection_of_upfront_fees;}
int DueForSettlement::getCollectionOfTransactionInvestigationFee()  {return collection_of_transaction_investigation_fee;}


string generateExtraField(string _account_name, string _start_fiscal_year,string _closing_day)
{
    return "(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = '"+ _account_name + "'\
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
    AND loan_id = loan_app_loan.id and le.entry_date between '" + _start_fiscal_year + "' and '" + _closing_day + "')";
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

    //1- "Accrued expenses, merchants, commissions"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accrued expenses, merchants, commissions", _start_fiscal_year, _closure_date_string),"merchant_commission_expense");
    //2- "Accounts payable, merchandise, merchants"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accounts payable, merchandise, merchants", _start_fiscal_year, _closure_date_string),"merchant_merchandise_value");
    //3- "Accrued expenses, merchants, cashiers commissions"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accrued expenses, merchants, cashiers commissions", _start_fiscal_year, _closure_date_string),"cashier_commission_expense");
    //4- "Accrued expenses, merchants, deferred commissions, Due"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accrued expenses, merchants, deferred commissions, Due", _start_fiscal_year, _closure_date_string),"merchant_deferred_commission_expense");
    //5- "Accrued expenses, merchants, repayment fees"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accrued expenses, merchants, repayment fees", _start_fiscal_year, _closure_date_string),"merchant_repayment_fee_expense");
    //6- "Due current notes payable, merchants"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Due current notes payable, merchants", _start_fiscal_year, _closure_date_string),"merchant_notes_payable");
    //7- "Cash in transit, merchants"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Cash in transit, merchants", _start_fiscal_year, _closure_date_string),"collection_of_cash_in_transit");
    //8- "Accounts receivable, merchants, commission income"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accounts receivable, merchants, commission income", _start_fiscal_year, _closure_date_string),"collection_of_commission_income");
    //9- "Accounts receivable, merchant, transaction upfront fee"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accounts receivable, merchant, transaction upfront fee", _start_fiscal_year, _closure_date_string),"collection_of_upfront_fees");
    //10- "Accounts receivable, merchant, transaction investigation fee"
    dueForSettlementIterator->addExtraFromField(generateExtraField("Accounts receivable, merchant, transaction investigation fee", _start_fiscal_year, _closure_date_string),"collection_of_transaction_investigation_fee");

    return dueForSettlementIterator;
}
