#include "AccrualInterest.h"

AccrualInterest::AccrualInterest()
{
        lal_orm = nullptr;
        lai_orm = nullptr;
        nli_orm = nullptr;
        marginalization_history = "";
        last_order_date = "";
        settled_history = "";
        accrual_type = -1;
}    
AccrualInterest::AccrualInterest(map<string, PSQLAbstractORM *> *_orms, int _accrual_type)
{
    loan_app_loan_primitive_orm * _lal_orm = ORMBL(loan_app_loan,_orms);
    loan_app_installment_primitive_orm * _lai_orm = ORM(loan_app_installment,_orms);
    new_lms_installmentextension_primitive_orm * _nli_orm = ORM(new_lms_installmentextension,_orms);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    if(_accrual_type == 1){
        string _marginalization_history = gorm->get("_marginalization_history");
        string _last_order_date = gorm->get("last_order_date");
        string _settled_history = gorm->get("settled_history");
        marginalization_history = _marginalization_history;
        last_order_date = _last_order_date;
        settled_history = _settled_history;
    } else if(_accrual_type == 2){
        marginalization_history = gorm->get("marginalization_history");
    }
    lal_orm = _lal_orm;  
    lai_orm =_lai_orm;
    nli_orm = _nli_orm;
    accrual_type = _accrual_type;
}

AccrualInterest::~AccrualInterest()
{
}

void AccrualInterest::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("Booking marginalized interest income, if applicable", _get_marginalization_interest);
    ledgerClosureService->addHandler("Interest income accrual (undue)", _get_accrued_interest);

}

LedgerAmount *AccrualInterest::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    lg->setInstallmentId(lai_orm->get_id());

    return lg;
}

void AccrualInterest::stampORMs(map<string, LedgerCompositLeg *> *leg_amounts)
{
    map<string, LedgerCompositLeg *>::iterator it = leg_amounts->begin();
    ledger_amount_primitive_orm* first_leg_amount = it->second->getLedgerCompositeLeg()->first;
    string accrual_key, marginalization_key;
    if (accrual_type == 1){
        accrual_key = "accrual_ledger_amount_id";
        marginalization_key = "marginalization_ledger_amount_id";
        nli_orm->set_actual_accrued_amount(abs(first_leg_amount->get_amount()));
    }
    else if (accrual_type == 2) {
        accrual_key = "partial_accrual_ledger_amount_id";
        marginalization_key = "partial_marginalization_ledger_amount_id";
        nli_orm->set_partial_accrual_amount(abs(first_leg_amount->get_amount()));
    }
    else if (accrual_type == 3) {
        accrual_key = "settlement_accrual_interest_ledger_amount_id";
    }
    
    nli_orm->setUpdateRefernce(accrual_key, first_leg_amount);
    it++;
    if(it != leg_amounts->end() && accrual_type != 3){
        ledger_amount_primitive_orm* second_leg_amount = it->second->getLedgerCompositeLeg()->first;
        nli_orm->setUpdateRefernce(marginalization_key, second_leg_amount);
    }
}

void AccrualInterest::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm){lal_orm= _lal_orm;}
void AccrualInterest::set_loan_app_installment(loan_app_installment_primitive_orm *_lai_orm){lai_orm= _lai_orm;}
void AccrualInterest::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm *_nli_orm){nli_orm= _nli_orm;}
void AccrualInterest::set_marginalization_history(string _marginalization_history){marginalization_history= _marginalization_history;}
loan_app_loan_primitive_orm *AccrualInterest::get_loan_app_loan(){return lal_orm;}
loan_app_installment_primitive_orm *AccrualInterest::get_loan_app_installment(){return lai_orm;}
new_lms_installmentextension_primitive_orm* AccrualInterest::get_new_lms_installmentextension(){return nli_orm;}
const string AccrualInterest::get_marginalization_history(){return marginalization_history;}
const int AccrualInterest::get_accrual_type(){return accrual_type;}

