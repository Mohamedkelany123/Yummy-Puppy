#include <IScoreNidInquiry.h>


IScoreNidInquiry::IScoreNidInquiry(map<string,PSQLAbstractORM *> * _orms, float _inquiry_fee)
{
    nid_orm = ORM(ekyc_app_nidlog,_orms);
    onb_orm = ORM(ekyc_app_onboardingsession,_orms);
    cout << "MERCHANT STAFF ID IS : ";
    cout << onb_orm->get_merchant_staff_id() << endl;
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    if(gorm->get("merchant_id") == ""){
        cout << "MERCHANT ID 2 IS :";

        merchantID =gorm->toInt("merchant_id2");
        cout << merchantID << endl;
    }
    else{
        merchantID =gorm->toInt("merchant_id");
        cout << gorm->toInt("merchant_id") << endl;
    }
    
    std::stringstream ss2(gorm->get("customer_id"));
    cout << "CUSTOMER ID IS :";
    cout << gorm->get("customer_id") << endl;
    if (gorm->get("customer_id") == ""){
        customerID = -1;
    }
    ss2 >> customerID;
    cout << "Stored customer id is";
    cout << customerID;

    inquiryFee = _inquiry_fee;
}

PSQLJoinQueryIterator* IScoreNidInquiry::aggregator(string _closure_date_string){
    PSQLJoinQueryIterator * nidLogsQuery = new PSQLJoinQueryIterator("main",
    {new ekyc_app_nidlog_primitive_orm("main"), new ekyc_app_onboardingsession_primitive_orm("main")},
    {{{"ekyc_app_nidlog","onboarding_session_id"},{"ekyc_app_onboardingsession","id"}}}
    );

    nidLogsQuery->filter(
        ANDOperator(
            new UnaryOperator("ekyc_app_nidlog.status",eq,1),
            new UnaryOperator("ekyc_app_nidlog.nid_expense_ledger_entry_id",isnull,"",true),
            new UnaryOperator("ekyc_app_nidlog.created_at",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::NID_ISCORE-1)
        )
    );
    string query_closure_date = "'" + _closure_date_string + "'"; 

    nidLogsQuery->addExtraFromField("(select merchant_id from crm_app_merchantstaffhistory cam where cam.staff_id=ekyc_app_onboardingsession.merchant_staff_id and cam.created_at <= " + query_closure_date + " order by id desc limit 1)","merchant_id");
    nidLogsQuery->addExtraFromField("(select id from crm_app_customer cac where cac.national_id = ekyc_app_onboardingsession.national_id limit 1)","customer_id");
    nidLogsQuery->addExtraFromField("(select merchant_id from crm_app_merchantstaff cam where ekyc_app_onboardingsession.merchant_staff_id = cam.user_ptr_id limit 1)","merchant_id2");
    return nidLogsQuery;
}

LedgerAmount* IScoreNidInquiry::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setMerchantId(merchantID);
    lg->setCashierId(onb_orm->get_merchant_staff_id());
    if(customerID != -1){
        lg->setCustomerId(customerID);
    }

    return lg;
}

void IScoreNidInquiry::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::NID_ISCORE)}}
        );
        psqlUpdateQuery.update();  
}

void IScoreNidInquiry::setupLedgerClosureService(LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("iScore NID inquiry fee expense",_calculate_inquiry_fee);
}

void IScoreNidInquiry::stampORMs(ledger_entry_primitive_orm * entry){
    // map<string,LedgerCompositLeg*>::iterator it = leg_amounts->begin();
    // ledger_amount_primitive_orm* first_leg_amount = it->second->getLedgerCompositeLeg()->first;
    // if (first_leg_amount != NULL){
    nid_orm->setUpdateRefernce("nid_expense_ledger_entry_id",entry);
    // }
    // else cout << "ERROR in fetching first leg of the entry " << endl;
}

float IScoreNidInquiry::get_inquiry_fee(){
    return inquiryFee;
}

LedgerAmount * IScoreNidInquiry::_calculate_inquiry_fee(LedgerClosureStep *iScoreNidInquiry){
    LedgerAmount * la = ((IScoreNidInquiry*)iScoreNidInquiry)->_init_ledger_amount();
    la->setAmount(((IScoreNidInquiry*)iScoreNidInquiry)->get_inquiry_fee());
    cout << la << endl;

    return la;
}

IScoreNidInquiry::~IScoreNidInquiry(){}



