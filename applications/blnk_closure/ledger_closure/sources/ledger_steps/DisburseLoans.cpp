#include <DisburseLoans.h>


DisburseLoan::DisburseLoan(vector<map <string,PSQLAbstractORM *> * > * _orms_list, float _percentage):LedgerClosureStep ()
{
    lal_orm = ORML(loan_app_loan,_orms_list,0);
    cac_orm = ORML(crm_app_customer,_orms_list,0);

    PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,_orms_list,0);
    short_term_principal = gorm->toFloat("short_term_principal");
    long_term_principal = gorm->toFloat("long_term_principal");
    created_from_rescheduling = gorm->toBool("created_from_rescheduling");  
    transaction_upfront_income_banked = gorm->toJson("transaction_upfront_income_banked");  
    transaction_upfront_income_unbanked = gorm->toJson("transaction_upfront_income_unbanked");  

    prov_percentage = _percentage;
    
    vector <new_lms_installmentextension_primitive_orm*>* nli_orms = new vector<new_lms_installmentextension_primitive_orm*>;
    for ( int i = 0 ;i < ORML_SIZE(_orms_list) ; i ++)
    {
        nli_orms->push_back(ORML(new_lms_installmentextension,_orms_list,i));
    }
    ie_list = nli_orms;

}


void DisburseLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    if(created_from_rescheduling){
        ledgerClosureService->addHandler("Booking rescheduled loan - long term, if applicable", DisburseLoan::_calc_long_term_receivable_balance);
        ledgerClosureService->addHandler("Booking rescheduled loan - short term; and", DisburseLoan::_calc_short_term_receivable_balance);
    }else{
        ledgerClosureService->addHandler("Booking new loan - long term, if applicable", DisburseLoan::_calc_long_term_receivable_balance);
        ledgerClosureService->addHandler("Booking new loan - short term; and", DisburseLoan::_calc_short_term_receivable_balance);
    }

    ledgerClosureService->addHandler("Booking the merchant’s commission income", DisburseLoan::_calc_mer_t_bl_fee);
    ledgerClosureService->addHandler("Booking an impairment provision", DisburseLoan::_calc_provision_percentage);
    ledgerClosureService->addHandler("Booking the cashier's commission expense", DisburseLoan::_calc_cashier_fee);
    ledgerClosureService->addHandler("Booking the merchant's commission expense", DisburseLoan::_calc_bl_t_mer_fee);
    ledgerClosureService->addHandler("Booking the transaction upfront fee", DisburseLoan::_calc_loan_upfront_fee);
}

DisburseLoan::~DisburseLoan(){}


LedgerAmount*  DisburseLoan::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCashierId(lal_orm->get_cashier_id());
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}

void DisburseLoan::stampORMs(ledger_entry_primitive_orm *entry, ledger_amount_primitive_orm *la_orm){
    lal_orm->setUpdateRefernce("loan_creation_ledger_entry_id", entry);
    if (la_orm != NULL)
    {
        for ( auto i : *ie_list)
        {
            if(!i->get_is_long_term()){
                i->setUpdateRefernce("short_term_ledger_amount_id", la_orm);
            }
        }
    }
    else cout << "ERROR in fetching first leg of the entry " << endl;
}

float DisburseLoan::get_upfront_income_cash(){
    loan_app_loan_primitive_orm* lal_orm = get_loan_app_loan();
    crm_app_customer_primitive_orm* cac_orm = get_crm_app_customer();
    json upfront_fee;  
    float fee = 0.0;

    if (cac_orm->get_limit_source() == 1) {
        upfront_fee = get_transaction_upfront_income_banked();
    }
    else {
        upfront_fee = get_transaction_upfront_income_unbanked();
    }

    if (upfront_fee["type"] == "Paid in Cash") {
        if (upfront_fee["data"]["option"] == "flat_fee"){
            fee = float(upfront_fee["data"]["flat_fee"]);
        }
        else if (upfront_fee["data"]["option"] == "percentage"){
            fee = ROUND((float(upfront_fee["data"]["percentage"])) / 100 * (lal_orm->get_principle()));
            if (upfront_fee["data"].contains("floor") && fee < float(upfront_fee["data"]["floor"]))
                fee = float(upfront_fee["data"]["floor"]);
            if (upfront_fee["data"].contains("cap") && fee > float(upfront_fee["data"]["cap"]))
                fee = float(upfront_fee["data"]["cap"]);
            else if (upfront_fee["data"]["option"] == "both"){
                fee = float(upfront_fee["data"]["flat_fee_bo"]) + ROUND(float(upfront_fee["data"]["percentage_bo"])) / 100 * (lal_orm->get_principle());
                if (upfront_fee["data"].contains("floor_bo") && fee < float(upfront_fee["data"]["floor_bo"]))
                    fee = float(upfront_fee["data"]["floor_bo"]);
                if (upfront_fee["data"].contains("cap_bo") && fee > float(upfront_fee["data"]["cap_bo"]))
                    fee = float(upfront_fee["data"]["cap_bo"]);
            }
        }
    }
    return fee;
    
}