LedgerAmount *AccrualInterest::_get_marginalization_interest(LedgerClosureStep *accrualInterest)
{
    LedgerAmount* ledgerAmount = ((AccrualInterest*)accrualInterest)->_init_ledger_amount();
    loan_app_loan_primitive_orm* lal_orm = ((AccrualInterest*) accrualInterest)->get_loan_app_loan();
    new_lms_installmentextension_primitive_orm* nli_orm = ((AccrualInterest*)accrualInterest)->get_new_lms_installmentextension();
    int accrual_type = ((AccrualInterest*)accrualInterest)->get_accrual_type();

    int partialMarginalizationLedgerAmountId = nli_orm->get_partial_marginalization_ledger_amount_id();
    int marginalizationLedgerAmountId = nli_orm->get_marginalization_ledger_amount_id();

    string history = ((AccrualInterest*) accrualInterest)->get_marginalization_history();

    // 1) accrual 2) partial accrual 3) settlement accrual
    if (accrual_type == 3)
        return ledgerAmount;
    if (accrual_type == 2 && partialMarginalizationLedgerAmountId == 0) 
        return ledgerAmount;
    if (accrual_type != 2 && marginalizationLedgerAmountId == 0)
        return ledgerAmount;
    
    bool is_marginalized = nli_orm->get_is_marginalized();
    bool is_partially_marginalized = nli_orm->get_is_partially_marginalized();
    bool is_interest_paid = nli_orm->get_is_interest_paid();
    BDate interest_paid_at(nli_orm->get_interest_paid_at());
    BDate marginalization_date(nli_orm->get_marginalization_date());
    BDate history_bdate(history);
    BDate accrual_date((nli_orm->get_accrual_date()));
    BDate partial_accrual_date(nli_orm->get_partial_accrual_date());
    BDate partial_marginalization(nli_orm->get_partial_marginalization_date());
    float first_installment_interest_adjustment = nli_orm->get_first_installment_interest_adjustment();
    
    if (history != "") {
        if (accrual_type == 2) {
            if (is_partially_marginalized || (!is_partially_marginalized && is_interest_paid && interest_paid_at() >= history_bdate()) && accrual_date() >= history_bdate()) {
                if (partial_accrual_date() == partial_marginalization()) {
                    return _get_accrued_interest(accrualInterest);
                }
            }
        }
        else {
            if (is_marginalized || (!is_marginalized && is_interest_paid && interest_paid_at() >= history_bdate()) && accrual_date() >= history_bdate()) {
                if (accrual_date() == marginalization_date()) {
                    float amount;
                    amount = _get_accrued_interest(accrualInterest)->getAmount() + first_installment_interest_adjustment;
                    ledgerAmount->setAmount(ROUND(amount));
                }
            }
        }
    }
    return ledgerAmount;
}

LedgerAmount *AccrualInterest::_get_accrued_interest(LedgerClosureStep *accrualInterest)
{
    LedgerAmount* ledgerAmount = ((AccrualInterest*)accrualInterest)->_init_ledger_amount();
    loan_app_loan_primitive_orm* lal_orm = ((AccrualInterest*) accrualInterest)->get_loan_app_loan();
    loan_app_installment_primitive_orm* lai_orm = ((AccrualInterest*) accrualInterest)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nli_orm = ((AccrualInterest*)accrualInterest)->get_new_lms_installmentextension();
    int accrual_type = ((AccrualInterest*)accrualInterest)->get_accrual_type();
    BDate installment_day = BDate(lai_orm->get_day());
    

    if (accrual_type == 2) {
        float expected_partial_accrual_amount = nli_orm->get_expected_partial_accrual_amount();
        ledgerAmount->setAmount(expected_partial_accrual_amount);
    }
    else if(accrual_type == 3) {
        float settlement_accrual_interest_amount = nli_orm->get_settlement_accrual_interest_amount();
        ledgerAmount->setAmount(settlement_accrual_interest_amount);
    }
    else {
        float expected_accrual_amount = nli_orm->get_expected_accrual_amount();
        ledgerAmount->setAmount(expected_accrual_amount);
    }

    return ledgerAmount;
}



