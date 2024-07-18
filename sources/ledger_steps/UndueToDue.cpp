#include <UndueToDue.h>
UndueToDue::UndueToDue()
{
        lal_orm = nullptr;
        lai_orm = nullptr;
        nli_orm = nullptr;

        template_id = -1;
        closing_day = BDate("");
        lsh_settle_paid_off_day = BDate("");
        lsh_settle_charge_off_day = BDate("");
        partial_settle_status = -1;
        settle_charge_off_status = -1;
        undue_to_due_amount_id = -1;
        undue_to_due_interest_amount_id = -1;
        undue_to_due_extra_interest_amount_id = -1;

        undue_to_due_amount = -1;
        undue_to_due_interest_amount = -1;
        undue_to_due_extra_interest_amount = -1;

        ledger_closure_service_type = -1;
}


UndueToDue::UndueToDue(map <string,PSQLAbstractORM *> * _orm, BDate _closing_day, int __ledger_closure_service_type)
{
    lal_orm = ORMBL(loan_app_loan,_orm);
    lai_orm = ORM(loan_app_installment,_orm);
    nli_orm = ORM(new_lms_installmentextension,_orm);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orm);

    lsh_settle_paid_off_day = BDate(gorm->get("settled_paid_off_day"));
    lsh_settle_charge_off_day = BDate(gorm->get("settled_charge_off_day_status"));
    
    undue_to_due_amount_id = gorm->toInt("undue_to_due_amount_id");
    undue_to_due_amount = gorm->toFloat("undue_to_due_amount");

    undue_to_due_interest_amount_id = gorm->toInt("undue_to_due_interest_amount_id");
    undue_to_due_interest_amount =  gorm->toFloat("undue_to_due_interest_amount");
    
    undue_to_due_extra_interest_amount_id = gorm->toInt("undue_to_due_extra_interest_amount_id");
    undue_to_due_extra_interest_amount = gorm->toFloat("undue_to_due_extra_interest_amount");

    closing_day = _closing_day;
    template_id = 10;
    partial_settle_status = 14;
    settle_charge_off_status = 15;
    ledger_closure_service_type = __ledger_closure_service_type; 
}




LedgerAmount * UndueToDue::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    lg->setInstallmentId(lai_orm->get_id());

    return lg;
}

//Type 1 Both, 2 Interest Only, 3 LoanPrincipal.
void UndueToDue::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    if(ledger_closure_service_type == 1){
        ledgerClosureService->addHandler("Interest income becoming due", UndueToDue::_get_installment_insterest);
        ledgerClosureService->addHandler("Loan principal becoming due", UndueToDue::_get_installment_principal);
    }else if (ledger_closure_service_type == 2)
    {
        ledgerClosureService->addHandler("Interest income becoming due", UndueToDue::_get_installment_insterest);
    }else if (ledger_closure_service_type == 3)
    {
        ledgerClosureService->addHandler("Loan principal becoming due", UndueToDue::_get_installment_principal);
    }
    
    
}

