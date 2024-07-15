#include "SettlementByCustomer.h"

SettlementByCustomer::SettlementByCustomer()
{
        lal_orm = nullptr;
        // lai_orm = nullptr;
        // nli_orm = nullptr;
        // nlif_orm = nullptr;
        loan_app_loan_primitive_orm * lal_orm;
        payments_loanorder_primitive_orm* plo_orm;
        vector <loan_app_installment_primitive_orm*> installments;        
        map <loan_app_installment_primitive_orm*, new_lms_installmentextension_primitive_orm*> installments_extensions;        
        map <loan_app_installment_primitive_orm*, vector <new_lms_installmentlatefees_primitive_orm*>> late_fees_map;

}   

SettlementByCustomer::SettlementByCustomer(loan_app_loan_primitive_orm * _lal_orm, payments_loanorder_primitive_orm* _plo_orm,
            loan_app_installment_primitive_orm* _lai_orm,  new_lms_installmentextension_primitive_orm* _nli_orm,
                vector <new_lms_installmentlatefees_primitive_orm*>* _lf_orms, int* _unmarginalization_template_id, BDate _closing_day, BDate* _settlement_day, int _last_status) 
{
    lal_orm = _lal_orm;
    plo_orm = _plo_orm;
    lai_orm = _lai_orm;
    nli_orm = _nli_orm;
    lf_orms = _lf_orms;
    unmarginalization_template_id = _unmarginalization_template_id;
    closing_day = _closing_day;
    settlement_day = _settlement_day;
    last_status = _last_status;
}

PSQLJoinQueryIterator* SettlementByCustomer::aggregator(string _closure_date_string){
    PSQLJoinQueryIterator * ordersQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"), new payments_loanorder_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"new_lms_installmentlatefees", "installment_extension_id"}, {"new_lms_installmentextension", "installment_ptr_id"}}, {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}});

        ordersQuery->filter(
            ANDOperator(
                new OROperator (
                    //PRINCIPAL
                    new ANDOperator (
                        new UnaryOperator("new_lms_installmentextension.is_principal_paid", eq, true),
                        new UnaryOperator("new_lms_installmentextension.principal_payment_ledger_amount_id", isnull, "", true),
                        new UnaryOperator("new_lms_installmentextension.principal_paid_at::date", lte, _closure_date_string),         
                        new UnaryOperator("payments_loanorder.id", eq, "new_lms_installmentextension.principal_order_id",true)  
                    ),
                    //INTEREST
                    new ANDOperator (
                        new UnaryOperator("new_lms_installmentextension.is_interest_paid", eq, true),
                        new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
                        new UnaryOperator("new_lms_installmentextension.interest_payment_ledger_amount_id", isnull, "", true),
                        new UnaryOperator("new_lms_installmentextension.interest_paid_at::date", lte, _closure_date_string),
                        new UnaryOperator("payments_loanorder.id", eq, "new_lms_installmentextension.interest_order_id",true)
                    ),
                    //EARLYFEES
                    new ANDOperator (
                        new UnaryOperator("new_lms_installmentextension.is_early_paid", eq, true),
                        new UnaryOperator("new_lms_installmentextension.early_fee_payment_ledger_amount_id", isnull, "", true),
                        new UnaryOperator("new_lms_installmentextension.early_paid_at::date", lte, _closure_date_string),
                        new UnaryOperator("payments_loanorder.id", eq, "new_lms_installmentextension.early_order_id",true)      
                    ),
                    //LATEFEES
                    new ANDOperator (
                        new UnaryOperator("new_lms_installmentlatefees.is_paid", eq, true),
                        new UnaryOperator("new_lms_installmentlatefees.is_cancelled", eq, false),
                        new UnaryOperator("new_lms_installmentlatefees.payment_amount_id", isnull, "", true),
                        new UnaryOperator("new_lms_installmentlatefees.paid_at::date", lte, _closure_date_string),
                        new UnaryOperator("payments_loanorder.id", eq, "new_lms_installmentlatefees.order_id",true)         
                    ),
                    //EXTRAINTEREST
                    new ANDOperator (
                        new UnaryOperator("new_lms_installmentextension.is_extra_interest_paid", eq, true),
                        new UnaryOperator("new_lms_installmentextension.extra_interest_payment_ledger_amount_id", isnull, "", true),
                        new UnaryOperator("new_lms_installmentextension.extra_interest_paid_at::date", lte, _closure_date_string),
                        new UnaryOperator("payments_loanorder.id", eq, "new_lms_installmentextension.extra_interest_order_id",true)  
                    )
                ),
                new ANDOperator(
                    // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_BY_CUSTOMER-1),
                    new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnotnull,"",true),
                    new UnaryOperator ("loan_app_loan.id" , ne, "14312")
                )
            )
        );
        ordersQuery->addExtraFromField("(select template_id from ledger_entry le where id = (select entry_id from ledger_amount where id = new_lms_installmentextension.unmarginalization_ledger_amount_id))","unmarginalization_template");        
        ordersQuery->addExtraFromField("(select day from loan_app_loanstatushistroy where loan_app_loanstatushistroy.loan_id  = loan_app_loan.id and reversal_order_id is null and status_type = 0 and status_id in (8,16) order by id desc limit 1)","settlement_day");        
        ordersQuery->addExtraFromField("(select status_id from loan_app_loanstatushistroy where loan_app_loanstatushistroy.loan_id  = loan_app_loan.id and day <= '" + _closure_date_string + "' and status_type = 0 order by id desc limit 1)","last_status");        

        ordersQuery->setOrderBy("payments_loanorder.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.id");
        ordersQuery->setAggregates ({
            {"payments_loanorder", {"id", 1}},  
        });


        return ordersQuery;
}


