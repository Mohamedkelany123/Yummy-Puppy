#include <InitialLoanInterestAccrual.h>


InitialLoanInterestAccrual::InitialLoanInterestAccrual(){
    lal_orm = nullptr;
    last_installment_principal_paid_at = BDate("");
    is_first_date = false;
}

InitialLoanInterestAccrual::InitialLoanInterestAccrual(loan_app_loan_primitive_orm * _lal_orm,bool _is_first_date){
    lal_orm = _lal_orm;
    is_first_date =_is_first_date;
    
    if(is_first_date)
        last_installment_principal_paid_at = BDate(lal_orm->getExtra("last_installment_principal_paid_at"));
    else last_installment_principal_paid_at = BDate("");
}


LedgerAmount*  InitialLoanInterestAccrual::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}


LedgerAmount * InitialLoanInterestAccrual::_get_accrued_interest(LedgerClosureStep *initialLoanInterestAccrual){
    loan_app_loan_primitive_orm* loan_orm = ((InitialLoanInterestAccrual*)initialLoanInterestAccrual)->get_loan_app_loan();
    LedgerAmount * ledgerAmount = ((InitialLoanInterestAccrual*)initialLoanInterestAccrual)->_init_ledger_amount();
    bool is_first_date = ((InitialLoanInterestAccrual*)initialLoanInterestAccrual)->get_is_first_date();

    if(is_first_date)
    {    
        ledgerAmount->setAmount(loan_orm->get_first_accrual_adjustment_amount());
        return ledgerAmount;
    }
    else{
        ledgerAmount->setAmount(loan_orm->get_second_accrual_adjustment_amount());
        return ledgerAmount;
    } 
}


void InitialLoanInterestAccrual::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Interest income accrual (undue)", InitialLoanInterestAccrual::_get_accrued_interest);
}

void InitialLoanInterestAccrual::set_is_first_date(int _is_first_date){is_first_date = _is_first_date;}
void InitialLoanInterestAccrual::set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm){lal_orm = _lal_orm;}
void InitialLoanInterestAccrual::set_is_first_date(BDate _last_installment_principal_paid_at){last_installment_principal_paid_at=_last_installment_principal_paid_at;}



loan_app_loan_primitive_orm* InitialLoanInterestAccrual::InitialLoanInterestAccrual::get_loan_app_loan(){return lal_orm;}
bool InitialLoanInterestAccrual::get_is_first_date(){return is_first_date;}
BDate InitialLoanInterestAccrual::get_last_installment_principal_paid_at(){return last_installment_principal_paid_at;}


loan_app_loan_primitive_orm_iterator* InitialLoanInterestAccrual::loans_to_get_first_accrual_agg(QueryExtraFeilds * query_fields){
    loan_app_loan_primitive_orm_iterator * loans_to_get_first_accrual_iterator = new loan_app_loan_primitive_orm_iterator("main");

    loans_to_get_first_accrual_iterator->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.first_accrual_adjustment_date",lte,query_fields->closure_date_string),
            
            // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::FIRST_INCOME_ACCRUAL-1)),
            new UnaryOperator ("loan_app_loan.id" , ne, "14312"),
            
            new UnaryOperator ("loan_app_loan.first_accrual_adjustment_ledger_entry_id",isnull,"", true),
            new UnaryOperator ("loan_app_loan.yearly_interest",ne,"0"),
            new UnaryOperator ("loan_app_loan.status_id",nin,"7,12,13"),
            query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
            query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
        )
    );
        
    loans_to_get_first_accrual_iterator->addExtraFromField("(select nli.principal_paid_at from new_lms_installmentextension nli inner join loan_app_installment lai on lai.id = nli.installment_ptr_id where lai.loan_id = loan_app_loan.id and lai.\"period\" = loan_app_loan.num_periods order by lai.id desc limit 1 )","last_installment_principal_paid_at");
        
    return loans_to_get_first_accrual_iterator;
}
loan_app_loan_primitive_orm_iterator* InitialLoanInterestAccrual::loans_to_get_second_accrual_agg(QueryExtraFeilds * query_fields){
    loan_app_loan_primitive_orm_iterator * loans_to_get_first_accrual_iterator = new loan_app_loan_primitive_orm_iterator("main");

    loans_to_get_first_accrual_iterator->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.second_accrual_adjustment_date",lte,query_fields->closure_date_string),
            
            // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::FIRST_INCOME_ACCRUAL-1)),
            new UnaryOperator ("loan_app_loan.id" , ne, "14312"),

            new UnaryOperator ("loan_app_loan.second_accrual_adjustment_ledger_entry_id",isnull,"", true),
            new UnaryOperator ("loan_app_loan.yearly_interest",ne,"0"),
            new UnaryOperator ("loan_app_loan.status_id",nin,"7,12,13"),
            query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
            query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
        )
    );
        
    return loans_to_get_first_accrual_iterator;
}

loan_app_loan_primitive_orm_iterator* InitialLoanInterestAccrual::aggregator(QueryExtraFeilds * query_fields, int _agg_number){
    InitialLoanInterestAccrual initialLoanInterestAccrual;

    if (_agg_number == 1){
        return initialLoanInterestAccrual.loans_to_get_first_accrual_agg(query_fields);
    }else if(_agg_number == 2){
        return initialLoanInterestAccrual.loans_to_get_second_accrual_agg(query_fields);
    }

    return nullptr;
}
void InitialLoanInterestAccrual::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::FIRST_INCOME_ACCRUAL)}}

        );
    psqlUpdateQuery.update(); 
}

void InitialLoanInterestAccrual::stampORMs(ledger_entry_primitive_orm* entry){
    
    if(is_first_date)
        lal_orm->setUpdateRefernce("first_accrual_adjustment_ledger_entry_id", entry);
    else 
        lal_orm->setUpdateRefernce("second_accrual_adjustment_ledger_entry_id", entry);     
        
    lal_orm->set_lms_closure_status(ledger_status::FIRST_INCOME_ACCRUAL);
}


InitialLoanInterestAccrual::~InitialLoanInterestAccrual(){}
