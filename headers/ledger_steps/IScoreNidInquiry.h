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

        float inquiry_fee;

    public:
        map<string, funcPtr> funcMap;
};  

#endif