#include <ParentAccounts.h>

ParentAccounts& ParentAccounts::getInstance() {
    static ParentAccounts instance;
    return instance;
}

set<int> ParentAccounts::getParentAccounts() {
    return parentAccountIds;
}

ParentAccounts::ParentAccounts() {
    ledger_account_primitive_orm_iterator * la_itr =  new ledger_account_primitive_orm_iterator("main",-1);
    la_itr->execute();

    ledger_account_primitive_orm* la_orm = la_itr->next();
    while(la_orm != nullptr){
        parentAccountIds.insert(la_orm->get_parent_id());
        la_orm = la_itr->next();
    }
}

ParentAccounts::~ParentAccounts() {}
