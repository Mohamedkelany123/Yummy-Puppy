#ifndef WALLETPREPAID_H
#define WALLETPREPAID_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>




class WalletPrepaid : public LedgerClosureStep
{
    private:
        new_lms_customerwallettransaction_primitive_orm * transaction_orm;
        int customer_id;

        int template_id;
        string provider_name;
        string method_name;

        // float prov_percentage;
        BDate closing_day;
     

    
    public:
        map<string, funcPtr> funcMap;
        WalletPrepaid(new_lms_customerwallettransaction_primitive_orm* _orm, BDate _closing_day);
        
        //Setters
        void set_wallet_transaction(new_lms_customerwallettransaction_primitive_orm* _lal_orm);
      
        
        void set_template_id(int _template_id);
        void set_customer_id(int _customer_id);

        //Getters
        new_lms_customerwallettransaction_primitive_orm* get_wallet_transaction();
      
        int get_template_id();
        int get_customer_id();
        BDate get_closing_day();

        
     

        LedgerAmount * _init_ledger_amount();
        static  LedgerAmount *_get_wallet_amount(LedgerClosureStep *walletPrepaid);
        void stampORMs(ledger_amount_primitive_orm * la_orm);

        // //static methods
    
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        static new_lms_customerwallettransaction_primitive_orm_iterator* aggregator(string _closure_date_string);
        static string get_paymob_name();

        static void update_step(); 

    ~WalletPrepaid();
};




#endif