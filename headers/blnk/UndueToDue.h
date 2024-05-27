#ifndef UNDUETODUE_H
#define UNDUETODUE_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>




class UndueToDue : public LedgerClosureStep
{
    private:
        loan_app_loan_bl_orm * lal_orm;
        loan_app_installment_primitive_orm * lai_orm;
        new_lms_installmentextension_primitive_orm * nli_orm;

        int template_id;
        // float prov_percentage;
        BDate closing_day;
        BDate lsh_settle_paid_off_day;
        BDate lsh_settle_charge_off_day;
        int partial_settle_status;
        int settle_charge_off_status;
        int undue_to_due_amount;
        int undue_to_due_interest_amount;

    public:
        map<string, funcPtr> funcMap;
        UndueToDue(map <string,PSQLAbstractORM *> * _orms, BDate _closing_day);
        
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

        void stampORMs(ledger_entry_primitive_orm* entry, ledger_amount_primitive_orm * la_orm);

        // //static methods
        static LedgerAmount * _get_installment_insterest(LedgerClosureStep *UndueToDue);
        static LedgerAmount * _get_installment_principal(LedgerClosureStep *UndueToDue);
        bool checkAmounts();

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        

    ~UndueToDue();
};




#endif