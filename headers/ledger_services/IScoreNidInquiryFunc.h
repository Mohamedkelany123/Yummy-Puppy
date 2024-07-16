#ifndef ISCORENIDINQUIRY_H
#define ISCORENIDINQUIRY_H


    #include <functional>
    #include <common.h>
    #include <TemplateManager.h>
    #include <IScoreNidInquiry.h>
    #include <common_orm.h>


    typedef struct {
        BlnkTemplateManager *blnkTemplateManager;
        float inquiryFee;
    } IScoreNidInquiryStruct;

    void IScoreNidInquiryFunc(map<string,PSQLAbstractORM*> *orms, int partition_number, mutex* shared_lock, void * extras);

#endif