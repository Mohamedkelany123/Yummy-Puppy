#include <DueToOverdue.h>

PSQLJoinQueryIterator* DueToOverdue::installments_becoming_overdue_agg(string _closure_date_string)
{
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {
            new loan_app_loan_bl_orm("main"), 
            new loan_app_installment_primitive_orm("main"), 
            new new_lms_installmentextension_primitive_orm("main"),
            new new_lms_installmentlatefees_primitive_orm("main")
        },
        {
            {{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, 
            {{"loan_app_installment","id"}, {"new_lms_installmentextension", "installment_ptr_id"}}, 
            {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}
        }
    );

    psqlQueryJoin->filter(
        ANDOperator (
            new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,_closure_date_string),
            new UnaryOperator("new_lms_installmentlatefees.day", lte, _closure_date_string),
            new UnaryOperator("new_lms_installmentlatefees.accrual_ledger_amount_id", isnull, "", true),
            // new UnaryOperator("loan_app_loan.closure_status", eq, closure_status::DUE_TO_OVERDUE-1),
            new UnaryOperator("new_lms_installmentextension.due_to_overdue_date", lte, _closure_date_string),
            new UnaryOperator("new_lms_installmentextension.payment_status", nin, "1, 3, 6"),
            new OROperator (
                new UnaryOperator("new_lms_installmentlatefees.is_cancelled", eq, false),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentlatefees.is_cancelled", eq, true),
                    new UnaryOperator("new_lms_installmentlatefees.cancellation_date", gte, _closure_date_string)
                )
            )
        )
    );

    // psqlQueryJoin->addExtraFromField(
    //     "(select entry.entry_date as last_entry_date from loan_app_loan loan left join ledger_amount amount on amount.loan_id=loan_app_loan.id inner join ledger_entry entry on entry.id=amount.entry_id order by entry.entry_date desc limit 1)", 
    //     "last_entry_date"
    // );
    // psqlQueryJoin->addExtraFromField(
    //     "(select is_paid from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1)",
    //     "latefee_is_paid"
    // );
    // psqlQueryJoin->addExtraFromField(
    //     "select paid_at from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
    //     "latefee_paid_at"
    // );
    // psqlQueryJoin->addExtraFromField(
    //     "select is_cancelled from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
    //     "latefee_is_cancelled"
    // );
    // psqlQueryJoin->addExtraFromField(
    //     "select cancellation_date from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
    //     "latefee_cancellation_date"
    // );

    psqlQueryJoin->setOrderBy("loan_app_loan.id asc, new_lms_installmentlatefees.day asc, loan_app_installment.id asc");
    psqlQueryJoin->setAggregates({
        {"loan_app_loan",{"id", 1}},
        {"new_lms_installmentlatefee", {"day", 2}}
    });
        
    return psqlQueryJoin;
}

DueToOverdue::DueToOverdue()
{
}

DueToOverdue::DueToOverdue(loan_app_loan_primitive_orm *_lal_orm, loan_app_installment_primitive_orm* _lai_orm, new_lms_installmentextension_primitive_orm* _nlie_orm, new_lms_installmentlatefees_primitive_orm* _nlilf_orm, BDate _due_to_overdue_date)
{
    lal_orm = _lal_orm;
    lai_orm = _lai_orm;
    nlie_orm = _nlie_orm;
    nlilf_orm = _nlilf_orm;
    due_to_overdue_date = _due_to_overdue_date;
}

DueToOverdue::DueToOverdue(map<string, PSQLAbstractORM *> *_orms, BDate _closing_day, int _ledger_closure_service_type)
{
}

void DueToOverdue::set_loan_app_loan(loan_app_loan_bl_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

void DueToOverdue::set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm)
{
    lai_orm = _lai_orm;
}

void DueToOverdue::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nlie_orm) {
    nlie_orm = _nlie_orm;
}

void DueToOverdue::set_new_lms_installmentlatefees(new_lms_installmentlatefees_primitive_orm *_nlilf_orm)
{
    nlilf_orm = _nlilf_orm;
}

void DueToOverdue::set_template_id(int _template_id)
{
    template_id = _template_id;
}

void DueToOverdue::set_closing_day(BDate _closing_day)
{
    closing_day = _closing_day;
}

void DueToOverdue::set_due_to_overdue_date(BDate _due_to_overdue_date)
{
    due_to_overdue_date = _due_to_overdue_date;
}

loan_app_loan_primitive_orm *DueToOverdue::get_loan_app_loan()
{
    return lal_orm;
}

loan_app_installment_primitive_orm* DueToOverdue::get_loan_app_installment()
{
    return lai_orm;
}

