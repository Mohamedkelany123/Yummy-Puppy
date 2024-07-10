#include "UpdatingProvisions.h"

PSQLJoinQueryIterator* UpdatingProvisions::aggregator(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    PSQLJoinQueryIterator * psqlJoinQuery = new PSQLJoinQueryIterator("main",
    {new loan_app_loan_primitive_orm("main"), new tms_app_loaninstallmentfundingrequest_primitive_orm("main")}
    ,{{{"loan_app_loan", "id"}, {"tms_app_loaninstallmentfundingrequest", "loan_id"}}});
    psqlJoinQuery->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.last_closing_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );

    psqlJoinQuery->addExtraFromField("WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Loans receivable (gross)'\
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
    AND loan_id = loan_app_loan.id and la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id and le.entry_date between " + start_date_string + " and " + end_date_string,"loans_rec_balance");

    psqlJoinQuery->addExtraFromField("WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Long-term loans receivable (net)'\
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
    AND loan_id = loan_app_loan.id and la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id and le.entry_date between " + start_date_string + " and " + end_date_string,"long_term_balance");

    psqlJoinQuery->addExtraFromField("WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Impairment provisions, On-balance sheet'\
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
    AND loan_id = loan_app_loan.id and la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id and le.entry_date between " + start_date_string + " and " + end_date_string,"impairment_provisions_balance");

    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date + "and status_type = 1 order by id limit 1))","history_provision_percentage");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status)","loan_provision_percentage");
    vector<pair<string, string>> distinct_map = {
        {"loan_app_loan", "id"},
        {"tms_app_loaninstallmentfundingrequest", "funding_facility_id"}
    };
    psqlJoinQuery->setDistinct(distinct_map);

    return psqlJoinQuery;
}

UpdatingProvisions::UpdatingProvisions(map<string,PSQLAbstractORM*>*_orms,string start_date_input, string end_date_input){
    lal_orm = ORM(loan_app_loan,_orms);
    tal_orm = ORM(tms_app_loaninstallmentfundingrequest, _orms);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    long_term_balance = gorm->toFloat("long_term_balance");
    loans_rec_balance = gorm->toFloat("loans_rec_balance");
    impairment_provisions_balance = gorm->toFloat("impairment_provisions_balance");
    history_provision_percentage = gorm->toInt("history_provision_percentage");
    loan_provision_percentage = gorm->toInt("loan_provision_percentage");
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
}

LedgerAmount* UpdatingProvisions::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    if(tal_orm->get_onbalance() == true){
        lg->setBondId(tal_orm->get_funding_facility_id());
    }
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
    ledgerClosureService->addHandler("Increasing provisions",_increasing_provisions);
    ledgerClosureService->addHandler("Decreasing provisions",_decreasing_provisions);
}

float UpdatingProvisions::get_missing_provisions(){
    return missing_provisions;
}

void UpdatingProvisions::set_missing_provisions(float x){
    missing_provisions = x;
}

void UpdatingProvisions::setMissingProvisions(){
    float expected_provisions = 0;
    float loan_marginalized_balance = loans_rec_balance + long_term_balance;
    if(end_date != ""){
        expected_provisions = (history_provision_percentage / 100) * loan_marginalized_balance;
    }
    else{
        expected_provisions = (loan_provision_percentage / 100) * loan_marginalized_balance;
    }
    missing_provisions =  std::round((expected_provisions - std::abs(impairment_provisions_balance)) * 100.0) / 100.0;
}

LedgerAmount * UpdatingProvisions::_increasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    if(missing_provisions > 0){
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
    }
}

LedgerAmount * UpdatingProvisions::_decreasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    if(missing_provisions < 0){
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
    }
}

UpdatingProvisions::~UpdatingProvisions(){}

