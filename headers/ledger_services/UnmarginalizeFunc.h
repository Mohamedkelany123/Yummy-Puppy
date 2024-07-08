    #ifndef UNMARGINALIZEFUNC_H
    #define UNMARGINALIZEFUNC_H

    #include <functional>
    #include <common.h>
    #include <TemplateManager.h>
    #include <Unmarginalize.h>
    #include <common_orm.h>

    typedef struct {
        BlnkTemplateManager *blnkTemplateManager;
        float current_provision_percentage;    
    } UnmarginalizeStruct;

    void UnmarginalizeFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

    #endif 
