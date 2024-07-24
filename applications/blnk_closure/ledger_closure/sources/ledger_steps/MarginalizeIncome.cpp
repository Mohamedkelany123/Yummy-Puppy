#include "MarginalizeIncome.h"

LedgerAmount *MarginalizeIncome::_marginalize_interest(LedgerClosureStep *marginalizeIncome)
{
    LedgerAmount* ledgerAmount = ((MarginalizeIncome*) marginalizeIncome)->_init_ledger_amount();
    loan_app_installment_primitive_orm* lai_orm = ((MarginalizeIncome*) marginalizeIncome)->get_loan_app_installment();
    new_lms_installmentextension_primitive_orm* nlie_orm = ((MarginalizeIncome*) marginalizeIncome)->get_new_lms_installment_extention();
    int partial_marginalization_ledger_amount_id = nlie_orm->get_partial_marginalization_ledger_amount_id();
    double actual_accrued_amount = nlie_orm->get_actual_accrued_amount();
    double first_installment_interest_adjustment = nlie_orm->get_first_installment_interest_adjustment();
    double interest_expected = lai_orm->get_interest_expected();
    double amount;
    if (partial_marginalization_ledger_amount_id != 0) {
        amount = actual_accrued_amount + first_installment_interest_adjustment;
    }
    else {
        amount = interest_expected + first_installment_interest_adjustment;
    }
    amount = ROUND(amount);
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}

LedgerAmount *MarginalizeIncome::_marginalize_late_fee(LedgerClosureStep *marginalizeIncome)
{
    LedgerAmount* ledgerAmount = ((MarginalizeIncome*) marginalizeIncome)->_init_ledger_amount();
    vector<new_lms_installmentlatefees_primitive_orm*>* nlilf_orms = ((MarginalizeIncome*) marginalizeIncome)->get_new_lms_installmentlatefees();
    double total_late_fee_amount = 0.0;
    for(new_lms_installmentlatefees_primitive_orm* nlilf_orm : *nlilf_orms) {
        if (nlilf_orm != nullptr) {
            total_late_fee_amount += nlilf_orm->get_amount();
        }
    }
    ledgerAmount->setAmount(ROUND(total_late_fee_amount));
    return ledgerAmount;
}

bool MarginalizeIncome::checkAmounts()
{
    return true;
}

void MarginalizeIncome::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    if (is_installment) {
        ledgerClosureService->addHandler("Marginalize interest income, if applicable", MarginalizeIncome::_marginalize_interest);
    }
    else {
        ledgerClosureService->addHandler("Marginalize late repayment fee income, if applicable", MarginalizeIncome::_marginalize_late_fee);
    }
}

PSQLJoinQueryIterator *MarginalizeIncome::aggregator(QueryExtraFeilds * query_fields)
{

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
        {
            new loan_app_loan_primitive_orm("main"), 
            new loan_app_installment_primitive_orm("main"), 
            new new_lms_installmentextension_primitive_orm("main"),
            new new_lms_installmentlatefees_primitive_orm("main")
        },
        {
            {{"loan_app_loan", "id"}, {"loan_app_installment", "loan_id"}}, 
            {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},
            {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}

        });

    psqlQueryJoin->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.id", in, "158353, 157649, 157619, 157545, 157536, 157397, 157344, 157331, 157329, 157303, 157210, 157201, 157078, 157050, 157038, 157017, 156898, 156897, 156871, 156867, 156829, 156765, 156685, 156683, 156653, 156631, 156626, 156613, 156596, 156519, 156500, 156488, 156478, 156270, 156228, 156226, 156216, 156208, 156187, 156169, 156158, 156151, 156133, 156113, 156111, 156104, 156094, 156077, 156076, 156067"),
            new OROperator(
                new UnaryOperator("new_lms_installmentextension.is_marginalized", eq, true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.is_marginalized", eq, false),
                    new UnaryOperator("new_lms_installmentextension.marginalization_date", isnotnull, "", true)
                )
            ),
            new UnaryOperator("new_lms_installmentextension.marginalization_date", lte, query_fields->closure_date_string),
            new UnaryOperator("new_lms_installmentextension.marginalization_ledger_amount_id", isnull, "", true),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.accrual_ledger_amount_id", isnotnull, "", true),

            // new UnaryOperator ("loan_app_loan.closure_status",eq,ledger_status::MARGINALIZE_INCOME-1),

            new OROperator(
                new UnaryOperator("new_lms_installmentlatefees.is_marginalized", eq, true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentlatefees.is_marginalized", eq, false),
                    new UnaryOperator("new_lms_installmentlatefees.marginalization_date", isnotnull, "", true)
                )
            ),
            new UnaryOperator("new_lms_installmentlatefees.marginalization_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentlatefees.marginalization_date", lte, query_fields->closure_date_string)
        )
    );

    psqlQueryJoin->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc, new_lms_installmentlatefees.id asc");

    psqlQueryJoin->setAggregates(
        {{"loan_app_loan", {"id", 1}}
    });


    return psqlQueryJoin;
}
void MarginalizeIncome::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::MARGINALIZE_INCOME)}}

        );
    psqlUpdateQuery.update(); 
}

