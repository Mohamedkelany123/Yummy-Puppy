#ifndef PARENTACCOUNT_H
#define PARENTACCOUNT_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>


//Singlton class to load parentAccount id once for template managers 
class ParentAccounts {
private:
    set<int> parentAccountIds;
    ParentAccounts();
    ~ParentAccounts();
public:
    static ParentAccounts& getInstance();
    // Delete copy constructor and assignment operator to prevent copies
    ParentAccounts(const ParentAccounts&) = delete;
    ParentAccounts& operator=(const ParentAccounts&) = delete;

    set<int> getParentAccounts();
};




#endif
