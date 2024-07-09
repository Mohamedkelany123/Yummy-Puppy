#ifndef _DUETOOVERDUE_H_
#define _DUETOOVERDUE_H_

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>




class DueToOverdue : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        loan_app_installment_primitive_orm* lai_orm;
        new_lms_installmentextension_primitive_orm* nlie_orm;
        new_lms_installmentlatefees_primitive_orm* nlilf_orm;

        int template_id;
        // float prov_percentage;
        BDate closing_day;
        BDate due_to_overdue_date;

        PSQLJoinQueryIterator* installments_becoming_overdue_agg(string _closure_date_string);
    public:
        map<string, funcPtr> funcMap;
        DueToOverdue();
        DueToOverdue(loan_app_loan_primitive_orm * _lal_orm, loan_app_installment_primitive_orm* _lai_orm, new_lms_installmentextension_primitive_orm* _nlie_orm, new_lms_installmentlatefees_primitive_orm* _nlilf_orm, BDate _due_to_overdue_date);
        DueToOverdue(map <string,PSQLAbstractORM *> * _orms, BDate _closing_day, int _ledger_closure_service_type=1);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_bl_orm* _lal_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        void set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nlie_orm);
        void set_new_lms_installmentlatefees(new_lms_installmentlatefees_primitive_orm* _nlilf_orm);
        
        void set_template_id(int _template_id);
        void set_closing_day(BDate _closing_day);
        void set_due_to_overdue_date(BDate _due_to_overdue_date);
        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();
        new_lms_installmentextension_primitive_orm* get_new_lms_installment_extention();
        new_lms_installmentlatefees_primitive_orm* get_new_lms_installmentlatefees();
        int get_template_id();
        BDate get_closing_day();
        BDate get_due_to_overdue_day();

        LedgerAmount * _init_ledger_amount();

        void stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms);

        // //static methods
        static LedgerAmount * _get_installment_insterest(LedgerClosureStep *dueToOverdue);
        static LedgerAmount * _get_installment_principal(LedgerClosureStep *dueToOverdue);
        static LedgerAmount * _calc_installment_late_fees(LedgerClosureStep *dueToOverdue);
        bool checkAmounts();

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        static void update_step(); 

    ~DueToOverdue();
};





#endif