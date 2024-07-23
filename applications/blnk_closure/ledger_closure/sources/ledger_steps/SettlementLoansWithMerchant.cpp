#include <SettlementLoansWithMerchant.h>

void SettlementLoansWithMerchant::stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *> *> *ledger_amount_orms)
{
}

void SettlementLoansWithMerchant::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    
    // ledgerClosureService->addHandler("Settlement loans with merchant", SettlementLoansWithMerchant::_get_installment_insterest);
    // ledgerClosureService->addHandler("Loan principal becoming overdue", SettlementLoansWithMerchant::_get_installment_principal);
    // ledgerClosureService->addHandler("Late repayment fee income accrual",SettlementLoansWithMerchant::_calc_installment_late_fees);
}

PSQLJoinQueryIterator *SettlementLoansWithMerchant::singleAggregator(string _closure_date_string)
{
        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
        {
            new settlement_dashboard_settlementrequest_primitive_orm("main"), 
            new settlement_dashboard_merchantpaymentrequest_primitive_orm("main"), 
            new loan_app_loan_primitive_orm("main"), 
            new crm_app_purchase_primitive_orm("main"), 

        },
        {

            {{{"jsonb_array_elements(settlement_dashboard_merchantpaymentrequest.transactions->'transactions')","tr"},{"",""}},JOIN_TYPE::cross},


            {{{"settlement_dashboard_merchantpaymentrequest","id"},{"settlement_dashboard_settlementrequest","request_id"}},JOIN_TYPE::full},

            {{{"<<expression>>","(tr->>'loan_id')::int"},{"loan_app_loan","id"}},JOIN_TYPE::full},
            {{{"loan_app_loan","id"},{"crm_app_purchase","loan_id"}},JOIN_TYPE::full}


            // {{"settlement_dashboard_settlementrequest", "request_id"}, {"settlement_dashboard_merchantpaymentrequest", "id"}}, 
            // {{"ledger_entry", "id"}, {"ledger_amount", "entry_id"}}
        });

    return psqlQueryJoin;
}

PSQLJoinQueryIterator *SettlementLoansWithMerchant::paymentRequestAggregator(string _closure_date_string)
{   
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
        {
            new settlement_dashboard_settlementrequest_primitive_orm("main"), 
            new settlement_dashboard_merchantpaymentrequest_primitive_orm("main"), 
            new ledger_entry_primitive_orm("main"), 
            new ledger_amount_primitive_orm("main"), 

        },
        {
            {{"settlement_dashboard_settlementrequest", "request_id"}, {"settlement_dashboard_merchantpaymentrequest", "id"}}, 
            {{"ledger_entry", "id"}, {"ledger_amount", "entry_id"}}
        });

    psqlQueryJoin->filter(
            ANDOperator(
                new UnaryOperator("settlement_dashboard_settlementrequest.status", in, "1, 3"),
                new UnaryOperator("settlement_dashboard_merchantpaymentrequest.status", eq, 2),

                new OROperator(
                    new ANDOperator(
                        new UnaryOperator("ledger_amount.merchant_payment_request_id",eq,"settlement_dashboard_merchantpaymentrequest.id", true),
                        new UnaryOperator("ledger_entry.template_id",eq, 63)
                    ),

                    new ANDOperator(
                        new UnaryOperator("ledger_amount.merchant_payment_request_id",eq,"settlement_dashboard_merchantpaymentrequest.id", true),
                        new UnaryOperator("ledger_entry.template_id",eq, 65)
                    ),

                    new ANDOperator(
                        new UnaryOperator("ledger_amount.merchant_payment_request_id",eq,"settlement_dashboard_merchantpaymentrequest.id", true),
                        new UnaryOperator("ledger_entry.template_id",eq, 169)
                    ),

                    new ANDOperator(
                        new UnaryOperator("ledger_amount.merchant_payment_request_id",eq,"settlement_dashboard_merchantpaymentrequest.id", true),
                        new UnaryOperator("ledger_entry.template_id",eq, 64)
                    )
                )

                ));
            
    // psqlQueryJoin->setDistinct({{"ledger_entry", "id"}});
    psqlQueryJoin->setAggregates(
        {{"ledger_entry", {"id", 1}},
    });

    psqlQueryJoin->setOrderBy("settlement_dashboard_settlementrequest.id asc ");

    return psqlQueryJoin;
}


