#include <CancelLateFees.h>

CancelLateFees::CancelLateFees(vector<map<string, PSQLAbstractORM*>*>* _orms_list)
{
    latefees_partition = new map <string,pair< vector<new_lms_installmentlatefees_primitive_orm *> *, double> *> ();
    lal_orm = ORML(loan_app_loan, _orms_list, 0);
    lai_orm = ORML(loan_app_installment, _orms_list, 0);
    auto* nli_orms = new vector<new_lms_installmentlatefees_primitive_orm*>();
    auto* latefees_orm = ORML(new_lms_installmentlatefees, _orms_list, 0);
    BDate cancellation_date = BDate(latefees_orm->get_cancellation_date());
    auto* partition = new pair<vector<new_lms_installmentlatefees_primitive_orm*>*, double>;
    double amount = 0;

    for (int i = 0; i < ORML_SIZE(_orms_list); i++)
    {
        latefees_orm = ORML(new_lms_installmentlatefees, _orms_list, i);
        BDate current_cancellation_date = BDate(latefees_orm->get_cancellation_date());

        if (current_cancellation_date() == cancellation_date())
        {
            cout << "they are equal" << endl;
            nli_orms->push_back(latefees_orm);
            amount += latefees_orm->get_amount();
        }
        else
        {
            cout << "they are not equal" << endl;

            partition->first = nli_orms;
            partition->second = amount;
            latefees_partition->insert({cancellation_date.getDateString(), partition});

            cancellation_date = BDate(latefees_orm->get_cancellation_date());
            nli_orms = new vector<new_lms_installmentlatefees_primitive_orm*>();
            partition = new pair<vector<new_lms_installmentlatefees_primitive_orm*>*, double>;
            amount = 0;
            i--;
        }
    }

    if (nli_orms != nullptr)
    {
        cout << "the nli orms is not null" << endl;
    }
    else
    {
        cout << "the nli orms is null" << endl;
    }

    partition->first = nli_orms;
    partition->second = amount;
    latefees_partition->insert({cancellation_date.getDateString(), partition});
    template_id = 50;
   
}



void CancelLateFees::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{

    ledgerClosureService->addHandler("Cancel Late Repayment Fees", CancelLateFees::_calculate_late_fee_amount);
}

 CancelLateFees::~CancelLateFees(){}

LedgerAmount*  CancelLateFees::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();

    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setInstallmentId(lai_orm->get_id());
    lg->setReversalBool(true);
    return lg;
}

loan_app_loan_primitive_orm* CancelLateFees::get_loan_app_loan()  {
    return lal_orm;
}

void CancelLateFees::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}
vector<new_lms_installmentlatefees_primitive_orm *> * CancelLateFees::get_late_fees()  {
    return nli_orms;
}

void CancelLateFees::set_late_fees(vector<new_lms_installmentlatefees_primitive_orm *> *_nli_orms)
{
    nli_orms = _nli_orms;
}


int CancelLateFees::get_template_id()  {
    return template_id;
}

void CancelLateFees::set_template_id(int _template_id)
{
    template_id = _template_id;
}

void CancelLateFees::set_amount(double _amount)
{
    amount = _amount;
}
double CancelLateFees::get_amount()
{
return amount;
}
map <string,pair< vector<new_lms_installmentlatefees_primitive_orm *> *, double> *> * CancelLateFees::get_latefees_partition(){
    return latefees_partition;
}
LedgerAmount * CancelLateFees::_calculate_late_fee_amount(LedgerClosureStep *cancelLateFees)
{
    double amount = ((CancelLateFees*)cancelLateFees)->get_amount();
    LedgerAmount * ledgerAmount = ((CancelLateFees*)cancelLateFees)->_init_ledger_amount();
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}


PSQLJoinQueryIterator *CancelLateFees::aggregator(string _closure_date_string)
{
    PSQLJoinQueryIterator *psqlQueryJoin = new PSQLJoinQueryIterator("main",
    {new loan_app_installment_primitive_orm("main"), new new_lms_installmentlatefees_primitive_orm("main"), new loan_app_loan_primitive_orm("main")},
    {{{"loan_app_installment", "id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}, {{"loan_app_installment", "loan_id"}, {"loan_app_loan", "id"}}});

    // psqlQueryJoin->addExtraFromField("(select count(*)>0 from loan_app_loanstatushistroy lal where lal.status_id in (12,13) and lal.day::date <= \'"+ _closure_date_string +"\' and lal.loan_id = loan_app_loan.id)","is_included");
    // psqlQueryJoin->addExtraFromField("(select distinct lal.day from loan_app_loanstatushistroy lal where lal.status_id in (12,13) and lal.loan_id = loan_app_loan.id)","cancellation_day");
    psqlQueryJoin->filter(
        ANDOperator(
            // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::CANCEL_LATE_FEES-1)),
            new UnaryOperator("new_lms_installmentlatefees.reversed_accrual_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentlatefees.cancellation_date", lte, _closure_date_string),
            new UnaryOperator("new_lms_installmentlatefees.is_cancelled", eq, true),
            new UnaryOperator("new_lms_installmentlatefees.accrual_ledger_amount_id", isnotnull, "",true),
            new UnaryOperator("loan_app_loan.id", in, "140204,137852,137189,137171,136900,136741,136664,135831,135829,135661,135614,135493,135310,135292,135241,135141,134918,134804,134763,134698,134538,134408,134241,134193,134172,134167,133970,133956,133853,133787,133774,133572,133469,133356,133343,133329,133135,133092,133089,133087,133080,133053,132950,132828,132803,132794,132780,132719,132714,132602,132377,132354,132352,132349,132303,132302,132249,132182,132144,132028")

            ));
    psqlQueryJoin->setOrderBy("loan_app_loan.id asc, loan_app_installment.id asc,new_lms_installmentlatefees.cancellation_date");
    psqlQueryJoin->setAggregates({{"loan_app_loan", {"id",1}}, {"loan_app_installment", {"id",1}}});

    return psqlQueryJoin;
}
void CancelLateFees::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator(
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.closure_status",gte,0)
            ),
            {{"closure_status",to_string(ledger_status::CANCEL_LATE_FEES)}}

            );
        psqlUpdateQuery.update();
}
void CancelLateFees::stampORMs (ledger_amount_primitive_orm * la_orm)
 
{

    for (auto it = nli_orms->begin(); it != nli_orms->end(); ++it) {
        (*it)->setUpdateRefernce("reversed_accrual_ledger_amount_id", la_orm);
    }
  
}