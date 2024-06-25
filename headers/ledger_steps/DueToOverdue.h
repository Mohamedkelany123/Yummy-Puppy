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
        loan_app_loan_bl_orm * lal_orm;
        vector<loan_app_installment_primitive_orm*>* lai_orms;
        vector<new_lms_installmentextension_primitive_orm*> * nlie_orms;

        int template_id;
        // float prov_percentage;
        BDate closing_day;
        BDate due_to_overdue_date;

        PSQLJoinQueryIterator* installments_becoming_overdue_agg(string _closure_date_string);
    public:
        map<string, funcPtr> funcMap;
        DueToOverdue();
        DueToOverdue(loan_app_loan_primitive_orm * _lal_orm, vector<loan_app_installment_primitive_orm*>* _lai_orms, vector<new_lms_installmentextension_primitive_orm*>* _nlie_orms);
        DueToOverdue(map <string,PSQLAbstractORM *> * _orms, BDate _closing_day, int _ledger_closure_service_type=1);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_bl_orm* _lal_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        void set_loan_app_installment(new_lms_installmentextension_primitive_orm* _nli_orm);
        
        void set_template_id(int _template_id);
        void set_provision_percentage(float _provision_percentage);
        
        //Getters
        loan_app_loan_bl_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();
        new_lms_installmentextension_primitive_orm *get_new_lms_installment_extention();
        int get_template_id();
        float get_provision_percentage();

        BDate get_closing_day();
        BDate get_lsh_settle_paid_off_day();
        BDate get_lsh_settle_charge_off_day();
        
        int get_partial_settle_status();
        int get_settle_charge_off_status();


        LedgerAmount * _init_ledger_amount();

        void stampORMs(map<string, LedgerCompositLeg *> *leg_amounts);

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