LedgerAmount * UndueToDue::_get_installment_insterest(LedgerClosureStep *undueToDue)
{  
    LedgerAmount * la = ((UndueToDue*)undueToDue)->_init_ledger_amount();
    loan_app_installment_primitive_orm* lai_orm = ((UndueToDue*)undueToDue)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nli_orm = ((UndueToDue*)undueToDue)->get_new_lms_installment_extention();
    loan_app_loan_bl_orm* lal_orm = ((UndueToDue*)undueToDue)->get_loan_app_loan();


    BDate lsh_settle_paid_off_day =  ((UndueToDue*)undueToDue)->get_lsh_settle_paid_off_day();
    BDate lsh_settle_charge_off_day =  ((UndueToDue*)undueToDue)->get_lsh_settle_charge_off_day();
    BDate closing_day = ((UndueToDue*)undueToDue)->get_closing_day(); 

    BDate lai_undue_to_due_date(BDate(nli_orm->get_undue_to_due_date()));
    
    bool checkAmounts = ((UndueToDue*)undueToDue)->checkAmounts();
    if(checkAmounts)
    {
        // cout << "LOLOLOLOLLOLOLOLOLOLOLOLOLO" << endl;
        la->setAmount(0);
        return la;
    }        
    if (nli_orm->get_undue_to_due_interest_ledger_amount_id() == 0)
    {
        if((lal_orm->get_status_id()) == (((UndueToDue*)undueToDue)->get_settle_charge_off_status() || ((UndueToDue*)undueToDue)->get_partial_settle_status()))
        {
            if(lsh_settle_charge_off_day.getDateString() != "")
            {   
                if((lsh_settle_charge_off_day() <  lai_undue_to_due_date()) && (nli_orm->get_is_interest_paid() == false))
                {
                    if(nli_orm->get_is_extra_interest_paid() == true && (((UndueToDue*)undueToDue)->get_undue_to_due_extra_interest_amount_id() == 0))
                    {
                        // cout << "22222222222222222222222" << endl;
                        la->setAmount(ROUND(nli_orm->get_first_installment_interest_adjustment()));
                        return la;
                    }else{
                        // cout << "33333333333333333333" << endl;
                        la->setAmount(0);
                        return la;
                    }
                }else if ((lsh_settle_charge_off_day() < lai_undue_to_due_date()) && (nli_orm->get_is_interest_paid() == true))
                {
                    if(nli_orm->get_is_extra_interest_paid() == true && (((UndueToDue*)undueToDue)->get_undue_to_due_extra_interest_amount_id() == 0))
                    {
                        // cout << "4444444444444444444444" << endl;
                        la->setAmount(ROUND((nli_orm->get_first_installment_interest_adjustment() + lai_orm->get_interest_expected())));
                        return la;
                    }
                    // cout << "555555555555555555" << endl;
                    la->setAmount(lai_orm->get_interest_expected());
                    return la;
                }
            }
        }
        if(lsh_settle_paid_off_day.getDateString() != "")
        {
            if((nli_orm->get_payment_status() == 1) && (lsh_settle_paid_off_day() <= closing_day()) && (lsh_settle_paid_off_day() < lai_undue_to_due_date())){
                // cout << "666666666666666" << endl;
                la->setAmount(0);
                return la;
            }
        }

        if(lai_undue_to_due_date() <= closing_day()){
            // cout << "77777777777777" << endl;
            la->setAmount(ROUND((lai_orm->get_interest_expected() + nli_orm->get_first_installment_interest_adjustment())));
            return la;
        }else if (nli_orm->get_is_extra_interest_paid() == true && (((UndueToDue*)undueToDue)->get_undue_to_due_extra_interest_amount_id() == 0)  && (nli_orm->get_extra_interest_paid_at() < lai_orm->get_day()))
        {
            // cout << "88888888" << endl;
            la->setAmount(ROUND(nli_orm->get_first_installment_interest_adjustment()));
            return la;
        }else{
            // cout << "9999999999" << endl;
            la->setAmount(0);
            return la;
        }
    }else{
        // cout << "Elseeeeeeeeeeeeeeeee" << endl;
        if (nli_orm->get_is_extra_interest_paid() == true && (((UndueToDue*)undueToDue)->get_undue_to_due_extra_interest_amount_id() == 0)  && (nli_orm->get_extra_interest_paid_at() < lai_orm->get_day()))
        {
            // cout << "1000000000" << endl;
            la->setAmount(ROUND(nli_orm->get_first_installment_interest_adjustment()));
            return la;
        }
    }
    // cout << "1222222222222" << endl;
    la->setAmount(0);
    return la;
}


