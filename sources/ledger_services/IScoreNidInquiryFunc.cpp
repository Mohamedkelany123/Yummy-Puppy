#include <IScoreNidInquiryFunc.h>

void IScoreNidInquiryFunc(map<string,PSQLAbstractORM*>* orms,int partition_number, mutex* shared_lock, void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((IScoreNidInquiryStruct *) extras)->blnkTemplateManager,partition_number);
    ekyc_app_nidlog_primitive_orm * nid_orm = ORM(ekyc_app_nidlog,orms);
    float inquiryFee = ((IScoreNidInquiryStruct *) extras)->inquiryFee;

    IScoreNidInquiry iScoreNidInquiry(orms,inquiryFee);

    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&iScoreNidInquiry);
    iScoreNidInquiry.setupLedgerClosureService(ledgerClosureService);
    map<string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();

    if(ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);
        string stamping_date = nid_orm->get_created_at();

        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(stamping_date));
        map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

        if (entry){
            iScoreNidInquiry.stampORMs(entry);
        }
        else {
            cerr << "Can not stamp ORM objects\n";
            exit(1);
        }
    }

    delete (ledgerClosureService);
    
};