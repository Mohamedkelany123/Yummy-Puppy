#include <SettlementLoansWithMerchant.h>

void SettlementLoansWithMerchant::stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *> *> *ledger_amount_orms)
{
}

void SettlementLoansWithMerchant::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    if (!is_reverse) {
        ledgerClosureService->addHandler("Settlement loans with merchant", SettlementLoansWithMerchant::get_amount);
    }
    else {
        ledgerClosureService->addHandler("Reverse settlement loans with merchant", SettlementLoansWithMerchant::get_amount);
    }
}


LedgerAmount *SettlementLoansWithMerchant::get_amount(LedgerClosureStep *settlementLoansWithMerchant)
{
    LedgerAmount* ledgerAmount = new LedgerAmount();
    int loan_id = ((SettlementLoansWithMerchant*)settlementLoansWithMerchant)->loan_id;
    double amount = ((SettlementLoansWithMerchant*)settlementLoansWithMerchant)->amount;
    int merchant_id = ((SettlementLoansWithMerchant*)settlementLoansWithMerchant)->merchant_id;
    int merchant_payment_request_id = ((SettlementLoansWithMerchant*)settlementLoansWithMerchant)->payment_request_id;
    int customer_id = ((SettlementLoansWithMerchant*)settlementLoansWithMerchant)->customer_id;
    ledgerAmount->setLoanId(loan_id);
    ledgerAmount->setAmount(ROUND(amount));
    ledgerAmount->setMerchantId(merchant_id);
    ledgerAmount->setMerchantPaymentRequestId(merchant_payment_request_id);
    ledgerAmount->setCustomerId(customer_id);
    return ledgerAmount;
}



string calculateAmountSum(string _account_name, string loan_id, string _start_fiscal_year,string _closing_day)
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
    AND loan_id = "  + loan_id + " and le.entry_date between '" + _start_fiscal_year + "' and '" + _closing_day + "')";
}

PSQLJoinQueryIterator *SettlementLoansWithMerchant::aggregator(string _closure_date_string, string start_fiscal_year)
{
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
    {
        new settlement_dashboard_merchantpaymentrequest_primitive_orm("main"), 
        new settlement_dashboard_settlementrequest_primitive_orm("main"), 
        new loan_app_loan_primitive_orm("main"), 
        new crm_app_purchase_primitive_orm("main"), 

    },
    {
        {{{"jsonb_array_elements(settlement_dashboard_merchantpaymentrequest.transactions->'transactions')","tr"},{"",""}},JOIN_TYPE::cross},
        {{{"settlement_dashboard_merchantpaymentrequest","id"},{"settlement_dashboard_settlementrequest","request_id"}},JOIN_TYPE::full},
        {{{"<<expression>>","(tr->>'loan_id')::int"},{"loan_app_loan","id"}},JOIN_TYPE::full},
        {{{"loan_app_loan","id"},{"crm_app_purchase","loan_id"}},JOIN_TYPE::full}
    });

    psqlQueryJoin->addExtraFromField("(select lal2.id from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_loan_id");
    psqlQueryJoin->addExtraFromField("(select id from crm_app_purchase cap2 where crm_app_purchase.parent_purchase_id = cap2.id  limit 1)", "parent_purchase_id");
    psqlQueryJoin->addExtraFromField("(select lal2.principle from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_principle");
    psqlQueryJoin->addExtraFromField("(select lal2.cashier_fee from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_cashier_fee");
    psqlQueryJoin->addExtraFromField("(select lal2.mer_t_bl_fee from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_mer_t_bl_fee");
    psqlQueryJoin->addExtraFromField("(select lal2.bl_t_mer_fee from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_bl_t_mer_fee");
    psqlQueryJoin->addExtraFromField("(select lal2.loan_upfront_fee from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_loan_upfront_fee");
    psqlQueryJoin->addExtraFromField("(select lal2.settled_pay_id from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_settle_pay_id");
    psqlQueryJoin->addExtraFromField("(select lal2.merchant_id from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", "parent_merchant_id");
    psqlQueryJoin->addExtraFromField("case when tr->>'check_bool'='true' then true else false end", "check_bool");
    psqlQueryJoin->addExtraFromField("case when tr->>'can_settle_bool'='true' then true else false end", "can_settle_bool");
    psqlQueryJoin->addExtraFromField("tr->>'type'", "type");


    psqlQueryJoin->addExtraFromField(calculateAmountSum("Due For Settlement, merchants", "loan_app_loan.id", start_fiscal_year, _closure_date_string), "balance_106");
    psqlQueryJoin->addExtraFromField(calculateAmountSum("Due For Settlement, merchants contra", "loan_app_loan.id", start_fiscal_year, _closure_date_string), "balance_179");

    psqlQueryJoin->addExtraFromField(calculateAmountSum("Due For Settlement, merchants", "(select lal2.id from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", start_fiscal_year, _closure_date_string), "parent_balance_106");
    psqlQueryJoin->addExtraFromField(calculateAmountSum("Due For Settlement, merchants contra", "(select lal2.id from crm_app_purchase cap2,loan_app_loan lal2 where crm_app_purchase.parent_purchase_id = cap2.id and cap2.loan_id=lal2.id limit 1)", start_fiscal_year, _closure_date_string), "parent_balance_179");

    psqlQueryJoin->filter(
        ANDOperator(
            new UnaryOperator("settlement_dashboard_merchantpaymentrequest.status", eq, 0),
            new UnaryOperator("settlement_dashboard_settlementrequest.status", eq, 1),
            new UnaryOperator("settlement_dashboard_settlementrequest.link", ne, 0)
        )
    );

    psqlQueryJoin->setAggregates({
        {"settlement_dashboard_merchantpaymentrequest", {"id", 1}}
    });
    return psqlQueryJoin;
}