LedgerAmount * UndueToDue::_get_installment_principal(LedgerClosureStep *undueToDue)
{  
    new_lms_installmentextension_primitive_orm* nli_orm = ((UndueToDue*)undueToDue)->get_new_lms_installment_extention();
    loan_app_installment_primitive_orm* lai_orm = ((UndueToDue*)undueToDue)->get_loan_app_installment();
    LedgerAmount * la = ((UndueToDue*)undueToDue)->_init_ledger_amount();
    BDate lsh_settle_paid_off_day =  ((UndueToDue*)undueToDue)->get_lsh_settle_paid_off_day();
    BDate closing_day = ((UndueToDue*)undueToDue)->get_closing_day(); 
    BDate lai_undue_to_due_date(BDate(nli_orm->get_undue_to_due_date()));

    bool checkAmounts = ((UndueToDue*)undueToDue)->checkAmounts();
    if(checkAmounts)
    {
        // cout << "LOLOLOLOLLOLOLOLOLOLOLOLOLO" << endl;
        la->setAmount(0);
        return la;
    }        

    // cout <<"Loan status history day: " << lsh_settle_paid_off_day.getDateString() << endl;
    if ((nli_orm->get_undue_to_due_ledger_amount_id() != 0) || ( (lsh_settle_paid_off_day.getDateString() != "")  &&  (nli_orm->get_payment_status() == 1) && (lsh_settle_paid_off_day() <= closing_day()) && (lsh_settle_paid_off_day() < lai_undue_to_due_date())))
    {
        // cout << "P1111111111111111" <<endl;
        la->setAmount(0);
        return la;
    }else{
        // cout << "P222222222222222" <<endl;
        la->setAmount(ROUND(lai_orm->get_principal_expected()));
        return la;
    }

    return la;
}

bool UndueToDue::checkAmounts(){
    if ((undue_to_due_amount_id != 0) && (undue_to_due_interest_amount_id != 0))
    {
        nli_orm->set_undue_to_due_ledger_amount_id(undue_to_due_amount_id);
        if (abs(undue_to_due_interest_amount) >= float(lai_orm->get_interest_expected())){
            nli_orm->set_undue_to_due_interest_ledger_amount_id(undue_to_due_interest_amount_id); 
        }
        if (abs(undue_to_due_interest_amount) > float(lai_orm->get_interest_expected()))
            nli_orm->set_undue_to_due_extra_interest_ledger_amount_id(undue_to_due_interest_amount_id);
        
        else if (undue_to_due_extra_interest_amount_id != 0)
            nli_orm->set_undue_to_due_extra_interest_ledger_amount_id(undue_to_due_extra_interest_amount_id);
        

        if ( (nli_orm->get_undue_to_due_extra_interest_ledger_amount_id() ||  nli_orm->get_first_installment_interest_adjustment() == 0) && (nli_orm->get_undue_to_due_interest_ledger_amount_id() != 0) && ((nli_orm->get_undue_to_due_interest_ledger_amount_id() != 0) || (lai_orm->get_interest_expected() == 0)) )
            return true;

        if(undue_to_due_extra_interest_amount_id != 0)
            nli_orm->set_undue_to_due_extra_interest_ledger_amount_id(undue_to_due_extra_interest_amount_id);
        if (undue_to_due_amount_id != 0 )
            nli_orm->set_undue_to_due_ledger_amount_id(undue_to_due_amount_id);
        if((undue_to_due_interest_amount_id != 0)  && (abs(undue_to_due_interest_amount) >= float(lai_orm->get_interest_expected()))){
            nli_orm->set_undue_to_due_interest_ledger_amount_id(undue_to_due_interest_amount_id);
            if (abs(undue_to_due_interest_amount) > float(lai_orm->get_interest_expected()))
                nli_orm->set_undue_to_due_extra_interest_ledger_amount_id(undue_to_due_interest_amount_id);
        }
    }
    return false;
}

