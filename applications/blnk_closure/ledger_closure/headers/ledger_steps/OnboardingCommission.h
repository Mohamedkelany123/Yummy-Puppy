#ifndef ONBOARDINGCOMMISSION_H
#define ONBOARDINGCOMMISSION_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>




class OnboardingCommission : public LedgerClosureStep
{
    private:
        crm_app_customer_primitive_orm * cac_orm;
        ekyc_app_onboardingsession_primitive_orm * ekyco_orm;
        int merchant_id;
        int commission_ledger_entry_id;

    public:
        map<string, funcPtr> funcMap;
        OnboardingCommission();
        OnboardingCommission(map <string,PSQLAbstractORM *> * _orms);
        
        //Setters
        void set_commission_ledger_entry_id(int _commission_ledger_entry_id);
        void set_merchant_id(int _merchant_id);
        void set_crm_app_customer(crm_app_customer_primitive_orm* _cac_orm);
        void set_ekyc_app_onboardingsession(ekyc_app_onboardingsession_primitive_orm* _ekyco_orm);
        
        
        //Getters
        int get_commission_ledger_entry_id();
        int get_merchant_id();
        crm_app_customer_primitive_orm* get_crm_app_customer();
        ekyc_app_onboardingsession_primitive_orm *get_ekyc_app_onboardingsession();

        LedgerAmount * _init_ledger_amount();

        void stampORMs(ledger_entry_primitive_orm* entry);

        //static methods
        static LedgerAmount * _calculate_merchant_commission(LedgerClosureStep *onboardingCommission);
        bool checkAmounts();

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        static PSQLJoinQueryIterator* aggregator(QueryExtraFeilds * query_fields);
        static void update_step(); 

        ~OnboardingCommission();
};




#endif