PSQLJoinQueryIterator* AccrualInterest::partial_accrual_agg(QueryExtraFeilds * query_fields)
{
    PSQLJoinQueryIterator * partialAccrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});
    
    partialAccrualQuery->filter(
        ANDOperator (
            new UnaryOperator("new_lms_installmentextension.partial_accrual_date", lte, query_fields->closure_date_string),
            // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::PARTIAL_INTEREST_ACCRUAL-1),
            new UnaryOperator ("loan_app_loan.id" , ne, "14312"),

          
          
            new UnaryOperator("new_lms_installmentextension.partial_accrual_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.expected_partial_accrual_amount", ne, 0),
            new OROperator(
                new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
                    new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date",gte, "new_lms_installmentextension.partial_accrual_date", true),
                    new UnaryOperator("loan_app_loan.status_id", eq,15)
                ),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
                    new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date",gt, "new_lms_installmentextension.partial_accrual_date", true),
                    new UnaryOperator("loan_app_loan.status_id", ne,15)
                )
            ),
            new OROperator(

                new UnaryOperator("new_lms_installmentextension.is_interest_paid", eq, true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.status_id", nin, "8, 15"),
                    new OROperator(
                        new UnaryOperator("new_lms_installmentextension.status_id", ne, 16),
                        new UnaryOperator("new_lms_installmentextension.payment_status", ne, 3)
                    )
                )
            ),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_loan.status_id", nin, "12,13"),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.partial_accrual_date", ne, "new_lms_installmentextension.accrual_date", true),
            new UnaryOperator ("loan_app_loan.id",ne,"14312")
        )
    );

    partialAccrualQuery->addExtraFromField("(select day from loan_app_loanstatushistroy lalsh  where loan_app_loan.id=lalsh.loan_id and status_type =0 and previous_status_id =loan_app_loan.marginalization_bucket_id and lalsh.status_id>loan_app_loan.marginalization_bucket_id and day <= new_lms_installmentextension.partial_accrual_date and status_id not in (6,7,8,12,13,14,15,16) order by id desc limit 1)","marginalization_history");

    partialAccrualQuery->setOrderBy("loan_app_loan.id");

    return partialAccrualQuery;
}
PSQLJoinQueryIterator* AccrualInterest::accrual_agg(QueryExtraFeilds * query_fields)
{
    PSQLJoinQueryIterator * accrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});

    accrualQuery->addExtraFromField("(select day from loan_app_loanstatushistroy lalsh  where loan_app_loan.id=lalsh.loan_id and status_type =0 and previous_status_id =loan_app_loan.marginalization_bucket_id and lalsh.status_id>loan_app_loan.marginalization_bucket_id and day <= new_lms_installmentextension.partial_accrual_date and status_id not in (6,7,8,12,13,14,15,16) order by id desc limit 1)","marginalization_history");

    accrualQuery->filter(
        ANDOperator (
            // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::INTEREST_ACCRUAL-1),
            new UnaryOperator ("loan_app_loan.id" , ne, "14312"),

            new OROperator (
                new UnaryOperator("new_lms_installmentextension.accrual_date", lte, query_fields->closure_date_string),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.payment_status", eq, 1),
                    new UnaryOperator("new_lms_installmentextension.accrual_date", gt, query_fields->closure_date_string)
                )
            ),
            new UnaryOperator("new_lms_installmentextension.accrual_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_loan.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "8, 15, 16"),
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
            query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
        )
    );

    accrualQuery->setOrderBy("loan_app_loan.id");
    return accrualQuery;
}
PSQLJoinQueryIterator* AccrualInterest::settlement_accrual_agg(QueryExtraFeilds * query_fields)
{
         PSQLJoinQueryIterator * settlementAccrualQuery = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
        {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});
        
        settlementAccrualQuery->filter(
            ANDOperator (
                new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date::date", lte, query_fields->closure_date_string),
    
                // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_INTEREST_ACCRUAL-1),
                new UnaryOperator ("loan_app_loan.id" , ne, "14312"),



                new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_ledger_amount_id", isnull, "", true),
                new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
                new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_amount", ne, 0),
                new UnaryOperator("loan_app_loan.status_id", nin, "12, 13"),
                new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
                query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
            )
        );

        settlementAccrualQuery->setOrderBy("loan_app_loan.id");
        return settlementAccrualQuery;
}

PSQLJoinQueryIterator* AccrualInterest::aggregator(QueryExtraFeilds * query_fields, int _agg_number)
{
    AccrualInterest instance;

    if(_agg_number == 1)
        return instance.partial_accrual_agg(query_fields);
    else if (_agg_number == 2)
        return instance.accrual_agg(query_fields);
    else if (_agg_number == 3)
        return instance.settlement_accrual_agg(query_fields);

    return nullptr;
}

void AccrualInterest::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
    ANDOperator(
        new UnaryOperator ("loan_app_loan.id",ne,"14312"),
        new UnaryOperator ("loan_app_loan.closure_status",gte,0)
    ),
    {{"closure_status",to_string(ledger_status::SETTLEMENT_INTEREST_ACCRUAL)}}

    );
    psqlUpdateQuery.update(); 
} 