MarginalizeIncome::~MarginalizeIncome()
{
}

MarginalizeIncome::MarginalizeIncome(loan_app_loan_primitive_orm* _lal_orm, loan_app_installment_primitive_orm* _lai_orm, new_lms_installmentextension_primitive_orm *_nlie_orm, vector<new_lms_installmentlatefees_primitive_orm*>* _nlilf_orms, bool _is_installment)
{
    lal_orm = _lal_orm;
    lai_orm = _lai_orm;
    nlie_orm = _nlie_orm;
    nlilf_orms = _nlilf_orms;
    is_installment = _is_installment;
}

void MarginalizeIncome::set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm) {
    lal_orm = _lal_orm;
}

void MarginalizeIncome::set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm) {
    lai_orm = _lai_orm;
}

void MarginalizeIncome::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nlie_orm) {
    nlie_orm = _nlie_orm;
}

void MarginalizeIncome::set_new_lms_installmentlatefees(vector<new_lms_installmentlatefees_primitive_orm*>* _nlilf_orms)
{
    nlilf_orms = _nlilf_orms;
}

void MarginalizeIncome::set_template_id(int _template_id)
{
    template_id = _template_id;
}

void MarginalizeIncome::set_closing_day(BDate _closing_day)
{
    closing_day = _closing_day;
}

loan_app_loan_primitive_orm *MarginalizeIncome::get_loan_app_loan()
{
    return lal_orm;
}

loan_app_installment_primitive_orm *MarginalizeIncome::get_loan_app_installment()
{
    return lai_orm;
}

new_lms_installmentextension_primitive_orm* MarginalizeIncome::get_new_lms_installment_extention()
{
    return nlie_orm;
}

vector<new_lms_installmentlatefees_primitive_orm*>* MarginalizeIncome::get_new_lms_installmentlatefees()
{
    return nlilf_orms;
}

int MarginalizeIncome::get_template_id()
{
    return template_id;
}

BDate MarginalizeIncome::get_closing_day()
{
    return closing_day;
}

LedgerAmount * MarginalizeIncome::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    lg->setInstallmentId(lai_orm->get_id());
    return lg;
}

void MarginalizeIncome::stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *> *> *ledger_amount_orms)
{
    for (auto amounts : *ledger_amount_orms) {
        if (amounts.second->first->get_leg_temple_id() == 1) {
            nlie_orm->setUpdateRefernce("marginalization_ledger_amount", amounts.second->first);
        }
        if (amounts.second->first->get_leg_temple_id() == 2) { 
            for (auto nlilf_orm : *nlilf_orms) {
                nlilf_orm->setUpdateRefernce("marginalization_ledger_amount", amounts.second->first);
            }
        }
    }
}
