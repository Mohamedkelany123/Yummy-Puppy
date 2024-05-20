#ifndef _ACCRUAL_INTEREST_H_
#define _ACCRUAL_INTEREST_H_

#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>


class AccrualInterest : public LedgerClosureStep {
    private:
        loan_app_loan_primitive_orm * lal_orm;
        loan_app_installment_primitive_orm* lai_orm;
        new_lms_installmentextension_primitive_orm* nli_orm;
        string marginalization_history;
        string last_order_date;
        string settled_history;
        int accrual_type;
    public:
        map<string, funcPtr> funcMap;
        AccrualInterest(map <string,PSQLAbstractORM *> * _orms, int _accrual_type);
        ~AccrualInterest();
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        LedgerAmount * _init_ledger_amount();

        void stampORMs(map <string,LedgerCompositLeg*> *leg_amounts);

        // Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        void set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nli_orm);
        void set_marginalization_history(string _last_status_history);
        void set_last_order_date(string _last_order_date);
        void set_settled_history(string _settled_history);
        void set_accrual_type(int _accrual_type);
        // Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();
        new_lms_installmentextension_primitive_orm* get_new_lms_installmentextension();
        const string get_marginalization_history();
        const string get_last_order_date();
        const string get_settled_history();
        const int get_accrual_type();

        // Calculators
        static LedgerAmount *_get_marginalization_interest(LedgerClosureStep* accrualInterest);
        static LedgerAmount *_get_accrued_interest(LedgerClosureStep* accrualInterest);



};

#endif