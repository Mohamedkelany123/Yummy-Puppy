    #ifndef DISBURSEFUNC_H
    #define DISBURSEFUNC_H

    #include <functional>
    #include <common.h>
    #include <TemplateManager.h>
    #include <DisburseLoans.h>
    #include <common_orm.h>

    typedef struct {
        BlnkTemplateManager *blnkTemplateManager;
        float current_provision_percentage;    
    } DisburseLoanStruct;

    void DisburseLoanFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

    #endif 
