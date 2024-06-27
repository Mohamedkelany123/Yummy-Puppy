#include <LongToShortTerm.h>

LongToShortTerm::LongToShortTerm(map <string,PSQLAbstractORM * > * _orms_list, float _percentage):LedgerClosureStep ()
{
    lal_orm = ORM(loan_app_loan,_orms_list);
    lai_orm = ORM(loan_app_installment, _orms_list);
    nli_orm = ORM(new_lms_installmentextension, _orms_list);
}

PSQLJoinQueryIterator* LongToShortTerm::aggregator(string _closure_date_string, int _agg_number=0){
    PSQLJoinQueryIterator * psqlJoinQuery = new PSQLJoinQueryIterator("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_ap_installment","loan_id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

    psqlJoinQuery->filter(
        ANDOperator
        (
            new UnaryOperator("new_lms_installmentextension.long_to_short_term_date",lte,_closure_date_string),
            new UnaryOperator("new_lms_installmentextension.short_term_ledger_amount",eq,NULL),
            new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::RECLASSIFY_LONG_TERM-1),
            new UnaryOperator("new_lms_installmentextension.is_long_term",eq, false),
            new OROperator(
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.long_to_short_term_date",lte,"new_lms_installmentextension.principal_paid_at"),
                    new UnaryOperator("new_lms_installmentextension.is_principal_paid",eq,true)
                ),
                    new UnaryOperator("new_lms_installmentextension.is_principal_paid",eq,false)
                
            ),
            new UnaryOperator("loan_app_loan.status",nin,{12,13})
        )
    );

    psqlJoinQuery->setOrderBy("loan_app_loan.id asc");

    return psqlJoinQuery;
}

LedgerAmount* LongToShortTerm::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setInstallmentId(lai_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}



void LongToShortTerm::update_step(){

        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::RECLASSIFY_LONG_TERM)}}
        );
        psqlUpdateQuery.update();   
}

void ::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Reclassifying long term loans as short term loans, if applicable", LongToShortTerm::_reclassify_inst_balance);
    ledgerClosureService->addHandler("Reclassifying long term notes payable to merchant as short term notes payable, if applicable",LongToShortTerm::_reclassify_merchant_balance);
}

void LongToShortTerm::stampORMs(ledger_entry_primitive_orm *entry, ledger_amount_primitive_orm *la_orm){
    if (la_orm != NULL)
    {
        lai_orm->setUpdateRefernce("short_term_ledger_amount_id", la_orm);
    }
    else cout << "ERROR in fetching first leg of the entry " << endl;
}

LedgerAmount * LongToShortTerm::_reclassify_inst_balance(LedgerClosureStep *longToShortTerm){
    LedgerAmount * la = ((LongToShortTerm*)longToShortTerm)->_init_ledger_amount();
    loan_app_installment_primitive_orm * installment = ((LongToShortTerm*)longToShortTerm)->get_loan_app_installment();
    float principal = installment->get_principal_expected();
    la->setAmount(principal);

    return la;
}

LedgerAmount * LongToShortTerm::_reclassify_merchant_balance(LedgerClosureStep *longToShortTerm){
    LedgerAmount * la = ((LongToShortTerm*)longToShortTerm)->_init_ledger_amount();
    la->setAmount(0.0);

    return la;
}

void LongToShortTerm::set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm){
    lal_orm = _lal_orm;
}
void LongToShortTerm::set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm){
    lai_orm = _lai_orm;
}
void LongToShortTerm::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nli_orm){
    nli_orm = _nli_orm;
}


LongToShortTerm::~LongToShortTerm(){}
