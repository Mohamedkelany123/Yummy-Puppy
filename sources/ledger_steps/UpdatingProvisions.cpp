#include "UpdatingProvisions.h"

PSQLJoinQueryIterator* UpdatingProvisions::aggregator(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {   
            new tms_app_bond_primitive_orm("main",false,true,-1,{"fundingfacility_ptr_id"}),
            new tms_app_loaninstallmentfundingrequest_primitive_orm("main",false,true,-1,{"installment_id","loan_id","funding_facility_id"}),
            new loan_app_installment_primitive_orm("main",false,true,-1,{"id","loan_id"}),
            new loan_app_loan_primitive_orm("main",false,true,-1,{"id","customer_id","merchant_id"}),
        },
        {
            {{{"tms_app_bond","fundingfacility_ptr_id"},{"tms_app_loaninstallmentfundingrequest","funding_facility_id"}},JOIN_TYPE::inner},
            {{{"tms_app_loaninstallmentfundingrequest","installment_id"},{"loan_app_installment","id"}},JOIN_TYPE::right},
            {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},JOIN_TYPE::inner}
        });

    psqlQueryJoin->filter(
        ANDOperator 
        (
            // new UnaryOperator("loan_app_loan.id",eq,30),
            new UnaryOperator("loan_app_loan.last_closing_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );
    psqlQueryJoin->setAggregates({{"loan_app_installment", {"loan_id",1}}, {"tms_app_loaninstallmentfundingrequest", {"funding_facility_id",1}}});
    psqlQueryJoin->setOrderBy("loan_app_installment.id asc");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
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
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance_on");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
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
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is not null and la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance_off");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
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
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance_on");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
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
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance_off");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
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
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance_on");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
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
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id \ 
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance_off");

    psqlQueryJoin->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id limit 1))","history_provision_percentage");
    psqlQueryJoin->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");
    // psqlJoinQuery->addExtraFromField("(select onbalance from tms_app_loaninstallmentfundingrequest tal where tal.id = tms_app_loaninstallmentfundingrequest.id)","on_balance");
    // psqlJoinQuery->addExtraFromField("(select funding_facility_id from tms_app_loaninstallmentfundingrequest tal2 where tal2.funding_facility_id=tms_app_loaninstallmentfundingrequest.funding_facility_id and tal2.loan_id = loan_app_loan.id limit 1)","funding_facility_id");
    // vector<pair<string, string>> distinct_map = {
    //     {"loan_app_loan", "id"},
    //     {"loan_app_loan","merchant_id"},
    //     {"loan_app_loan","customer_id"},
    //     {"tms_app_loaninstallmentfundingrequest" ,"funding_facility_id"},
    // };
    // psqlJoinQuery->setDistinct(distinct_map);

    // cout << psqlJoinQuery;

    return psqlQueryJoin;
}

loan_app_loan_primitive_orm_iterator* UpdatingProvisions::aggregator2(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    loan_app_loan_primitive_orm_iterator * psqlJoinQuery = new loan_app_loan_primitive_orm_iterator("main");

    psqlJoinQuery->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.id",eq,30),
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
    AND loan_id = loan_app_loan.id and la.bond_id is null and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance");

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
    AND loan_id = loan_app_loan.id and la.bond_id is null and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance");

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
    AND loan_id = loan_app_loan.id and la.bond_id is null and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance");

    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id limit 1))","history_provision_percentage");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");

    cout << psqlJoinQuery;

    return psqlJoinQuery;
}

UpdatingProvisions::UpdatingProvisions(map<string,PSQLAbstractORM*>*_orms,string start_date_input, string end_date_input){
    lal_orm = ORM(loan_app_loan,_orms);
    tal_orm = ORM(tms_app_loaninstallmentfundingrequest, _orms);

    cout << "LOAN ID : " << lal_orm->get_id() << endl;
    cout << "MERCHANT ID: " << lal_orm->get_merchant_id() << endl;
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    long_term_balance = gorm->toFloat("long_term_balance");
    cout << "LONG TERM BALANCE : ";
    cout << long_term_balance << endl;
    loans_rec_balance = gorm->toFloat("loans_rec_balance");
    cout << "Loans receivable BALANCE : ";
    cout << loans_rec_balance << endl;
    impairment_provisions_balance = gorm->toFloat("impairment_provisions_balance");
    cout << "IMP prov BALANCE : ";
    cout << impairment_provisions_balance << endl;
    history_provision_percentage = gorm->toInt("history_provision_percentage");
    cout << "HISTORY PERCENTAGE " << gorm->get("history_provision_percentage");
    loan_provision_percentage = gorm->toInt("loan_provision_percentage");
    cout << "LOAN PERCENTAGE " << gorm->get("loan_provision_percentage");
    on_balance = gorm->toBool("on_balance");
    funding_facility_id = gorm->toInt("funding_facility_id");
    cout << "BOND ID : " << gorm->get("funding_facility_id") << endl;
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
}

UpdatingProvisions::UpdatingProvisions(loan_app_loan_primitive_orm * loan,string start_date_input,string end_date_input){
    lal_orm = loan;
    cout << "LOAN ID : " << lal_orm->get_id() << endl;
    cout << "MERCHANT ID: " << lal_orm->get_merchant_id() << endl;
    long_term_balance = std::stof(lal_orm->getExtra("long_term_balance"));
    cout << "LONG TERM BALANCE : ";
    cout << long_term_balance << endl;
    loans_rec_balance = std::stof(lal_orm->getExtra("loans_rec_balance"));
    cout << "Loans receivable BALANCE : ";
    cout << loans_rec_balance << endl;
    impairment_provisions_balance = std::stof(lal_orm->getExtra("impairment_provisions_balance"));
    cout << "IMP prov BALANCE : ";
    cout << impairment_provisions_balance << endl;
    history_provision_percentage = std::stoi(lal_orm->getExtra("history_provision_percentage"));
    cout << "HISTORY PERCENTAGE " << lal_orm->getExtra("history_provision_percentage");
    loan_provision_percentage = std::stoi(lal_orm->getExtra("loan_provision_percentage"));
    cout << "LOAN PERCENTAGE " << lal_orm->getExtra("loan_provision_percentage");
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
}

LedgerAmount* UpdatingProvisions::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    // if(!on_balance){
    //     lg->setBondId(funding_facility_id);
    // }
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

void UpdatingProvisions::setMissingProvisions(){
    float expected_provisions = 0;
    float loan_marginalized_balance = loans_rec_balance + long_term_balance;
    if(end_date != ""){
        expected_provisions = (history_provision_percentage / 100) * loan_marginalized_balance;
    }
    else{
        expected_provisions = (loan_provision_percentage / 100) * loan_marginalized_balance;
    }
    // cout << "EXPECTED PROVISIONS : " << expected_provisions << endl;
    missing_provisions =  std::round((expected_provisions - std::abs(impairment_provisions_balance)) * 100.0) / 100.0;
}

LedgerAmount * UpdatingProvisions::_increasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    // cout << "MISSING PROVISIONS : " << missing_provisions << endl;
    if(missing_provisions > 0){
        // cout << "-----------INCREASING PROVISIONS WITH " << missing_provisions << endl;
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
        return la;
    }
}

LedgerAmount * UpdatingProvisions::_decreasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    cout << "GOT MISSING PROVISIONS WITH : " << missing_provisions << endl;
    if(missing_provisions < 0){
        cout << "-----------DECREASING PROVISIONS WITH " << missing_provisions << endl;
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
        return la;
    }
}

UpdatingProvisions::~UpdatingProvisions(){}