void SettlementByCustomer::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("Reversing marginalized late repayment fee income, if applicable", _get_marginalized_late_fees);
    ledgerClosureService->addHandler("Reversing marginalized interest income, if applicable", _get_marginalized_interest);
    ledgerClosureService->addHandler("Reversal of late repayment fee income, if applicable", _skip_leg);
    ledgerClosureService->addHandler("Repayment of late repayment fee receivable, if applicable", _get_late_fees_paid);
    ledgerClosureService->addHandler("Repayment of accrued interest receivable, overdue, if applicable", _get_overdue_interest_paid);
    ledgerClosureService->addHandler("Repayment of loans receivable, overdue, if applicable", _get_overdue_principal_paid);
    ledgerClosureService->addHandler("Repayment of accrued interest receivable, (due), if applicable", _get_due_interest_paid);
    ledgerClosureService->addHandler("Repayment of loans receivable, (due)", _get_due_principal_paid);
    ledgerClosureService->addHandler("Repayment of accrued interest receivable, undue", _get_undue_interest_paid);
    ledgerClosureService->addHandler("Repayment of loans receivable, undue", _get_undue_principal_paid);
    ledgerClosureService->addHandler("Repayment of Long term loan receivable", _get_principal_long_term);
    ledgerClosureService->addHandler("Repayment of early repayment fee", _get_early_repayment_fee_income);
    ledgerClosureService->addHandler("Reversal of provisions balance", _skip_leg);
    ledgerClosureService->addHandler("Payment of merchant notes payable (short term)", _skip_leg);
    ledgerClosureService->addHandler("Payment of merchant notes payable (long term)", _skip_leg);
    ledgerClosureService->addHandler("Repayment of late repayment fee receivable, if applicable (securitization)", _get_late_fees_paid_sec);
    ledgerClosureService->addHandler("Repayment of accrued interest receivable, overdue, if applicable (securitization)", _get_overdue_interest_paid_sec);
    ledgerClosureService->addHandler("Repayment of loans receivable, overdue, if applicable (securitization)", _get_overdue_principal_paid_sec);
    ledgerClosureService->addHandler("Repayment of accrued interest receivable, (due), if applicable (securitization)", _get_due_interest_paid_sec);
    ledgerClosureService->addHandler("Repayment of loans receivable, (due) (securitization)", _get_due_principal_paid_sec);
    ledgerClosureService->addHandler("Repayment of accrued interest receivable, undue (securitization)", _get_undue_interest_paid_sec);
    ledgerClosureService->addHandler("Repayment of loans receivable, undue (securitization)", _get_undue_principal_paid_sec);
    ledgerClosureService->addHandler("Repayment of Long term loan receivable (securitization)", _get_principal_long_term_sec);
    ledgerClosureService->addHandler("Repayment of early repayment fee (securitization)", _get_early_repayment_fee_income_sec);
}

