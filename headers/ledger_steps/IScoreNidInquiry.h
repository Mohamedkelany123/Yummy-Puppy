#ifndef ISCORE_NID_INQUIRY_H
#define ISCORE_NID_INQUIRY_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>

class IScoreNidInquiry : public LedgerClosureStep
{
    private:
        ekyc_app_nidlog_primitive_orm * nid_orm;
        ekyc_app_onboardingsession_primitive_orm * onb_orm;

        int merchantID;
        int customerID;
        float inquiryFee;

    public:
        map<string, funcPtr> funcMap;
        IScoreNidInquiry(map<string,PSQLAbstractORM *> * _orms, float inquiry_fee);

        LedgerAmount * _init_ledger_amount();
        void stampORMs(ledger_entry_primitive_orm * entry);
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        //Getters
        float get_inquiry_fee();

        //static methods
        static LedgerAmount * _calculate_inquiry_fee(LedgerClosureStep *IScoreNidInquiry);
        static void update_step(); 
        static  PSQLJoinQueryIterator* aggregator(string _closure_date_string);

        ~IScoreNidInquiry();
};  

#endif