json DisburseLoan::get_transaction_upfront_income_banked(){return transaction_upfront_income_banked;}
json DisburseLoan::get_transaction_upfront_income_unbanked(){return transaction_upfront_income_unbanked;}
loan_app_loan_primitive_orm* DisburseLoan::get_loan_app_loan() {return lal_orm;}
crm_app_customer_primitive_orm *DisburseLoan::get_crm_app_customer(){return cac_orm;}
float DisburseLoan::get_provision_percentage(){return prov_percentage;}
void DisburseLoan::set_provision_percentage(float _provision_percentage){prov_percentage = _provision_percentage;}
float DisburseLoan::get_short_term_principal(){return short_term_principal;}
void DisburseLoan::set_short_term_principal(float _short_term_principal){short_term_principal = _short_term_principal;}
void DisburseLoan::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm){lal_orm = _lal_orm;}
void DisburseLoan::set_crm_app_customer(crm_app_customer_primitive_orm *_cac_orm){cac_orm = _cac_orm;}
float DisburseLoan::get_long_term_principal(){return long_term_principal;}
void DisburseLoan::set_long_term_principal(float _long_term_principal){long_term_principal = _long_term_principal;}
bool DisburseLoan::get_created_from_rescheduling()  {return created_from_rescheduling;}
void DisburseLoan::set_created_from_rescheduling(bool _created_from_rescheduling){created_from_rescheduling =  _created_from_rescheduling;}


LedgerAmount * DisburseLoan::_calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan)
 {  
    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la->setAmount(ROUND(((DisburseLoan*)disburseLoan)->get_short_term_principal()));
    return la;

}


LedgerAmount * DisburseLoan::_calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan)
{
    LedgerAmount * ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double principal = loan_orm->get_principle();
    double merchant_to_blnk_fee = loan_orm->get_mer_t_bl_fee();

    double amount = ROUND((principal * (merchant_to_blnk_fee / 100)));
    ledgerAmount->setAmount(amount);
    
    return ledgerAmount;
}
LedgerAmount * DisburseLoan::_calc_provision_percentage(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double perc = ((DisburseLoan*)disburseLoan)->get_provision_percentage()/100;
    double amount = ROUND((loan_orm->get_principle()*perc));

    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la->setAmount(amount);
    
    return la;
}
LedgerAmount * DisburseLoan::_calc_cashier_fee(LedgerClosureStep *disburseLoan)
{
    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    loan_app_loan_primitive_orm* lal_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    
    float cashier_fee = (lal_orm->get_principle() * (lal_orm->get_cashier_fee()/ 100));
    la->setAmount(ROUND(cashier_fee));
 
    return la;
}
LedgerAmount * DisburseLoan::_calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double perc = loan_orm->get_bl_t_mer_fee()/100;
    double amount = ROUND((loan_orm->get_principle()*perc));
    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la->setAmount(amount);
    return la;
}
LedgerAmount * DisburseLoan::_calc_loan_upfront_fee(LedgerClosureStep *disburseLoan)
{
    LedgerAmount  * ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    bool created_from_rescheduling = ((DisburseLoan*)disburseLoan)->get_created_from_rescheduling();
    loan_app_loan_primitive_orm* lal_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    
    if(created_from_rescheduling){
        ledgerAmount->setAmount(0.0);
    }else if((!lal_orm->get_partial_refund_loan()) || (lal_orm->get_partial_refund_loan() && lal_orm->get_refund_upfront_fee_bool()) ){
        ledgerAmount->setAmount(((DisburseLoan*)disburseLoan)->get_upfront_income_cash());
    }
    
    return ledgerAmount;
}
LedgerAmount * DisburseLoan::_calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan)
{
    LedgerAmount * ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    float long_term_principal = ROUND(((DisburseLoan*)disburseLoan)->get_long_term_principal());
    ledgerAmount->setAmount(long_term_principal);
    return ledgerAmount;
}


PSQLJoinQueryIterator* DisburseLoan::aggregator(QueryExtraFeilds * query_fields){

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_bl_orm("main"), new crm_app_customer_primitive_orm("main"), new crm_app_purchase_primitive_orm("main"), new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
        {{{"loan_app_loan", "id"}, {"crm_app_purchase", "loan_id"}}, {{"loan_app_loan", "customer_id"}, {"crm_app_customer", "id"}}, {{"loan_app_loan", "id"}, {"loan_app_installment", "loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});

        psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
        psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");
        psqlQueryJoin->addExtraFromField("(SELECT cap2.is_rescheduled FROM crm_app_purchase cap INNER JOIN crm_app_purchase cap2 ON cap.parent_purchase_id = cap2.id WHERE  cap.id = crm_app_purchase.id)","created_from_rescheduling");
        psqlQueryJoin->addExtraFromField("(select transaction_upfront_income_banked from loan_app_loanproduct lal where lal.id = loan_app_loan.loan_product_id)","transaction_upfront_income_banked");
        psqlQueryJoin->addExtraFromField("(select  transaction_upfront_income_unbanked  from loan_app_loanproduct lal where lal.id = loan_app_loan.loan_product_id)","transaction_upfront_income_unbanked");
        
        psqlQueryJoin->filter(
            ANDOperator 
            (
                // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::DISBURSE_LOAN-1)),
                new UnaryOperator ("loan_app_loan.id" , ne, "14312"),
                new UnaryOperator ("loan_app_loan.loan_creation_ledger_entry_id",isnull,"",true),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,query_fields->closure_date_string),
                query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
                query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
            )
        );

        psqlQueryJoin->setAggregates({
            {"crm_app_customer", {"id", 1}},  
            {"loan_app_loan", {"id", 1}},  
            {"crm_app_purchase", {"id", 1}}
        });

        psqlQueryJoin->setOrderBy("crm_app_customer.id asc ,loan_app_loan.id asc,  crm_app_purchase.id asc");

        return psqlQueryJoin;
}


void DisburseLoan::update_step(){

        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::DISBURSE_LOAN)}}
        );
        psqlUpdateQuery.update();   
}