void UndueToDue::stampORMs(map<string, LedgerCompositLeg *> *leg_amounts, bool extra_interest){
    lal_orm->set_lms_closure_status(ledger_status::LEDGER_UNDUE_TO_DUE);

    if ((*leg_amounts).find("Interest income becoming due") != (*leg_amounts).end())
    {
        if(extra_interest){
            LedgerCompositLeg * leg_interest = (*leg_amounts)["Interest income becoming due"];
            nli_orm->setUpdateRefernce("undue_to_due_extra_interest_ledger_amount_id",leg_interest->getLedgerCompositeLeg()->first);
        }else{
            LedgerCompositLeg * leg_interest = (*leg_amounts)["Interest income becoming due"];
            nli_orm->setUpdateRefernce("undue_to_due_interest_ledger_amount_id",leg_interest->getLedgerCompositeLeg()->first);
        }
    } 

    if ((*leg_amounts).find("Loan principal becoming due") != (*leg_amounts).end())
    {
        LedgerCompositLeg * leg_principal = (*leg_amounts)["Loan principal becoming due"];
        nli_orm->setUpdateRefernce("undue_to_due_ledger_amount_id",leg_principal->getLedgerCompositeLeg()->first);
    }
}


PSQLJoinQueryIterator* UndueToDue::installments_becoming_due_agg(string _closure_date_string)
{
     PSQLJoinQueryIterator * installments_becoming_due_iterator = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_bl_orm("main"), new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
        {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});


        installments_becoming_due_iterator->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,_closure_date_string),
                // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::LEDGER_UNDUE_TO_DUE-1)),
                new OROperator (
                    new UnaryOperator ("new_lms_installmentextension.undue_to_due_ledger_amount_id ",isnull,"",true),
                    new ANDOperator (
                        new UnaryOperator ("new_lms_installmentextension.undue_to_due_interest_ledger_amount_id ",isnull,"",true),
                        new UnaryOperator ("loan_app_installment.interest_expected",ne,"0")
                    )
                ),
                new UnaryOperator ("loan_app_loan.status_id",nin,"12,13"),
                new UnaryOperator ("new_lms_installmentextension.status_id",nin,"8,15,16,12,13"),

                new UnaryOperator ("loan_app_loan.id",ne,"14312")
                 
            )
        );
        
        installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=8 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id asc limit 1)","settled_paid_off_day");
        installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=15 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id asc limit 1)","settled_charge_off_day_status");
        
        installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 26 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_amount_id");
        installments_becoming_due_iterator->addExtraFromField("(select la.amount from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 26 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_amount");
        
        installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_interest_amount_id");
        installments_becoming_due_iterator->addExtraFromField("(select la.amount from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_interest_amount");
        
        installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null and amount = new_lms_installmentextension.first_installment_interest_adjustment order by la.id asc limit 1)","undue_to_due_extra_interest_amount_id");
        installments_becoming_due_iterator->addExtraFromField("(select la.amount from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null and amount = new_lms_installmentextension.first_installment_interest_adjustment order by la.id asc limit 1)","undue_to_due_extra_interest_amount");
        
        
        return installments_becoming_due_iterator;
}
PSQLJoinQueryIterator* UndueToDue::sticky_nstallments_becoming_due_agg(string _closure_date_string)
{
       PSQLJoinQueryIterator * sticky_installments_becoming_due_iterator = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_bl_orm("main"), new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
        {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        
        sticky_installments_becoming_due_iterator->filter(
            ANDOperator 
            (
                new OROperator (
                    // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::LEDGER_UNDUE_TO_DUE-1)),
                    new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",gt,_closure_date_string),
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,true),
                        new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",gt,"interest_paid_at", true)
                    ),
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.is_extra_interest_paid",eq,true),
                        new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",gt,"extra_interest_paid_at", true)
                    )
                ),

                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"2,4"),
                new UnaryOperator ("new_lms_installmentextension.is_principal_paid",eq,true),
                new UnaryOperator ("new_lms_installmentextension.principal_paid_at::date",lte,_closure_date_string),


                new OROperator(
                    new UnaryOperator ("new_lms_installmentextension.undue_to_due_ledger_amount_id",isnull,"", true),
                    
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.settlement_accrual_interest_amount",gt,"0"),
                        new UnaryOperator ("new_lms_installmentextension.undue_to_due_interest_ledger_amount_id",isnull,"",true)
                    ),
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.first_installment_interest_adjustment",gt,"0"),
                        new UnaryOperator ("new_lms_installmentextension.undue_to_due_extra_interest_ledger_amount_id",isnull,"",true),
                        new UnaryOperator ("new_lms_installmentextension.is_extra_interest_paid",eq,true)
                    )
                ),

                new UnaryOperator ("loan_app_loan.status_id",nin,"12,13"),



                new OROperator(
                    new UnaryOperator ("new_lms_installmentextension.status_id",nin,"8,12,13"),
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.status_id",ne,"16"),
                        new UnaryOperator ("new_lms_installmentextension.payment_status",ne,"3")
                    )                
                ),

                new UnaryOperator ("loan_app_loan.id",ne,"14312") 



            )
        );

        sticky_installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=8 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id asc limit 1)","settled_paid_off_day");
        sticky_installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=15 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id asc limit 1)","settled_charge_off_day_status");
        
        sticky_installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 26 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_amount_id");
        sticky_installments_becoming_due_iterator->addExtraFromField("(select la.amount from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 26 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_amount");
        
        sticky_installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_interest_amount_id");
        sticky_installments_becoming_due_iterator->addExtraFromField("(select la.amount from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null order by la.id asc limit 1)","undue_to_due_interest_amount");
        
        sticky_installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null and amount = new_lms_installmentextension.first_installment_interest_adjustment order by la.id asc limit 1)","undue_to_due_extra_interest_amount_id");
        sticky_installments_becoming_due_iterator->addExtraFromField("(select la.amount from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null and amount = new_lms_installmentextension.first_installment_interest_adjustment order by la.id asc limit 1)","undue_to_due_extra_interest_amount");
        
        return sticky_installments_becoming_due_iterator;
}
PSQLJoinQueryIterator* UndueToDue::aggregator(string _closure_date_string, int _agg_number)
{
    UndueToDue undueToDue;

    if(_agg_number == 1)
        return undueToDue.installments_becoming_due_agg(_closure_date_string);
    else if(_agg_number == 2)
        return undueToDue.sticky_nstallments_becoming_due_agg(_closure_date_string);

    return nullptr;
}   
void UndueToDue::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::LEDGER_UNDUE_TO_DUE)}}

        );
    psqlUpdateQuery.update(); 
}



