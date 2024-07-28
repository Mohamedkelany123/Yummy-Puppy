#include <CreditIScore.h>

CreditIScore::CreditIScore(map<string,PSQLAbstractORM *> * _orms, float _expense_fee):LedgerClosureStep ()
{
    cail_orm = ORM(credit_app_inquirylog,_orms);    
    eyobs_orm = ORM(ekyc_app_onboardingsession,_orms);    
    template_id = 1;
    expense_fee = _expense_fee;
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    if(gorm->get("merchant_id") == "")
        merchant_id = gorm->toInt("merchant_id2");
    else
        merchant_id = gorm->toInt("merchant_id");
}

void CreditIScore::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("iScore inquiry fee expense", CreditIScore::_calculate_inquiry_fee);
}

CreditIScore::~CreditIScore(){}

PSQLJoinQueryIterator* CreditIScore::aggregator(QueryExtraFeilds * query_fields){

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new credit_app_inquirylog_primitive_orm("main"), new crm_app_customer_primitive_orm("main"),  new ekyc_app_onboardingsession_primitive_orm("main")},
        {{{"credit_app_inquirylog", "customer_id"}, {"crm_app_customer", "id"}}, {{"credit_app_inquirylog", "onboarding_session_id"},{"ekyc_app_onboardingsession", "id"}}});

        // _closure_date_string = "'"+ _closure_date_string+"'";
        psqlQueryJoin->filter(
            ANDOperator 
            (
                // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::DISBURSE_LOAN-1)),
                new UnaryOperator ("credit_app_inquirylog.iscore_ledger_amount_id",isnull,"",true),
                new UnaryOperator ("ekyc_app_onboardingsession.onboarding_step",eq,"4"),
                new UnaryOperator ("crm_app_customer.state",in,"3,4,5,7"),
                new UnaryOperator ("credit_app_inquirylog.status",in,"'NO_HIT','BANKED','ZERO_BANKED_LIMIT'"),
                new UnaryOperator ("crm_app_customer.created_at",gte,"2023-09-01"),
                new UnaryOperator ("credit_app_inquirylog.created_at::date",lte,query_fields->closure_date_string),
                query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
                query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
            )
        );

        psqlQueryJoin->addExtraFromField("(select merchant_id from crm_app_merchantstaffhistory cam where cam.staff_id=ekyc_app_onboardingsession.merchant_staff_id and cam.created_at::date <= '" + query_fields->closure_date_string + "' order by id desc limit 1)","merchant_id");
        psqlQueryJoin->addExtraFromField("(select merchant_id from crm_app_merchantstaff cam where ekyc_app_onboardingsession.merchant_staff_id = cam.user_ptr_id limit 1)","merchant_id2");
        
        return psqlQueryJoin;
}

LedgerAmount * CreditIScore::_calculate_inquiry_fee(LedgerClosureStep *creditIScore)
{
    double expense_fee = ((CreditIScore*)creditIScore)->get_expense_fee();

    LedgerAmount * la = ((CreditIScore*)creditIScore)->_init_ledger_amount();
    la->setAmount(expense_fee);
    
    return la;
}

float CreditIScore::get_expense_fee()
{
    return expense_fee;
}

void CreditIScore::set_credit_app_inquirylog(credit_app_inquirylog_primitive_orm * _cail_orm){
    cail_orm = _cail_orm;
}

credit_app_inquirylog_primitive_orm * CreditIScore::get_credit_app_inquirylog(){
    return cail_orm;
}

void CreditIScore::set_template_id(int _template_id){
    template_id = _template_id;
}

int CreditIScore::get_template_id(){
    return template_id;
}

LedgerAmount * CreditIScore::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCashierId(eyobs_orm->get_merchant_staff_id());
    lg->setCustomerId(cail_orm->get_customer_id());
    lg->setMerchantId(merchant_id);

    return lg;
}


void CreditIScore::stampORMs(ledger_entry_primitive_orm *entry, ledger_amount_primitive_orm *la_orm){
    cail_orm->setUpdateRefernce("iscore_ledger_amount_id", la_orm);
}