PSQLJoinQueryIterator *SettlementLoansWithMerchant::loanAggregator(string _closure_date_string, set<int>* loan_ids)
{
    std::ostringstream oss;
    for (auto it=loan_ids->begin(); it!=loan_ids->end(); it++) {
        oss << *it;
        if (next(it,1) != loan_ids->end()) {
            oss<<", ";
        }
    }

    string loan_ids_string = oss.str();

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
        {
            new settlement_dashboard_settlementrequest_primitive_orm("main"), 
            new settlement_dashboard_merchantpaymentrequest_primitive_orm("main"),
            new loan_app_loan_primitive_orm("main"),
            new crm_app_purchase_primitive_orm("main")
        },
        {
            {{"settlement_dashboard_settlementrequest", "request_id"}, {"settlement_dashboard_merchantpaymentrequest", "id"}}, 

        });

    psqlQueryJoin->filter(
        ANDOperator(
            new UnaryOperator("settlement_dashboard_settlementrequest.status_id", eq, 1),
            new UnaryOperator("settlement_dashboard_settlementrequest.link", ne, 0),
            new UnaryOperator("loan_app_loan.id" , in ,loan_ids_string),
            new UnaryOperator("crm_app_purchase.loan_id" , in ,loan_ids_string)

        ));

    

    psqlQueryJoin->setAggregates(
        {{"settlement_dashboard_settlementrequest", {"id", 1}},
    });

    psqlQueryJoin->setOrderBy("settlement_dashboard_settlementrequest.id asc ");

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
    // lg->setInstallmentId(lai_orm->get_id());
    lg->setReversalBool(true);
    return lg;
}

LedgerAmount *SettlementLoansWithMerchant::_get_request_amount(LedgerClosureStep *settlementLoansWithMerchant)
{
    LedgerAmount * ledgerAmount = ((SettlementLoansWithMerchant*) settlementLoansWithMerchant)->_init_ledger_amount();
    loan_app_loan_primitive_orm * lal_orm = ((SettlementLoansWithMerchant*) settlementLoansWithMerchant)->get_loan();
    settlement_dashboard_settlementrequest_primitive_orm * sds_orm = ((SettlementLoansWithMerchant*) settlementLoansWithMerchant)->get_settlement_dashboard_settlementrequest();
    int request_type = sds_orm->get_type();
    
    //getting variables from loan table
    double principle = lal_orm->get_principle();
    double cashier_fee = lal_orm->get_cashier_fee();
    // double merchant_commission = lal_orm->get_merchant_commission();
    double loan_upfront_fee = lal_orm->get_loan_upfront_fee();
    double blnk_to_merchant_fee = lal_orm->get_bl_t_mer_fee();
    double merchant_to_blnk_fee = lal_orm->get_bl_t_mer_fee();
    
    //calculate values from the loan
    double cashier_commision = (principle*cashier_fee)/100;
    cashier_commision = ROUND(cashier_commision);
    double merchant_commission = (principle*blnk_to_merchant_fee)/100;
    merchant_commission = ROUND(merchant_commission);
    double upfront_fee = (loan_upfront_fee/100)*principle;
    upfront_fee = ROUND(upfront_fee);
    double rebate_commission = (principle*merchant_to_blnk_fee)/100;
    rebate_commission = ROUND(rebate_commission);
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

SettlementLoansWithMerchant::SettlementLoansWithMerchant(settlement_dashboard_settlementrequest_primitive_orm *_sds_orm, settlement_dashboard_merchantpaymentrequest_primitive_orm *_sdm_orm, loan_app_loan_primitive_orm *_lal_orm)
{
    sds_orm = _sds_orm;
    sdm_orm = _sdm_orm;
    lal_orm = _lal_orm;
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
settlement_dashboard_merchantpaymentrequest_primitive_orm* SettlementLoansWithMerchant::get_settlement_dashboard_merchantpaymentrequest()
{
    return sdm_orm;
}

settlement_dashboard_settlementrequest_primitive_orm* SettlementLoansWithMerchant::get_settlement_dashboard_settlementrequest()
{
    return sds_orm;
}

loan_app_loan_primitive_orm* SettlementLoansWithMerchant::get_loan()
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