#ifndef _DUETOOVERDUE_H_
#define _DUETOOVERDUE_H_

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>




class MarginalizeIncome : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        loan_app_installment_primitive_orm* lai_orm;
        new_lms_installmentextension_primitive_orm* nlie_orm;
        new_lms_installmentlatefees_primitive_orm* nlilf_orm;

        int template_id;
        // float prov_percentage;
        BDate closing_day;
        BDate marginalization_date;

    public:
        map<string, funcPtr> funcMap;
        MarginaliseIncome();
        MarginaliseIncome(loan_app_loan_primitive_orm * _lal_orm, loan_app_installment_primitive_orm* _lai_orm, new_lms_installmentextension_primitive_orm* _nlie_orm, new_lms_installmentlatefees_primitive_orm* _nlilf_orm, BDate _marginalization_date);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_bl_orm* _lal_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        void set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nlie_orm);
        void set_new_lms_installmentlatefees(new_lms_installmentlatefees_primitive_orm* _nlilf_orm);
        
        void set_template_id(int _template_id);
        void set_closing_day(BDate _closing_day);
        void set_marginalization_date(BDate _due_to_overdue_date);
        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();
        new_lms_installmentextension_primitive_orm* get_new_lms_installment_extention();
        new_lms_installmentlatefees_primitive_orm* get_new_lms_installmentlatefees();
        int get_template_id();
        BDate get_closing_day();
        BDate get_marginalization_date();

        LedgerAmount * _init_ledger_amount();

        void stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms);

        // //static methods
        LedgerAmount* _calculate_accrual_to_date(LedgerClosureStep* marginalizeIncome);
        LedgerAmount* _marginalize_interest(LedgerClosureStep* marginalizeIncome);
        LedgerAmount* _marginalize_late_fee(LedgerClosureStep* marginalizeIncome);
        

        bool checkAmounts();

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        static void update_step(); 

    ~DueToOverdue();
};





#endif