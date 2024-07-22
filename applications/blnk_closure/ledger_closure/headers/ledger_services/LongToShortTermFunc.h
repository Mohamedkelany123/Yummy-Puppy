    #ifndef LONGTOSHORTTERMFUNC_H
    #define LONGTOSHORTTERMFUNC_H

    #include <common.h>
    #include <TemplateManager.h>
    #include <LongToShortTerm.h>
    #include <common_orm.h>

    typedef struct {
        BlnkTemplateManager *blnkTemplateManager;
    } LongToShortTermStruct;

    void LongToShortTermFunc(map<string,PSQLAbstractORM*> *orms, int partition_number, mutex* shared_lock, void * extras);

    #endif