void SettlementLoansWithMerchant::update_step()
{
}

SettlementLoansWithMerchant::~SettlementLoansWithMerchant()
{
}

void SettlementLoansWithMerchant::unstampLoans()
{   
    PSQLUpdateQuery psqlUpdateQuerySettledPay ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator(
                "loan_app_loan.id",
                in,
                "(select lal.id from loan_app_loan lal join settlement_dashboard_merchantpaymentrequest sdm on lal.settled_pay_id = sdm.id join settlement_dashboard_settlementrequest sds on sds.request_id = sdm.id where sds.status in(1,3))"
            )
        ),
        {{"settled_pay_id", "null"}}
    );
    psqlUpdateQuerySettledPay.update();   


    PSQLUpdateQuery psqlUpdateQuerySettledCancel ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator(
                "loan_app_loan.id",
                in,
                "(select lal.id from loan_app_loan lal join settlement_dashboard_merchantpaymentrequest sdm on lal.settled_cancel_id = sdm.id join settlement_dashboard_settlementrequest sds on sds.request_id = sdm.id where sds.status in(1,3))"
            )
        ),
        {{"settled_cancel_id", "null"}}
    );
    psqlUpdateQuerySettledCancel.update();   
    
}


LedgerAmount*  SettlementLoansWithMerchant::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(customer_id);
    lg->setLoanId(loan_id);
    lg->setMerchantId(merchant_id);
    lg->setMerchantPaymentRequestId(payment_request_id);
    lg->setReversalBool(false);
    return lg;
}




LedgerAmount *SettlementLoansWithMerchant::_settle_with_merchant(LedgerClosureStep *settlementLoansWithMerchant)
{
    return nullptr;
}

bool SettlementLoansWithMerchant::checkAmounts()
{
    return false;
}

SettlementLoansWithMerchant::SettlementLoansWithMerchant()
{
}

SettlementLoansWithMerchant::SettlementLoansWithMerchant(double _amount, int _loan_id, int _payment_request_id, int _merchant_id, int _customer_id, int _category, int _activation_user_id, bool _is_reverse)
{
    amount = _amount;
    loan_id = _loan_id;
    payment_request_id = _payment_request_id;
    merchant_id = _merchant_id;
    customer_id = _customer_id;
    category = _category;
    activation_user_id = _activation_user_id;
    is_reverse = _is_reverse;
}