UndueToDue::~UndueToDue(){}


BDate UndueToDue::get_lsh_settle_paid_off_day(){return lsh_settle_paid_off_day;}
BDate UndueToDue::get_lsh_settle_charge_off_day(){return lsh_settle_charge_off_day;}
loan_app_loan_bl_orm* UndueToDue::get_loan_app_loan(){return lal_orm;}
loan_app_installment_primitive_orm* UndueToDue::get_loan_app_installment(){return lai_orm;}
new_lms_installmentextension_primitive_orm *UndueToDue::get_new_lms_installment_extention(){return nli_orm;}
BDate UndueToDue::get_closing_day(){return closing_day;}
int UndueToDue::get_partial_settle_status(){return partial_settle_status;}
int UndueToDue::get_settle_charge_off_status(){return settle_charge_off_status;}
int UndueToDue::get_undue_to_due_amount_id(){return undue_to_due_amount_id;}
float UndueToDue::get_undue_to_due_amount(){return undue_to_due_amount;}
int UndueToDue::get_undue_to_due_interest_amount_id(){return undue_to_due_interest_amount_id;}
float UndueToDue::get_undue_to_due_interest_amount(){return undue_to_due_interest_amount;}
int UndueToDue::get_undue_to_due_extra_interest_amount_id(){return undue_to_due_extra_interest_amount_id;}
float UndueToDue::get_undue_to_due_extra_interest_amount(){return undue_to_due_extra_interest_amount;}