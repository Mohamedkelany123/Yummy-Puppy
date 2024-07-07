#include "OnboardingCommission.h"

// Constructors
OnboardingCommission::OnboardingCommission() {

}

OnboardingCommission::OnboardingCommission(map<string, PSQLAbstractORM *> *_orms) {
    cac_orm = ORM(crm_app_customer, _orms);
    ekyco_orm = ORM(ekyc_app_onboardingsession, _orms);
    PSQLGeneric_primitive_orm *gorm = ORM(PSQLGeneric, _orms);
    merchant_id = gorm->toInt("merchant_id");
    commission_ledger_entry_id = gorm->toInt("commission_ledger_entry_id");
}

//SETTERS
void OnboardingCommission::set_commission_ledger_entry_id(int _commission_ledger_entry_id){commission_ledger_entry_id = _commission_ledger_entry_id;}
void OnboardingCommission::set_merchant_id(int _merchant_id){merchant_id = _merchant_id;}
void OnboardingCommission::set_crm_app_customer(crm_app_customer_primitive_orm *_cac_orm){cac_orm = _cac_orm;}
void OnboardingCommission::set_ekyc_app_onboardingsession(ekyc_app_onboardingsession_primitive_orm *_ekyco_orm){ekyco_orm = _ekyco_orm;}

//GETTERS
int OnboardingCommission::get_commission_ledger_entry_id(){return commission_ledger_entry_id;}
int OnboardingCommission::get_merchant_id(){return merchant_id;}
crm_app_customer_primitive_orm *OnboardingCommission::get_crm_app_customer(){return cac_orm;}
ekyc_app_onboardingsession_primitive_orm *OnboardingCommission::get_ekyc_app_onboardingsession(){return ekyco_orm;}

// Destructor
OnboardingCommission::~OnboardingCommission() {}

LedgerAmount *OnboardingCommission::_init_ledger_amount() {
    LedgerAmount * lg = new LedgerAmount();

    //customer cashier merchant
    lg->setCustomerId(cac_orm->get_id());
    lg->setCashierId(ekyco_orm->get_merchant_staff_id());
    lg->setMerchantId(merchant_id);

    return lg;
}

void OnboardingCommission::stampORMs(ledger_entry_primitive_orm* entry)
{
    ekyco_orm->set_onboarding_commission_ledger_stamped(onboardingCommissionLedgerStamp::STAMPED);
    ekyco_orm->setUpdateRefernce("onboarding_commission_ledger_entry_id",entry);
}

LedgerAmount *OnboardingCommission::_calculate_merchant_commission(LedgerClosureStep *onboardingCommission) {
    
    LedgerAmount * la = ((OnboardingCommission*)onboardingCommission)->_init_ledger_amount();

    bool checkAmounts = ((OnboardingCommission*)onboardingCommission)->checkAmounts();
    if(checkAmounts)
    {
        cout << "LOLOLOLOLLOLOLOLOLOLOLOLOLO" << endl;
        la->setAmount(0);
        return la;
    }        

    crm_app_customer_primitive_orm* cac_orm = ((OnboardingCommission*)onboardingCommission)->get_crm_app_customer();
    la->setAmount(cac_orm->get_onboarding_commission());

    return la;
}

bool OnboardingCommission::checkAmounts()
{   
    if (commission_ledger_entry_id != 0)
    {
        ekyco_orm->set_onboarding_commission_ledger_stamped(onboardingCommissionLedgerStamp::STAMPED);
        ekyco_orm->set_onboarding_commission_ledger_entry_id(commission_ledger_entry_id);
        return true;
    }
    return false;
}

void OnboardingCommission::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{   
    ledgerClosureService->addHandler("Onboarding Commission", OnboardingCommission::_calculate_merchant_commission);
}

PSQLJoinQueryIterator *OnboardingCommission::aggregator(string _closure_date_string, int _agg_number)
{
    PSQLJoinQueryIterator *onboarding_commission_iterator = new PSQLJoinQueryIterator("main",
        {new crm_app_customer_primitive_orm("main"), new ekyc_app_onboardingsession_primitive_orm("main")},
        {{{"crm_app_customer", "national_id"}, {"ekyc_app_onboardingsession", "national_id"}}});
    onboarding_commission_iterator->filter(
        ANDOperator(
            new UnaryOperator("crm_app_customer.onboarding_commission", gt, "0"),
            new UnaryOperator("crm_app_customer.state", in, "3,5,7"),
            new UnaryOperator("ekyc_app_onboardingsession.onboarding_step", eq, "4"),
            
            // new UnaryOperator("ekyc_app_onboardingsession.onboarding_commission_ledger_stamped", eq, to_string(onboardingCommissionLedgerStamp::NOT_STAMPED)),
            new UnaryOperator ("loan_app_loan.id" , ne, "14312"),

            new UnaryOperator("ekyc_app_onboardingsession.onboarding_commission_ledger_entry_date", lte, _closure_date_string),
            new UnaryOperator("ekyc_app_onboardingsession.onboarding_commission_ledger_entry_id", isnull, "", true)
        )
    );

    onboarding_commission_iterator->addExtraFromField("(select merchant_id from crm_app_merchantstaff where crm_app_merchantstaff.user_ptr_id = ekyc_app_onboardingsession.merchant_staff_id)", "merchant_id");
    onboarding_commission_iterator->addExtraFromField("(select la.entry_id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where customer_id = crm_app_customer.id and le.template_id = 68 order by la.id desc limit 1)", "commission_ledger_entry_id");
    
    return onboarding_commission_iterator;
}

void OnboardingCommission::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
    ANDOperator(
        new UnaryOperator ("loan_app_loan.id",ne,"14312"),
        new UnaryOperator ("loan_app_loan.closure_status",gte,0)
    ),
    {{"closure_status",to_string(ledger_status::ONBOARDING_COMMISSION)}}

    );
    psqlUpdateQuery.update(); 
}