new_lms_installmentextension_primitive_orm* DueToOverdue::get_new_lms_installment_extention()
{
    return nlie_orm;
}

new_lms_installmentlatefees_primitive_orm *DueToOverdue::get_new_lms_installmentlatefees()
{
    return nlilf_orm;
}

int DueToOverdue::get_template_id()
{
    return template_id;
}

BDate DueToOverdue::get_closing_day()
{
    return closing_day;
}

BDate DueToOverdue::get_due_to_overdue_day()
{
    return due_to_overdue_date;
}

PSQLJoinQueryIterator *DueToOverdue::aggregator(string _closure_date_string)
{
    DueToOverdue dueToOverdue;
    return dueToOverdue.installments_becoming_overdue_agg(_closure_date_string);
}

LedgerAmount * DueToOverdue::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    lg->setInstallmentId(lai_orm->get_id());
    return lg;
}

void DueToOverdue::stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms)
{
    for (auto amounts_pair : *ledger_amount_orms) {
        ledger_amount_primitive_orm* amount = amounts_pair.second->first;
        int leg_template_id = amount->get_leg_temple_id();
        if (leg_template_id == 2) {
            nlie_orm->setUpdateRefernce("due_to_overdue_ledger_amount", amount);
        } 
        else if (leg_template_id == 3) {
            nlilf_orm->setUpdateRefernce("accrual_ledger_amount", amount);
        }
    }
}


void DueToOverdue::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::LEDGER_DUE_TO_OVERDUE)}}

        );
    psqlUpdateQuery.update(); 
}

DueToOverdue::~DueToOverdue()
{
}

LedgerAmount *DueToOverdue::_get_installment_insterest(LedgerClosureStep *dueToOverdue)
{
    LedgerAmount* ledgerAmount = ((DueToOverdue*) dueToOverdue)->_init_ledger_amount();
    loan_app_installment_primitive_orm* lai_orm = ((DueToOverdue*) dueToOverdue)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((DueToOverdue*) dueToOverdue)->get_new_lms_installment_extention();
    BDate installment_due_to_overdue_date(nlie_orm->get_due_to_overdue_date());
    if (installment_due_to_overdue_date() >= due_to_overdue_date()) {
        double interest_expected = lai_orm->get_interest_expected();
        double first_installment_interest_adjustment = nlie_orm->get_first_installment_interest_adjustment();
        ledgerAmount->setAmount(ROUND(interest_expected+first_installment_interest_adjustment));
    }
    return ledgerAmount;
}

LedgerAmount *DueToOverdue::_get_installment_principal(LedgerClosureStep *dueToOverdue)
{
    LedgerAmount* ledgerAmount = ((DueToOverdue*) dueToOverdue)->_init_ledger_amount();
    loan_app_installment_primitive_orm* lai_orm = ((DueToOverdue*) dueToOverdue)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((DueToOverdue*) dueToOverdue)->get_new_lms_installment_extention();
    BDate installment_due_to_overdue_date(nlie_orm->get_due_to_overdue_date());
    if (installment_due_to_overdue_date() >= due_to_overdue_date()) {
        bool is_principal_paid = nlie_orm->get_is_principal_paid();
        if (is_principal_paid) {
            string principal_payment_date_string = nlie_orm->get_principal_paid_at();
            BDate principal_payment_date = BDate(principal_payment_date_string);
            string due_to_overdue_date_string = nlie_orm->get_due_to_overdue_date();
            BDate due_to_overdue_date = BDate(due_to_overdue_date_string);
            if (principal_payment_date() < due_to_overdue_date()) {
                ledgerAmount->setAmount(0);
                return ledgerAmount;
            }
        }
        double principal_expected = lai_orm->get_principal_expected();
        ledgerAmount->setAmount(principal_expected);
    }
    return ledgerAmount;
}

LedgerAmount * DueToOverdue::_calc_installment_late_fees(LedgerClosureStep* dueToOverdue) {
    LedgerAmount* ledgerAmount = ((DueToOverdue*) dueToOverdue)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((DueToOverdue*) dueToOverdue)->get_new_lms_installment_extention();
    double late_fee = nlie_orm->get_late_fees_amount();
    ledgerAmount->setAmount(late_fee);
    return ledgerAmount;
}

bool DueToOverdue::checkAmounts()
{
    return true;
}

void DueToOverdue::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("Interest income becoming overdue", DueToOverdue::_get_installment_insterest);
    ledgerClosureService->addHandler("Loan principal becoming overdue", DueToOverdue::_get_installment_principal);
    ledgerClosureService->addHandler("Late repayment fee income accrual",DueToOverdue::_calc_installment_late_fees);
    // ledgerClosureService->addHandler("Booking marginalized late repayment fee income, if applicable",);
}