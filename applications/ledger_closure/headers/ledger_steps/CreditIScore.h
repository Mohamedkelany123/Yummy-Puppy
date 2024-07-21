#ifndef CREDIT_ISCORE_H
#define CREDIT_ISCORE_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>


class CreditIScore : public LedgerClosureStep{
    private:
        credit_app_inquirylog_primitive_orm * cail_orm;
        ekyc_app_onboardingsession_primitive_orm * eyobs_orm;
        int template_id;
        float expense_fee;
        int merchant_id;
    public:
        map<string, funcPtr> funcMap;
        CreditIScore(map<string,PSQLAbstractORM *> * _orms, float _expense_fee);
        
        //Setters
        void set_credit_app_inquirylog(credit_app_inquirylog_primitive_orm* _cail_orm);
        void set_template_id(int _template_id);

        //Getters
        credit_app_inquirylog_primitive_orm* get_credit_app_inquirylog();
        int get_template_id();
        float get_expense_fee();

        LedgerAmount * _init_ledger_amount();

        void stampORMs(ledger_entry_primitive_orm* entry, ledger_amount_primitive_orm * la_orm);

        // //static methods
        static LedgerAmount * _calculate_inquiry_fee(LedgerClosureStep *creditIScore);

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);

        ~CreditIScore();
};

#endif