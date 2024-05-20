#include "AccrualInterest.h"

AccrualInterest::AccrualInterest(map<string, PSQLAbstractORM *> *_orms, int _accrual_type)
{
    loan_app_loan_primitive_orm * _lal_orm = ORMBL(loan_app_loan,_orms);
    loan_app_installment_primitive_orm * _lai_orm = ORM(loan_app_installment,_orms);
    new_lms_installmentextension_primitive_orm * _nli_orm = ORM(new_lms_installmentextension,_orms);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    string _marginalization_history = gorm->get("_marginalization_history");
    string _last_order_date = gorm->get("last_order_date");
    string _settled_history = gorm->get("settled_history");
    lal_orm = _lal_orm;  
    lai_orm =_lai_orm;
    nli_orm = _nli_orm;
    marginalization_history = _marginalization_history;
    last_order_date = _last_order_date;
    settled_history = _settled_history;
    accrual_type = _accrual_type;
}

AccrualInterest::~AccrualInterest()
{
}

void AccrualInterest::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("", _get_marginalization_interest);
    ledgerClosureService->addHandler("", _get_accrued_interest);

}

LedgerAmount *AccrualInterest::_init_ledger_amount()
{
    return nullptr;
}

void AccrualInterest::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

void AccrualInterest::set_loan_app_installment(loan_app_installment_primitive_orm *_lai_orm)
{
    lai_orm = _lai_orm;
}

void AccrualInterest::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm *_nli_orm)
{
    nli_orm = _nli_orm;
}

void AccrualInterest::set_marginalization_history(string _marginalization_history)
{
    marginalization_history = _marginalization_history;
}

void AccrualInterest::set_last_order_date(string _last_order_date)
{
    last_order_date = _last_order_date;
}

void AccrualInterest::set_settled_history(string _settled_history)
{
    settled_history = _settled_history;
}

void AccrualInterest::set_accrual_type(int _accrual_type)
{
    accrual_type = _accrual_type;
}

loan_app_loan_primitive_orm *AccrualInterest::get_loan_app_loan()
{
    return lal_orm;
}

new_lms_installmentextension_primitive_orm* AccrualInterest::get_new_lms_installmentextension()
{
    return nli_orm;
}

const string AccrualInterest::get_marginalization_history()
{
    return marginalization_history;
}

const string AccrualInterest::get_last_order_date()
{
    return last_order_date;
}

const string AccrualInterest::get_settled_history()
{
    return settled_history;
}

const int AccrualInterest::get_accrual_type()
{
    return accrual_type;
}

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
    if (accrual_type == 3) {
        return 0;
    }

    if (accrual_type == 2 && partialMarginalizationLedgerAmountId == 0) {
        return 0;
    }

    if (accrual_type != 2 && marginalizationLedgerAmountId == 0) {
        return 0;
    }
    bool is_marginalized = nli_orm->get_is_marginalized();
    bool is_partially_marginalized = nli_orm->get_is_partially_marginalized();
    bool is_interest_paid = nli_orm->get_is_interest_paid();
    BDate interest_paid_at = BDate(nli_orm->get_interest_paid_at());
    BDate marginalization_date = BDate(nli_orm->get_marginalization_date());
    BDate history_bdate = BDate(history);
    BDate accrual_date = BDate(nli_orm->get_accrual_date());
    BDate partial_accrual_date = BDate(nli_orm->get_partial_accrual_date());
    BDate partial_marginalization = BDate(nli_orm->get_partial_marginalization_date());
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
    return nullptr;
}
