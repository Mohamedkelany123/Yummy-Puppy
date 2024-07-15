#include <SettlementLoansWithMerchant.h>

void SettlementLoansWithMerchant::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    
    ledgerClosureService->addHandler("Settlement loans with merchant", SettlementLoansWithMerchant::_get_installment_insterest);
    ledgerClosureService->addHandler("Loan principal becoming overdue", DueToOverdue::_get_installment_principal);
    ledgerClosureService->addHandler("Late repayment fee income accrual",DueToOverdue::_calc_installment_late_fees);
}

PSQLJoinQueryIterator *SettlementLoansWithMerchant::aggregator(string _closure_date_string)
{

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
        {
            new settlement_dashboard_settlementrequest_primitive_orm("main"), 
            new settlement_dashboard_merchantpaymentrequest_primitive_orm("main"), 

        },
        {
            {{"settlement_dashboard_settlementrequest", "request_id"}, {"settlement_dashboard_merchantpaymentrequest", "id"}}, 
        });

psqlQueryJoin->filter(
        ANDOperator(
            new UnaryOperator("settlement_dashboard_settlementrequest.status_id", in, "1, 3"),
            ));

psqlQueryJoin->setAggregates(
    {{"settlement_dashboard_settlementrequest", {"id", 1}},
});

psqlQueryJoin->setOrderBy("settlement_dashboard_settlementrequest.id asc ");

return psqlQueryJoin;
}
// void SettlementLoansWithMerchant::update_step()
// {
//     PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
//         ANDOperator(
//             new UnaryOperator ("loan_app_loan.id",ne,"14312"),
//             new UnaryOperator ("loan_app_loan.closure_status",gte,0)
//         ),
//         {{"closure_status",to_string(ledger_status::MARGINALIZE_INCOME)}}

//         );
//     psqlUpdateQuery.update(); 
// }
LedgerAmount*  SettlementLoansWithMerchant::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();

    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setInstallmentId(lai_orm->get_id());
    lg->setReversalBool(true);
    return lg;
}

LedgerAmount *SettlementLoansWithMerchant::_get_request_amount(LedgerClosureStep *settlementLoansWithMerchant)
{
    LedgerAmount * ledgerAmount = ((SettlementLoansWithMerchant*) settlementLoansWithMerchant)->_init_ledger_amount();
    loan_app_loan_primitive_orm * lal_orm = ((SettlementLoansWithMerchant*) settlementLoansWithMerchant)->get_loan();
    settlement_dashboard_settlementrequest_primitive_orm * sds_orm = ((SettlementLoansWithMerchant*) settlementLoansWithMerchant)->get_settlement_dashboard_settlementrequest();
    int request_type = sdm_orm->get_type();
    
    //getting variables from loan table
    double principle = lal_orm->get_principle();
    double cashier_commision = lal_orm->get_cashier_commision();
    // double merchant_commission = lal_orm->get_merchant_commission();
    double loan_upfront_fee = lal_orm->get_loan_upfront_fee();
    double cashier_fee = lal_orm->get_cashier_fee();
    double blnk_to_merchant_fee = lal_orm->get_bl_t_mer_fee();
    double merchant_to_blnk_fee = lal_orm->get_bl_t_mer_fee();
    
    //calculate values from the loan
    double cashier_commision = ROUND((principle*cashier_fee)/100);
    double merchant_commission = ROUND((principle*blnk_to_merchant_fee)/100);
    double upfront_fee = ROUND((loan_upfront_fee/100)*principle);
    double rebate_commission = ROUND((principle*merchant_to_blnk_fee)/100);

    double amount = 0;
    switch (request_type)
    {
    case 0:
        amount = ROUND(principle);
        break;
    case 1:
        amount = cashier_commision;
        break;
    case 2:
        amount = merchant_commission ;
        break;
    case 3:
        amount = -upfront_fee;
        break;
    case 4:
        amount = -rebate_commission;
        break;
    }

    //set and return amount
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}

SettlementLoansWithMerchant::SettlementLoansWithMerchant()
{
}

SettlementLoansWithMerchant::SettlementLoansWithMerchant(settlement_dashboard_settlementrequest_primitive_orm *_sds_orm, settlement_dashboard_merchantpaymentrequest_primitive_orm *_sdm_orm)
{
    sds_orm = _sds_orm;
    sdm_orm = _sdm_orm;
    
}

void SettlementLoansWithMerchant::set_settlement_dashboard_settlementrequest(settlement_dashboard_settlementrequest_primitive_orm *_sds_orm)
{
    sds_orm = _sds_orm;
}

void SettlementLoansWithMerchant::set_settlement_dashboard_merchantpaymentrequest(settlement_dashboard_merchantpaymentrequest_primitive_orm *_sdm_orm)
{
    sdm_orm = _sdm_orm;
}

void SettlementLoansWithMerchant::set_template_id(int _template_id)
{
    template_id = _template_id;
}

void SettlementLoansWithMerchant::set_closing_day(BDate _closing_day)
{
    closing_day = _closing_day;
}
settlement_dashboard_merchantpaymentrequest_primitive_orm *SettlementLoansWithMerchant::get_settlement_dashboard_merchantpaymentrequest()
{
    return sdm_orm;
}

settlement_dashboard_settlementrequest_primitive_orm *SettlementLoansWithMerchant::get_settlement_dashboard_settlementrequest()
{
    return sds_orm;
}

loan_app_loan_prmitive_orm *SettlementLoansWithMerchant::get_loan()
{
    return lal_orm;
}

int SettlementLoansWithMerchant::get_template_id()
{
    return template_id;
}

BDate SettlementLoansWithMerchant::get_closing_day()
{
    return closing_day;
}