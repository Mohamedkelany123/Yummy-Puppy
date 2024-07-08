#include <CancelLoans.h>


CancelLoan::CancelLoan(loan_app_loan_primitive_orm * _lal_orm):LedgerClosureStep ()
{
    lal_orm = _lal_orm;    
    template_id = 5;
}


void CancelLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Booking the transaction upfront fee", CancelLoan::_get_upfront_fee);
}

CancelLoan::~CancelLoan(){}


LedgerAmount*  CancelLoan::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCashierId(lal_orm->get_cashier_id());
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}




loan_app_loan_primitive_orm* CancelLoan::get_loan_app_loan()  {
    return lal_orm;
}

void CancelLoan::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}



int CancelLoan::get_template_id()  {
    return template_id;
}



void CancelLoan::set_template_id(int _template_id)
{
    template_id = _template_id;
}





LedgerAmount * CancelLoan::_get_upfront_fee(LedgerClosureStep *cancelLoan)
{
    loan_app_loan_primitive_orm* loan_orm = ((CancelLoan*)cancelLoan)->get_loan_app_loan();
    double amount = ROUND(loan_orm->get_loan_upfront_fee() * !(loan_orm->get_refund_upfront_fee_bool()));
    LedgerAmount * ledgerAmount = ((CancelLoan*)cancelLoan)->_init_ledger_amount();
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}


PSQLJoinQueryIterator* CancelLoan::aggregator(string _closure_date_string, int _agg_number){
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_bl_orm("main"), new ledger_amount_primitive_orm("main")},
        {{{"loan_app_loan","id"},{"ledger_amount","loan_id"}}});

        psqlQueryJoin->addExtraFromField("(select count(*)>0 from loan_app_loanstatushistroy lal where lal.status_id in (12,13) and lal.day::date <= \'"+ _closure_date_string +"\' and lal.loan_id = loan_app_loan.id)","is_included");
        psqlQueryJoin->addExtraFromField("(select distinct lal.day from loan_app_loanstatushistroy lal where lal.status_id in (12,13) and lal.loan_id = loan_app_loan.id)","cancellation_day");
        psqlQueryJoin->filter(
            ANDOperator 
            (
                // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::CANCEL_LOAN-1)),
                new UnaryOperator ("loan_app_loan.id" , ne, "14312"),

                new UnaryOperator ("loan_app_loan.cancel_ledger_entry_id",isnull,"",true),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,_closure_date_string),
                new UnaryOperator ("loan_app_loan.status_id",in,"12,13")

            )
        );

        psqlQueryJoin->setOrderBy("loan_app_loan.id asc, ledger_amount.id asc");
        psqlQueryJoin->setAggregates ({
            {"loan_app_loan", {"id", 1}},  
        });

        return psqlQueryJoin;
}
void CancelLoan::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator(
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.closure_status",gte,0)
            ),
            {{"closure_status",to_string(ledger_status::CANCEL_LOAN)}}

            );
        psqlUpdateQuery.update();   
}