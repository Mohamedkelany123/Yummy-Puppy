#include <CustomerPayment.h>

CustomerPayment::CustomerPayment(loan_app_loan_primitive_orm *_lal_orm, payments_loanorder_primitive_orm *_plo_orm, payments_loanorderheader_primitive_orm *_ploh_orm, BDate _closing_day, int _template_id, int _first_loan_order_id)
{
    lal_orm = _lal_orm;
    plo_orm = _plo_orm;
    ploh_orm = _ploh_orm;
    closing_day = _closing_day;
    template_id = _template_id;
    first_loan_order_id = _first_loan_order_id;
}

PSQLJoinQueryIterator *CustomerPayment::aggregator(string _closure_date_string)
{
    PSQLJoinQueryIterator* psqlJoinQueryIterator = new PSQLJoinQueryIterator(
        "main",
        {new payments_loanorder_primitive_orm("main"), new loan_app_loan_primitive_orm("main"), new payments_loanorderheader_primitive_orm("main"), new payments_paymentmethod_primitive_orm("main"), new payments_paymentprovider_primitive_orm("main")},
        {{{"payments_loanorder", "loan_id"}, {"loan_app_loan", "id"}}, {{"payments_loanorderheader", "id"}, {"payments_loanorder", "loan_order_header_id"}}, {{"payments_loanorder", "payment_method_id"}, {"payments_paymentmethod", "id"}}, {{"payments_paymentmethod", "provider_id"}, {"payments_paymentprovider", "id"}}}
    );
    psqlJoinQueryIterator->filter(
        ANDOperator (
            new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnull, "", true),
            new UnaryOperator("payments_loanorder.paid_at::date", lte, _closure_date_string),
            // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::REPAYMENT_BY_CUSTOMER-1)
            new UnaryOperator("payments_loanorder.status", nin, "2, 3"),
            new UnaryOperator("payments_loanorder.payment_method_id", ne, 34),
            new UnaryOperator("loan_app_loan.id", ne, 14312)
        )
    );

    psqlJoinQueryIterator->addExtraFromField(
        "(select id from payments_loanorder plo where plo.loan_order_header_id=payments_loanorder.loan_order_header_id order by id asc limit 1)",
        "first_loan_order_id"
    );
    // psqlJoinQueryIterator->setAggregates({
    //     {"payments_loanorder", {"loan_order_header_id", 1}}
    // });
    return psqlJoinQueryIterator;
}

loan_app_loan_primitive_orm *CustomerPayment::get_loan_app_loan()
{
    return lal_orm;
}

payments_loanorder_primitive_orm *CustomerPayment::get_payments_loanorder()
{
    return plo_orm;
}

payments_loanorderheader_primitive_orm *CustomerPayment::get_payments_loanorderheader()
{
    return ploh_orm;
}

int CustomerPayment::get_first_loan_order_id()
{
    return first_loan_order_id;
}

LedgerAmount *CustomerPayment::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    // lg->setCashierId(lal_orm->get_cashier_id());
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    // lg->setMerchantId(lal_orm->get_merchant_id());
    return lg;
}

void CustomerPayment::setupLedgerClosureService(LedgerClosureService * ledgerClosureService)
{
    if (template_id == 18) {
        ledgerClosureService->addHandler("Repayment fee Income, Accept", _get_order_income);
    }
    if (template_id == 19 || template_id == 119 || template_id == 133) {
        ledgerClosureService->addHandler("Repayment fee Income, Fawry", _get_order_income);
    }
    if (template_id == 44) {
        ledgerClosureService->addHandler("Repayment fee Income, Blnk", _get_order_income);
    }
    if (template_id == 165 || template_id == 53) {
        ledgerClosureService->addHandler("Repayment fee Income, Collection", _get_order_income);
    }
    ledgerClosureService->addHandler("Repayment net of repayment fee income", _get_order_total_amount);
    ledgerClosureService->addHandler("Add remaining of payment in Escorw Account", _get_remaining_escrow);
}

void CustomerPayment::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
    ANDOperator(
        new UnaryOperator ("loan_app_loan.id",ne,"14312"),
        new UnaryOperator ("loan_app_loan.closure_status",gte,0)
    ),
    {{"closure_status",to_string(ledger_status::REPAYMENT_BY_CUSTOMER)}}
    );
    psqlUpdateQuery.update();   
}

LedgerAmount *CustomerPayment::_get_order_income(LedgerClosureStep* customerPaymentStep)
{
    LedgerAmount* ledgerAmount = ((CustomerPayment*)customerPaymentStep)->_init_ledger_amount();
    int first_loan_order_id = ((CustomerPayment*)customerPaymentStep)->get_first_loan_order_id();
    payments_loanorder_primitive_orm* plo_orm = ((CustomerPayment*)customerPaymentStep)->get_payments_loanorder();
    payments_loanorderheader_primitive_orm* ploh_orm = ((CustomerPayment*)customerPaymentStep)->get_payments_loanorderheader();
    int order_id = plo_orm->get_id();
    if (order_id == first_loan_order_id) {
        double amount = ROUND(ploh_orm->get_income_fee() / 100);
        ledgerAmount->setAmount(amount);
    }
    return ledgerAmount;
}

LedgerAmount *CustomerPayment::_get_order_total_amount(LedgerClosureStep *customerPaymentStep)
{
    LedgerAmount* ledgerAmount = ((CustomerPayment*)customerPaymentStep)->_init_ledger_amount();
    int first_loan_order_id = ((CustomerPayment*)customerPaymentStep)->get_first_loan_order_id();
    payments_loanorder_primitive_orm* plo_orm = ((CustomerPayment*)customerPaymentStep)->get_payments_loanorder();
    payments_loanorderheader_primitive_orm* ploh_orm = ((CustomerPayment*)customerPaymentStep)->get_payments_loanorderheader();
    double amount = ROUND(plo_orm->get_amount() / 100);
    int order_id = plo_orm->get_id();
    if (order_id == first_loan_order_id) {
        double faulty_adjustment_amount = ploh_orm->get_faulty_adjustment_amount();
        if (faulty_adjustment_amount < 0) {
            ledgerAmount->setAmount(amount);
        }
        else {
            ledgerAmount->setAmount(amount + ROUND(faulty_adjustment_amount / 100));
        }
    }
    else {
        ledgerAmount->setAmount(amount);
    }
    return ledgerAmount;
}

LedgerAmount *CustomerPayment::_get_remaining_escrow(LedgerClosureStep *customerPaymentStep)
{
    LedgerAmount* ledgerAmount = ((CustomerPayment*)customerPaymentStep)->_init_ledger_amount();
    int first_loan_order_id = ((CustomerPayment*)customerPaymentStep)->get_first_loan_order_id();
    payments_loanorder_primitive_orm* plo_orm = ((CustomerPayment*)customerPaymentStep)->get_payments_loanorder();
    payments_loanorderheader_primitive_orm* ploh_orm = ((CustomerPayment*)customerPaymentStep)->get_payments_loanorderheader();
    int order_id = plo_orm->get_id();
    if (order_id == first_loan_order_id) {
        double amount = ROUND(ploh_orm->get_adjustment_amount() / 100);
        ledgerAmount->setAmount(amount);
    }
    return ledgerAmount;
}

void CustomerPayment::stampORMS(ledger_entry_primitive_orm *_entry)
{
    plo_orm->setUpdateRefernce("payment_ledger_entry_id", _entry);
}
