#include <DueToOverdue.h>

PSQLJoinQueryIterator* DueToOverdue::installments_becoming_overdue_agg(string _closure_date_string)
{
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {
            new loan_app_loan_bl_orm("main"), 
            new loan_app_installment_primitive_orm("main"), 
            new new_lms_installmentextension_primitive_orm("main"), 
        },
        {
            {{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, 
            {{"loan_app_installment","id"}, {"new_lms_installmentextension", "installment_ptr_id"}}, 
        }
    );

    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,_closure_date_string),
            new UnaryOperator("new_lms_installmentlatefees.day", lte, _closure_date_string),
            new UnaryOperator("new_lms_installmentlatefees.accrual_ledger_amount", isnull, true),
            new UnaryOperator("loan_app_loan.closure_status", eq, closure_status::DUE_TO_OVERDUE-1),
            new UnaryOperator("new_lms_installmentextension.due_to_overdue_date", lte, _closure_date_string),
            new UnaryOperator("new_lms_installmentextension.payment_status", nin, "1, 3, 6")
        )
    );

    psqlQueryJoin->addExtraFromField(
        "select entry.entry_date as last_entry_date from loan_app_loan loan left join ledger_amount amount on amount.loan_id=loan_app_loan.id inner join ledger_entry entry on entry.id=amount.entry_id order by entry.entry_date desc limit 1", 
        "last_entry_date"
    );
    psqlQueryJoin->addExtraFromField(
        "select is_paid from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
        "latefee_is_paid"
    );
    psqlQueryJoin->addExtraFromField(
        "select paid_at from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
        "latefee_paid_at"
    );
    psqlQueryJoin->addExtraFromField(
        "select is_cancelled from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
        "latefee_is_cancelled"
    );
    psqlQueryJoin->addExtraFromField(
        "select cancellation_date from new_lms_installmentlatefee lf right join new_lms_installmentextension ie on lf.installment_extension_id = new_lms_installmentextension.installment_ptr_id where lf.accrual_ledger_amount = null order by id desc limit 1",
        "latefee_cancellation_date"
    );

    psqlQueryJoin->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc");
    psqlQueryJoin->setAggregates ({
        {"loan_app_loan","id"}
    });
        
    return psqlQueryJoin;
}

DueToOverdue::DueToOverdue(loan_app_loan_primitive_orm *_lal_orm, vector<loan_app_installment_primitive_orm *> *_lai_orms, vector<new_lms_installmentextension_primitive_orm *> *_nlie_orms)
{
    lal_orm = _lal_orm;
    lai_orms = _lai_orms;
    nlie_orms = _nlie_orms;
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
    // lg->setMerchantId(lal_orm->get_merchant_id());
    return lg;
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

LedgerAmount *DueToOverdue::_get_installment_insterest(LedgerClosureStep *dueToOverdue)
{
    LedgerAmount* ledgerAmount = ((DueToOverdue*) dueToOverdue)->_init_ledger_amount();
    loan_app_installment_primitive_orm* lai_orm = ((DueToOverdue*) dueToOverdue)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((DueToOverdue*) dueToOverdue)->get_new_lms_installment_extention();
    double interest_expected = lai_orm->get_interest_expected();
    double first_installment_interest_adjustment = nlie_orm->get_first_installment_interest_adjustment();
    ledgerAmount->setAmount(ROUND(interest_expected+first_installment_interest_adjustment));
    return ledgerAmount;
}

LedgerAmount *DueToOverdue::_get_installment_principal(LedgerClosureStep *dueToOverdue)
{
    LedgerAmount* ledgerAmount = ((DueToOverdue*) dueToOverdue)->_init_ledger_amount();
    loan_app_installment_primitive_orm* lai_orm = ((DueToOverdue*) dueToOverdue)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((DueToOverdue*) dueToOverdue)->get_new_lms_installment_extention();
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
    return ledgerAmount;
}

LedgerAmount * DueToOverdue::_calc_installment_late_fees(LedgerClosureStep* dueToOverdue) {
    LedgerAmount* ledgerAmount = ((DueToOverdue*) dueToOverdue)->_init_ledger_amount();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((DueToOverdue*) dueToOverdue)->get_new_lms_installment_extention();
    double late_fee = nlie_orm->get_late_fees_amount();
    ledgerAmount->setAmount(late_fee);
    return ledgerAmount;
}

void DueToOverdue::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("Interest income becoming overdue", DueToOverdue::_get_installment_insterest);
    ledgerClosureService->addHandler("Loan principal becoming overdue", DueToOverdue::_get_installment_principal);
    ledgerClosureService->addHandler("Late repayment fee income accrual",DueToOverdue::_calc_installment_late_fees);
    // ledgerClosureService->addHandler("Booking marginalized late repayment fee income, if applicable",);
}