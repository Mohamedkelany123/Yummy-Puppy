#include "UpdatingProvisions.h"


loan_app_loan_primitive_orm_iterator* UpdatingProvisions::aggregator_onbalance(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string balance_query = "(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = '%1%'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id is null \
    and le.entry_date between %2% and %3% )";
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string loans_rec_name = "Loans receivable (gross)";
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    loan_app_loan_primitive_orm_iterator * psqlJoinQuery = new loan_app_loan_primitive_orm_iterator("main");

    psqlJoinQuery->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.last_closing_day",lt,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );

    psqlJoinQuery->addExtraFromField(boost::str(boost::format(balance_query) % "Loans receivable (gross)" % start_date_string % end_date_string),"loans_rec_balance");
    psqlJoinQuery->addExtraFromField(boost::str(boost::format(balance_query) % "Long-term loans receivable (net)" % start_date_string % end_date_string),"long_term_balance");
    psqlJoinQuery->addExtraFromField(boost::str(boost::format(balance_query) % "Impairment provisions, On-balance sheet" % start_date_string % end_date_string),"impairment_provisions_balance");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id desc limit 1))","history_provision_percentage");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");

    cout << psqlJoinQuery;

    return psqlJoinQuery;
}

PSQLJoinQueryIterator* UpdatingProvisions::aggregator_offbalance(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string balance_query = "(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = '%1%'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id = tms_app_loanfundingrequest.funding_facility_id \
    and le.entry_date between %2% and %3% )";
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    PSQLJoinQueryIterator * psqlJoinQuery = new PSQLJoinQueryIterator("main",
    {
        new tms_app_bond_primitive_orm("main"),
        new tms_app_loanfundingrequest_primitive_orm("main"),
        new loan_app_loan_primitive_orm("main")
        },
    {
        {{"tms_app_bond","fundingfacility_ptr_id"},{"tms_app_loanfundingrequest","funding_facility_id"}},
        {{"tms_app_loanfundingrequest","loan_id"},{"loan_app_loan","id"}}
    }
    );

    psqlJoinQuery->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.last_closing_day",lt,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );

    psqlJoinQuery->addExtraFromField(boost::str(boost::format(balance_query) % "Loans receivable (gross)" % start_date_string % end_date_string),"loans_rec_balance");
    psqlJoinQuery->addExtraFromField(boost::str(boost::format(balance_query) % "Long-term loans receivable (net)" % start_date_string % end_date_string),"long_term_balance");
    psqlJoinQuery->addExtraFromField(boost::str(boost::format(balance_query) % "Impairment provisions, On-balance sheet" % start_date_string % end_date_string),"impairment_provisions_balance");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id desc limit 1))","history_provision_percentage");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");

    cout << psqlJoinQuery;

    return psqlJoinQuery;
}

UpdatingProvisions::UpdatingProvisions(map<string,PSQLAbstractORM*>*_orms,string start_date_input, string end_date_input){
    lal_orm = ORM(loan_app_loan,_orms);
    tal_orm = ORM(tms_app_loanfundingrequest, _orms);

    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    long_term_balance = gorm->toFloat("long_term_balance");
    loans_rec_balance = gorm->toFloat("loans_rec_balance");
    impairment_provisions_balance = gorm->toFloat("impairment_provisions_balance");
    history_provision_percentage = gorm->toInt("history_provision_percentage");
    loan_provision_percentage = gorm->toInt("loan_provision_percentage");
    on_balance = gorm->toBool("on_balance");
    on_balance = false;
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
    this->calculateMissingProvisions();
}

UpdatingProvisions::UpdatingProvisions(loan_app_loan_primitive_orm * loan,string start_date_input,string end_date_input){
    lal_orm = loan;
    long_term_balance = lal_orm->getExtraToFloat("long_term_balance");
    loans_rec_balance = lal_orm->getExtraToFloat("loans_rec_balance");
    impairment_provisions_balance = lal_orm->getExtraToFloat("impairment_provisions_balance");
    history_provision_percentage = lal_orm->getExtraToFloat("history_provision_percentage");
    loan_provision_percentage = lal_orm->getExtraToFloat("loan_provision_percentage");
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
    on_balance = true;
    this->calculateMissingProvisions();
}

LedgerAmount* UpdatingProvisions::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    if(!on_balance){
        lg->setBondId(funding_facility_id);
    }
    return lg;
}

void UpdatingProvisions::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::UPDATE_PROVISION)}}
        );
        psqlUpdateQuery.update();  
}

void UpdatingProvisions::setupLedgerClosureService(LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Increasing provisions ",_increasing_provisions);
    ledgerClosureService->addHandler("Decreasing provisions ",_decreasing_provisions);
}

float UpdatingProvisions::get_missing_provisions(){
    return missing_provisions;
}

void UpdatingProvisions::set_missing_provisions(float x){
    missing_provisions = x;
}

void UpdatingProvisions::calculateMissingProvisions(){
    double expected_provisions = 0;
    double loan_marginalized_balance = loans_rec_balance + long_term_balance;
    if(end_date != ""){
        expected_provisions = std::floor(((static_cast<double>(history_provision_percentage) / 100.00) * loan_marginalized_balance)*100) / 100;
    }
    else{
        expected_provisions = (loan_provision_percentage / 100) * loan_marginalized_balance;
    }
    expected_provisions = ROUND(expected_provisions);
    missing_provisions =  ROUND((expected_provisions - std::abs(impairment_provisions_balance)));
    if(missing_provisions <= 0.01 && missing_provisions >= -0.01){
        missing_provisions = 0.0;
    }
}

LedgerAmount * UpdatingProvisions::_increasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    if(missing_provisions > 0.0){
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
        return la;
    }
    else{
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(0.0);
        return la;
    }
}

LedgerAmount * UpdatingProvisions::_decreasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    if(missing_provisions < 0.0){
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
        return la;
    }
    else{
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(0.0);
        return la;
    }
}

UpdatingProvisions::~UpdatingProvisions(){}