LedgerAmount *SettlementByCustomer::_skip_leg(LedgerClosureStep *settlementByCustomer){
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    la->setAmount(0);
    return la;
}

LedgerAmount * SettlementByCustomer::_init_ledger_amount(){}

// TODO 
LedgerAmount *SettlementByCustomer::_get_marginalized_late_fees(LedgerClosureStep *settlementByCustomer){
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    vector<new_lms_installmentlatefees_primitive_orm*>* nlif_orms = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentlatefees();
    // new_lms_installmentextension_primitive_orm* nli_orm = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentextension();
    float total_amount = 0.0;
    for(auto *nlif_orm : *nlif_orms){
        if (nlif_orm != nullptr){
            if (nlif_orm->get_marginalization_ledger_amount_id() != 0 && nlif_orm->get_unmarginalization_ledger_amount_id() == 0)
                total_amount += nlif_orm->get_amount();
        }
    }

    la->setAmount(ROUND(total_amount));
    return la;
}

LedgerAmount *SettlementByCustomer::_get_marginalized_interest(LedgerClosureStep *settlementByCustomer){
    // vector<loan_app_installment_primitive_orm*> lai_orms = ((SettlementByCustomer*)settlementByCustomer)->get_installments();
    // map <loan_app_installment_primitive_orm*, new_lms_installmentextension_primitive_orm*> nli_orms = ((SettlementByCustomer*)settlementByCustomer)->get_installments_extensions();
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    float amount = 0.0;
    
    new_lms_installmentextension_primitive_orm *nli_orm = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentextension();
    payments_loanorder_primitive_orm* plo_orm = ((SettlementByCustomer*)settlementByCustomer)->get_payments_loanorder();
    //TODO CHANGE TO BDATE
    if ((nli_orm->get_marginalization_ledger_amount_id() != 0) && nli_orm->get_is_interest_paid()
        && (nli_orm->get_marginalization_ledger_amount_id() == 0 || (nli_orm->get_unmarginalization_ledger_amount_id() != 0 
            && *((SettlementByCustomer*)settlementByCustomer)->get_unmarginalization_template_id() == 81))
                && (nli_orm->get_interest_paid_at() <= plo_orm->get_paid_at())){
                    amount = std::round((nli_orm->get_actual_interest_paid() + nli_orm->get_first_installment_interest_adjustment()) * 100.0) / 100.0;
    }

    else if((nli_orm->get_partial_marginalization_ledger_amount_id() != 0) && (nli_orm->get_marginalization_ledger_amount_id()==0) && (nli_orm->get_is_interest_paid()) 
        && (nli_orm->get_marginalization_ledger_amount_id() == 0 || (nli_orm->get_unmarginalization_ledger_amount_id() != 0 
            && *((SettlementByCustomer*)settlementByCustomer)->get_unmarginalization_template_id() == 81))
                && (nli_orm->get_interest_paid_at() <= plo_orm->get_paid_at())
                    && (nli_orm->get_partial_marginalization_date() <= nli_orm->get_interest_paid_at())){
                        amount = std::round(nli_orm->get_partial_accrual_amount() * 100.0) / 100.0;
    }

    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_late_fees_paid(LedgerClosureStep *settlementByCustomer)
{
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentextension();
    if (nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    float total_amount = 0.0;
    vector<new_lms_installmentlatefees_primitive_orm*>* nlif_orms = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentlatefees();
    for(auto *nlif_orm : *nlif_orms){
        if (nlif_orm != nullptr){
                total_amount += nlif_orm->get_amount();
        }
    }
    la->setAmount(ROUND(total_amount));
    return la;
}

LedgerAmount *SettlementByCustomer::_get_late_fees_paid_sec(LedgerClosureStep *settlementByCustomer)
{
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentextension();
    if (!nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    float total_amount = 0.0;
    vector<new_lms_installmentlatefees_primitive_orm*>* nlif_orms = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentlatefees();
    for(auto *nlif_orm : *nlif_orms){
        if (nlif_orm != nullptr){
                total_amount += nlif_orm->get_amount();
        }
    }
    la->setAmount(ROUND(total_amount));
    return la;
}

LedgerAmount *SettlementByCustomer::_get_due_principal_paid(LedgerClosureStep *settlementByCustomer)
{
    SettlementByCustomer* settlementByCustomerObject = settlementByCustomerObject;
    LedgerAmount* la = (settlementByCustomerObject)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = (settlementByCustomerObject)->get_new_lms_installmentextension();
    loan_app_loan_primitive_orm* lal_orm = (settlementByCustomerObject)->get_loan_app_loan();
    if (nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    loan_app_installment_primitive_orm* lai_orm =(settlementByCustomerObject)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = (settlementByCustomerObject)->get_payments_loanorder();
    float amount = 0.0;
    bool is_sticky = (lal_orm->get_status_id() == 11 || lal_orm->get_status_id() == 15);
    if (is_sticky){
        if (nli_orm->get_is_principal_paid() && (BDate(pl_orm->get_paid_at())() < BDate(nli_orm->get_undue_to_due_date())())){
            amount = (settlementByCustomerObject)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
        }
    } else {
        if (BDate(pl_orm->get_paid_at())() < BDate(nli_orm->get_due_to_overdue_date())() 
            && BDate(pl_orm->get_paid_at())() >= BDate(nli_orm->get_undue_to_due_date())() 
                || ((BDate(pl_orm->get_paid_at())() < BDate(nli_orm->get_undue_to_due_date())()
                    && BDate(nli_orm->get_undue_to_due_date())() <= settlementByCustomerObject->get_closing_day()()) 
                        && nli_orm->get_payment_status() != 3)){
            amount = (settlementByCustomerObject)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
        }
    }
    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_due_principal_paid_sec(LedgerClosureStep *settlementByCustomer)
{
    SettlementByCustomer* settlementByCustomerObject = settlementByCustomerObject;
    LedgerAmount* la = (settlementByCustomerObject)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = (settlementByCustomerObject)->get_new_lms_installmentextension();
    loan_app_loan_primitive_orm* lal_orm = (settlementByCustomerObject)->get_loan_app_loan();
    if (nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    loan_app_installment_primitive_orm* lai_orm =(settlementByCustomerObject)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = (settlementByCustomerObject)->get_payments_loanorder();
    float amount = 0.0;
    bool is_sticky = (lal_orm->get_status_id() == 11 || lal_orm->get_status_id() == 15);
    if (is_sticky){
        if (nli_orm->get_is_principal_paid() && (BDate(pl_orm->get_paid_at())() < BDate(nli_orm->get_undue_to_due_date())())){
            amount = (settlementByCustomerObject)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
        }
    } else {
        if (BDate(pl_orm->get_paid_at())() < BDate(nli_orm->get_due_to_overdue_date())() 
            && BDate(pl_orm->get_paid_at())() >= BDate(nli_orm->get_undue_to_due_date())() 
                || ((BDate(pl_orm->get_paid_at())() < BDate(nli_orm->get_undue_to_due_date())()
                    && BDate(nli_orm->get_undue_to_due_date())() <= settlementByCustomerObject->get_closing_day()()) 
                        && nli_orm->get_payment_status() != 3)){
            amount = (settlementByCustomerObject)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
        }
    }
    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_undue_principal_paid(LedgerClosureStep *settlementByCustomer)
{
    SettlementByCustomer* settlementByCustomerObject = settlementByCustomerObject;
    LedgerAmount* la = (settlementByCustomerObject)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = (settlementByCustomerObject)->get_new_lms_installmentextension();
    loan_app_loan_primitive_orm* lal_orm = (settlementByCustomerObject)->get_loan_app_loan();
    if (nli_orm->get_is_securitized() || (lal_orm->get_status_id() == 11 || lal_orm->get_status_id() == 15)) {
        la->setAmount(0);
        return la;
    }
    loan_app_installment_primitive_orm* lai_orm =(settlementByCustomerObject)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = (settlementByCustomerObject)->get_payments_loanorder();
    float amount = 0.0;
    if(nli_orm->get_payment_status() == 3 || (nli_orm->get_payment_status() == 1 && (settlementByCustomerObject)->get_settlement_day() != nullptr
            && (*settlementByCustomerObject->get_settlement_day())() < BDate(nli_orm->get_undue_to_due_date())())){
        amount = (settlementByCustomerObject)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
    }
    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_principal_long_term(LedgerClosureStep *settlementByCustomer)
{
    SettlementByCustomer* settlementByCustomerObject = settlementByCustomerObject;
    LedgerAmount* la = (settlementByCustomerObject)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = (settlementByCustomerObject)->get_new_lms_installmentextension();
    loan_app_loan_primitive_orm* lal_orm = (settlementByCustomerObject)->get_loan_app_loan();
    if (nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    loan_app_installment_primitive_orm* lai_orm =(settlementByCustomerObject)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = (settlementByCustomerObject)->get_payments_loanorder();
    float amount = 0.0;
    if(nli_orm->get_is_long_term() && nli_orm->get_payment_status() == 3 && nli_orm->get_is_principal_paid()
            && nli_orm->get_principal_payment_ledger_amount_id() == 0 && (BDate(nli_orm->get_principal_paid_at())() <= BDate(pl_orm->get_paid_at())())){
                amount = lai_orm->get_principal_expected();
    }

    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_principal_long_term_sec(LedgerClosureStep *settlementByCustomer)
{
    SettlementByCustomer* settlementByCustomerObject = settlementByCustomerObject;
    LedgerAmount* la = (settlementByCustomerObject)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = (settlementByCustomerObject)->get_new_lms_installmentextension();
    loan_app_loan_primitive_orm* lal_orm = (settlementByCustomerObject)->get_loan_app_loan();
    if (!nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    loan_app_installment_primitive_orm* lai_orm =(settlementByCustomerObject)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = (settlementByCustomerObject)->get_payments_loanorder();
    float amount = 0.0;
    if(nli_orm->get_is_long_term() && nli_orm->get_payment_status() == 3 && nli_orm->get_is_principal_paid()
            && nli_orm->get_principal_payment_ledger_amount_id() == 0 && (BDate(nli_orm->get_principal_paid_at())() <= BDate(pl_orm->get_paid_at())())){
                amount = lai_orm->get_principal_expected();
    }

    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_undue_principal_paid_sec(LedgerClosureStep *settlementByCustomer)
{
    SettlementByCustomer* settlementByCustomerObject = settlementByCustomerObject;
    LedgerAmount* la = (settlementByCustomerObject)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = (settlementByCustomerObject)->get_new_lms_installmentextension();
    loan_app_loan_primitive_orm* lal_orm = (settlementByCustomerObject)->get_loan_app_loan();
    if (!nli_orm->get_is_securitized() || (lal_orm->get_status_id() == 11 || lal_orm->get_status_id() == 15)) {
        la->setAmount(0);
        return la;
    }
    loan_app_installment_primitive_orm* lai_orm =(settlementByCustomerObject)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = (settlementByCustomerObject)->get_payments_loanorder();
    float amount = 0.0;
    if(nli_orm->get_payment_status() == 3 || (nli_orm->get_payment_status() == 1 && (settlementByCustomerObject)->get_settlement_day() != nullptr
            && (*settlementByCustomerObject->get_settlement_day())() < BDate(nli_orm->get_undue_to_due_date())())){
        amount = (settlementByCustomerObject)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
    }
    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_overdue_principal_paid(LedgerClosureStep *settlementByCustomer)
{
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentextension();
    if (nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    loan_app_loan_primitive_orm* lal_orm = ((SettlementByCustomer*)settlementByCustomer)->get_loan_app_loan();
    loan_app_installment_primitive_orm* lai_orm =((SettlementByCustomer*)settlementByCustomer)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = ((SettlementByCustomer*)settlementByCustomer)->get_payments_loanorder();
    float amount = 0.0;
    if(BDate(pl_orm->get_paid_at())() >= BDate(nli_orm->get_due_to_overdue_date())()){
        amount = ((SettlementByCustomer*)settlementByCustomer)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
    }
    la->setAmount(amount);
    return la;
}

LedgerAmount *SettlementByCustomer::_get_overdue_principal_paid_sec(LedgerClosureStep *settlementByCustomer)
{
    LedgerAmount* la = ((SettlementByCustomer*)settlementByCustomer)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nli_orm = ((SettlementByCustomer*)settlementByCustomer)->get_new_lms_installmentextension();
    if (!nli_orm->get_is_securitized()) {
        la->setAmount(0);
        return la;
    }
    loan_app_loan_primitive_orm* lal_orm = ((SettlementByCustomer*)settlementByCustomer)->get_loan_app_loan();
    loan_app_installment_primitive_orm* lai_orm =((SettlementByCustomer*)settlementByCustomer)->get_loan_app_installment();;
    payments_loanorder_primitive_orm* pl_orm = ((SettlementByCustomer*)settlementByCustomer)->get_payments_loanorder();
    float amount = 0.0;
    if(BDate(pl_orm->get_paid_at())() >= BDate(nli_orm->get_due_to_overdue_date())()){
        amount = ((SettlementByCustomer*)settlementByCustomer)->get_principal_paid(lal_orm, lai_orm, nli_orm, pl_orm);
    }
    la->setAmount(amount);
    return la;
}

float SettlementByCustomer::get_principal_paid(loan_app_loan_primitive_orm* lal_orm, loan_app_installment_primitive_orm* lai_orm,new_lms_installmentextension_primitive_orm* nli_orm,payments_loanorder_primitive_orm* pl_orm ){
    bool is_sticky = lal_orm->get_status_id() == 11 || lal_orm->get_status_id() == 15;
    if (nli_orm->get_is_principal_paid() && nli_orm->get_principal_payment_ledger_amount_id() == 0
            && (!nli_orm->get_is_long_term() || (is_sticky && nli_orm->get_is_long_term()))
                && (BDate(nli_orm->get_principal_paid_at())() <= BDate(pl_orm->get_paid_at())()) )
                    return lai_orm->get_principal_expected();
    return 0.0;
}

float SettlementByCustomer::get_interest_paid(LedgerClosureStep *settlementByCustomer, loan_app_loan_primitive_orm* lal_orm, loan_app_installment_primitive_orm* lai_orm,new_lms_installmentextension_primitive_orm* nli_orm,payments_loanorder_primitive_orm* pl_orm ){
    SettlementByCustomer* settlementByCustomerObject = ((SettlementByCustomer*)settlementByCustomer);
    bool is_sticky = lal_orm->get_status_id() == 11 || lal_orm->get_status_id() == 15;
    bool is_last_status_settled = settlementByCustomerObject->get_last_status() == 8 || settlementByCustomerObject->get_last_status() == 15 || settlementByCustomerObject->get_last_status() == 16;
    float amount = 0.0;
    if (nli_orm->get_is_interest_paid() && nli_orm->get_interest_payment_ledger_amount_id() == 0 && (BDate(nli_orm->get_interest_paid_at())() <= BDate(pl_orm->get_paid_at())())
            && ((nli_orm->get_payment_status() == 3) || (BDate(nli_orm->get_undue_to_due_date())() <= BDate(settlementByCustomerObject->get_closing_day())()) 
                || (is_last_status_settled && BDate(nli_orm->get_undue_to_due_date())() > BDate(settlementByCustomerObject->get_closing_day())()))){
                    amount += ROUND(nli_orm->get_actual_interest_paid());
                }//TODO COMPLETE CONDITIONS
    return 0.0;
}

loan_app_loan_primitive_orm *SettlementByCustomer::get_loan_app_loan()
{
    return lal_orm;
}

payments_loanorder_primitive_orm* SettlementByCustomer::get_payments_loanorder()
{
    return plo_orm;
}

loan_app_installment_primitive_orm *SettlementByCustomer::get_loan_app_installment()
{
    return lai_orm;
}

new_lms_installmentextension_primitive_orm *SettlementByCustomer::get_new_lms_installmentextension()
{
    return nli_orm;
}

vector<new_lms_installmentlatefees_primitive_orm *> *SettlementByCustomer::get_new_lms_installmentlatefees()
{
    return lf_orms;
}

int *SettlementByCustomer::get_unmarginalization_template_id(){
    return unmarginalization_template_id;
}

int SettlementByCustomer::get_last_status()
{
    return last_status;
}

BDate *SettlementByCustomer::get_settlement_day()
{
    return settlement_day;
}

BDate SettlementByCustomer::get_closing_day()
{
    return closing_day;
}

// vector <loan_app_installment_primitive_orm*> SettlementByCustomer::get_installments(){return installments;}        
// map <loan_app_installment_primitive_orm*, new_lms_installmentextension_primitive_orm*> SettlementByCustomer::get_installments_extensions(){return installments_extensions;}       
// map <loan_app_installment_primitive_orm*, vector <new_lms_installmentlatefees_primitive_orm*>> SettlementByCustomer::get_late_fees_map(){return late_fees_map;}   

// PSQLJoinQueryIterator* SettlementByCustomer::aggregator22(string _closure_date_string, int _agg_number, string processed_order_ids)
// {
//     SettlementByCustomer instance;

//     if(_agg_number == 1)
//         return instance._principal_orders_agg(_closure_date_string, processed_order_ids);
//     else if (_agg_number == 2)
//         return instance._interest_orders_agg(_closure_date_string, processed_order_ids);
//     else if (_agg_number == 3)
//         return instance._early_orders_agg(_closure_date_string, processed_order_ids);
//     else if (_agg_number == 4)
//         return instance._extra_orders_agg(_closure_date_string, processed_order_ids);
//     else if (_agg_number == 5)
//         return instance._lfs_orders_agg(_closure_date_string, processed_order_ids);
//     return nullptr;
// }


// PSQLJoinQueryIterator* SettlementByCustomer::_principal_orders_agg(string _closure_date_string, string processed_order_ids){
//     PSQLJoinQueryIterator * principalQuery = new PSQLJoinQueryIterator ("main",
//     {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"), new payments_loanorder_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main")},
//     {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"payments_loanorder", "id"}, {"new_lms_installmentextension", "principal_order_id"}}, {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}});

//         principalQuery->filter(
//             ANDOperator (
//                 new UnaryOperator("new_lms_installmentextension.is_principal_paid", eq, true),
//                 new UnaryOperator("new_lms_installmentextension.principal_payment_ledger_amount_id", isnull, "", true),
//                 new UnaryOperator("new_lms_installmentextension.principal_paid_at::date", lte, _closure_date_string),
//                 new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_BY_CUSTOMER-1),
//                 new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnotnull,"",true),
//                 new UnaryOperator("payments_loanorder.id", nin, processed_order_ids),
    
//                 new UnaryOperator ("loan_app_loan.id" , ne, "14312")
//             )
//         );
//         principalQuery->addExtraFromField("(select template_id from ledger_entry le where entry_id = (select entry_id from ledger_amount where id = new_lms_installmentextension.unmarginalization_ledger_amount_id)","unmarginalization_template");        

//         principalQuery->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.id");
//         principalQuery->setAggregates ({
//             {"loan_app_loan", {"id", 1}},  
//         });

//         return principalQuery;
// }

// PSQLJoinQueryIterator* SettlementByCustomer::_interest_orders_agg(string _closure_date_string, string processed_order_ids){
//     PSQLJoinQueryIterator * interestQuery = new PSQLJoinQueryIterator ("main",
//     {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"), new payments_loanorder_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main")},
//     {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"payments_loanorder", "id"}, {"new_lms_installmentextension", "interest_order_id"}}, {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}});

//         interestQuery->filter(
//             ANDOperator (
//                 new UnaryOperator("new_lms_installmentextension.is_interest_paid", eq, true),
//                 new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
//                 new UnaryOperator("new_lms_installmentextension.interest_payment_ledger_amount_id", isnull, "", true),
//                 new UnaryOperator("new_lms_installmentextension.interest_paid_at::date", lte, _closure_date_string),
//                 new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_BY_CUSTOMER-1),
//                 new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnotnull,"",true),
//                 new UnaryOperator("payments_loanorder.id", nin, processed_order_ids),
    
//                 new UnaryOperator ("loan_app_loan.id" , ne, "14312")
//             )
//         );
//         interestQuery->addExtraFromField("(select template_id from ledger_entry le where entry_id = (select entry_id from ledger_amount where id = new_lms_installmentextension.unmarginalization_ledger_amount_id)","unmarginalization_template");        

//         interestQuery->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.id");
//         interestQuery->setAggregates ({
//             {"loan_app_loan", {"id", 1}},  
//         });


//         return interestQuery;
// }

// PSQLJoinQueryIterator* SettlementByCustomer::_early_orders_agg(string _closure_date_string, string processed_order_ids){
//     PSQLJoinQueryIterator * earlyQuery = new PSQLJoinQueryIterator ("main",
//     {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"), new payments_loanorder_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main")},
//     {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"payments_loanorder", "id"}, {"new_lms_installmentextension", "early_order_id"}}, {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}});

//         earlyQuery->filter(
//             ANDOperator (
//                 new UnaryOperator("new_lms_installmentextension.is_early_paid", eq, true),
//                 new UnaryOperator("new_lms_installmentextension.early_fee_payment_ledger_amount_id", isnull, "", true),
//                 new UnaryOperator("new_lms_installmentextension.early_paid_at::date", lte, _closure_date_string),
//                 new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_BY_CUSTOMER-1),
//                 new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnotnull,"",true),
//                 new UnaryOperator("payments_loanorder.id", nin, processed_order_ids),
    
//                 new UnaryOperator ("loan_app_loan.id" , ne, "14312")
//             )
//         );
//         earlyQuery->addExtraFromField("(select template_id from ledger_entry le where entry_id = (select entry_id from ledger_amount where id = new_lms_installmentextension.unmarginalization_ledger_amount_id)","unmarginalization_template");        

//         earlyQuery->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.id");
//         earlyQuery->setAggregates ({
//             {"loan_app_loan", {"id", 1}},  
//         });


//         return earlyQuery;
// }

// PSQLJoinQueryIterator* SettlementByCustomer::_extra_orders_agg(string _closure_date_string, string processed_order_ids){
//     PSQLJoinQueryIterator * extraInterestQuery = new PSQLJoinQueryIterator ("main",
//     {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"), new payments_loanorder_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main")},
//     {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"payments_loanorder", "id"}, {"new_lms_installmentextension", "extra_interest_order_id"}}, {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}});

//         extraInterestQuery->filter(
//             ANDOperator (
//                 new UnaryOperator("new_lms_installmentextension.is_extra_interest_paid", eq, true),
//                 new UnaryOperator("new_lms_installmentextension.extra_interest_payment_ledger_amount_id", isnull, "", true),
//                 new UnaryOperator("new_lms_installmentextension.extra_interest_paid_at::date", lte, _closure_date_string),
//                 new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_BY_CUSTOMER-1),
//                 new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnotnull,"",true),
//                 new UnaryOperator("payments_loanorder.id", nin, processed_order_ids),
    
//                 new UnaryOperator ("loan_app_loan.id" , ne, "14312")
//             )
//         );
//         extraInterestQuery->addExtraFromField("(select template_id from ledger_entry le where entry_id = (select entry_id from ledger_amount where id = new_lms_installmentextension.unmarginalization_ledger_amount_id)","unmarginalization_template");        

//         extraInterestQuery->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.id");
//         extraInterestQuery->setAggregates ({
//             {"loan_app_loan", {"id", 1}},  
//         });


//         return extraInterestQuery;
// }

// PSQLJoinQueryIterator* SettlementByCustomer::_lfs_orders_agg(string _closure_date_string, string processed_order_ids){
//     PSQLJoinQueryIterator * lateFeesQuery = new PSQLJoinQueryIterator ("main",
//     {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"), new payments_loanorder_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main")},
//     {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"new_lms_installmentlatefees", "installment_extension_id"}, {"new_lms_installmentextension", "installment_ptr_id"}},{{"payments_loanorder", "id"}, {"new_lms_installmentlatefees", "order_id"}}, {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}});

//         lateFeesQuery->filter(
//             ANDOperator (
//                 new UnaryOperator("new_lms_installmentlatefees.is_paid", eq, true),
//                 new UnaryOperator("new_lms_installmentlatefees.is_cancelled", eq, false),
//                 new UnaryOperator("new_lms_installmentlatefees.payment_amount_id", isnull, "", true),
//                 new UnaryOperator("new_lms_installmentlatefees.paid_at::date", lte, _closure_date_string),
//                 new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_BY_CUSTOMER-1),
//                 new UnaryOperator("payments_loanorder.payment_ledger_entry_id", isnotnull,"",true),
//                 new UnaryOperator("payments_loanorder.id", nin, processed_order_ids),

//                 new UnaryOperator ("loan_app_loan.id" , ne, "14312")
//             )
//         );
//         lateFeesQuery->addExtraFromField("(select template_id from ledger_entry le where entry_id = (select entry_id from ledger_amount where id = new_lms_installmentextension.unmarginalization_ledger_amount_id)","unmarginalization_template");        

//         lateFeesQuery->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.id");
//         lateFeesQuery->setAggregates ({
//             {"loan_app_loan", {"id", 1}},  
//         });


//         return lateFeesQuery;
// }