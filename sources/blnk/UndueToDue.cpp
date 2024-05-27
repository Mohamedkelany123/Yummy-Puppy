#include <UndueToDue.h>


UndueToDue::UndueToDue(map <string,PSQLAbstractORM *> * _orm, BDate _closing_day)
{
    lal_orm = ORMBL(loan_app_loan,_orm);
    lai_orm = ORM(loan_app_installment,_orm);
    nli_orm = ORM(new_lms_installmentextension,_orm);

    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orm);
    lsh_settle_paid_off_day = BDate(gorm->get("settled_paid_off_day"));
    lsh_settle_charge_off_day = BDate(gorm->get("settled_charge_off_day_status"));
    
    closing_day = _closing_day;
    template_id = 10;
    partial_settle_status = 14;
    settle_charge_off_status = 15;
}


BDate UndueToDue::get_lsh_settle_paid_off_day(){return lsh_settle_paid_off_day;}
BDate UndueToDue::get_lsh_settle_charge_off_day(){return lsh_settle_charge_off_day;}
loan_app_loan_bl_orm* UndueToDue::get_loan_app_loan(){return lal_orm;}
loan_app_installment_primitive_orm* UndueToDue::get_loan_app_installment(){return lai_orm;}
new_lms_installmentextension_primitive_orm *UndueToDue::get_new_lms_installment_extention(){return nli_orm;}
BDate UndueToDue::get_closing_day(){return closing_day;}
int UndueToDue::get_partial_settle_status(){return partial_settle_status;}
int UndueToDue::get_settle_charge_off_status(){return settle_charge_off_status;}

LedgerAmount * UndueToDue::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    lg->setInstallmentId(lai_orm->get_id());

    return lg;
}

void UndueToDue::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    // ledgerClosureService->addHandler("Interest income becoming due", UndueToDue::_get_installment_insterest);
    ledgerClosureService->addHandler("Loan principal becoming due", UndueToDue::_get_installment_principal);
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


    if (nli_orm->get_undue_to_due_interest_ledger_amount_id() == 0)
    {
        cout << "------------------------------------------\nStatus_id =" << lal_orm->get_status_id() << "\n";
        if((lal_orm->get_status_id()) == (((UndueToDue*)undueToDue)->get_settle_charge_off_status() || ((UndueToDue*)undueToDue)->get_partial_settle_status()))
        {
            if(lsh_settle_charge_off_day.getDateString() == "")
            {   
                cout << "1111111111111111111111111111" << endl;
                la->setAmount(0);
                return la;
                // return NULL;
            }
            if((lsh_settle_charge_off_day() <  lai_undue_to_due_date()) && (nli_orm->get_is_interest_paid() == false))
            {
                if(nli_orm->get_is_extra_interest_paid() == true)
                {
                    cout << "22222222222222222222222" << endl;
                    la->setAmount(ROUND(nli_orm->get_first_installment_interest_adjustment()));
                    return la;
                }else{
                    cout << "33333333333333333333" << endl;
                    la->setAmount(0);
                    return la;
                    // return NULL;
                }
            }else if ((lsh_settle_charge_off_day() < lai_undue_to_due_date()) && (nli_orm->get_is_interest_paid() == true))
            {
                if(nli_orm->get_is_extra_interest_paid() == true)
                {
                    cout << "4444444444444444444444" << endl;
                    la->setAmount(ROUND(nli_orm->get_first_installment_interest_adjustment() + lai_orm->get_interest_expected()));
                    return la;
                }
                cout << "555555555555555555" << endl;
                la->setAmount(lai_orm->get_interest_expected());
                return la;
            }
        }
    }
    if((lsh_settle_paid_off_day.getDateString() == "") || (nli_orm->get_payment_status() == 1) && (lsh_settle_paid_off_day() < closing_day()) && (lsh_settle_paid_off_day() < lai_undue_to_due_date())){
        cout << "666666666666666" << endl;
        la->setAmount(0);
        return la;
        // return NULL;
    }
    
    if(lai_undue_to_due_date() <= closing_day()){
        cout << "77777777777777" << endl;
        la->setAmount(ROUND(lai_orm->get_interest_expected() && nli_orm->get_first_installment_interest_adjustment()));
        return la;
    }else{
        cout << "8888888888888888" << endl;
        la->setAmount(0);
        return la;
        // return NULL;
    }
    return la;
}


LedgerAmount * UndueToDue::_get_installment_principal(LedgerClosureStep *undueToDue)
{  

    // new_lms_installmentextension_primitive_orm* nli_orm = ((UndueToDue*)undueToDue)->get_new_lms_installment_extention();
    // loan_app_installment_primitive_orm* lai_orm = ((UndueToDue*)undueToDue)->get_loan_app_installment();
    cout << "HIIIIIIIIIIIIIIIIIIII" << endl;
    LedgerAmount * la = ((UndueToDue*)undueToDue)->_init_ledger_amount();
    // BDate lsh_settle_paid_off_day =  ((UndueToDue*)undueToDue)->get_lsh_settle_paid_off_day();
    // BDate closing_day = ((UndueToDue*)undueToDue)->get_closing_day(); 
    // BDate lai_undue_to_due_date(BDate(nli_orm->get_undue_to_due_date()));

    // if ((nli_orm->get_undue_to_due_interest_ledger_amount_id() != 0) || ( (lsh_settle_paid_off_day.getDateString() != "")  &&  (nli_orm->get_payment_status() == 1) && (lsh_settle_paid_off_day() <= closing_day()) && (lsh_settle_paid_off_day() < lai_undue_to_due_date())))
    // {
    //     //Todo:: What should we do if we set the amount to 0
    //     la->setAmount(0);
    //     return la;
    //     // return NULL;
    // }else{
    //     // cout << "AMOUNT:" << ROUND(lai_orm->get_principal_expected()) << endl;
    //     la->setAmount(ROUND(lai_orm->get_principal_expected()));
    //     return la;
    // }

    return la;
}


UndueToDue::~UndueToDue(){}
