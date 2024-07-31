#include "MarginalizeIncomeFunc.h"

void MarginalizeIncomeFunc(vector<map<string, PSQLAbstractORM *> *> *orms_list, int partition_number, mutex *shared_lock, void *extras)
{
    loan_app_loan_primitive_orm* lal_orm = nullptr;
    loan_app_installment_primitive_orm* lai_orm = nullptr;
    new_lms_installmentextension_primitive_orm* nlie_orm = nullptr;
    new_lms_installmentlatefees_primitive_orm* nlilf_orm = nullptr;
    BlnkTemplateManager* localTemplateManager = ((MarginalizeIncomeStruct*)extras)->blnkTemplateManager;
    map<time_t, BlnkTemplateManager*> templateManagerMap;
    map<loan_app_installment_primitive_orm*, new_lms_installmentextension_primitive_orm*> installmentToExtension;
    BDate lateFeeDate, installmentDate;

    //Create Tempalte Manager for every date
    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        lal_orm = ORML(loan_app_loan, orms_list, i);
        lai_orm = ORML(loan_app_installment, orms_list, i);
        nlie_orm = ORML(new_lms_installmentextension, orms_list, i);
        nlilf_orm = ORML(new_lms_installmentlatefees, orms_list, i);

        installmentDate = BDate(nlie_orm->get_marginalization_date());
        lateFeeDate = BDate(nlilf_orm->get_marginalization_date());

        
        if (templateManagerMap.find(installmentDate())==templateManagerMap.end()) {
            templateManagerMap[installmentDate()] = new BlnkTemplateManager(localTemplateManager, partition_number);
            templateManagerMap[installmentDate()]->createEntry(installmentDate);
        }
        if (templateManagerMap.find(lateFeeDate())==templateManagerMap.end()) {
            templateManagerMap[lateFeeDate()] = new BlnkTemplateManager(localTemplateManager, partition_number);
            templateManagerMap[lateFeeDate()]->createEntry(lateFeeDate);
        }
        installmentToExtension[lai_orm] = nlie_orm;
    }

    map<time_t, set<loan_app_installment_primitive_orm*>*> installmentsMap;
    map<time_t, map<loan_app_installment_primitive_orm*, vector<new_lms_installmentlatefees_primitive_orm*>*>*> lateFeeMap;

    //To divide ins & lf per date
    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        lal_orm = ORML(loan_app_loan, orms_list, i);
        lai_orm = ORML(loan_app_installment, orms_list, i);
        nlie_orm = ORML(new_lms_installmentextension, orms_list, i);
        nlilf_orm = ORML(new_lms_installmentlatefees, orms_list, i);

        installmentDate = BDate(nlie_orm->get_marginalization_date());
        lateFeeDate = BDate(nlilf_orm->get_marginalization_date());
        if (nlie_orm != nullptr) {
            if (installmentsMap.find(installmentDate())==installmentsMap.end()) {
                installmentsMap[installmentDate()] = new set<loan_app_installment_primitive_orm*>;
            }
            installmentsMap[installmentDate()]->insert(lai_orm);
        }

        if (nlilf_orm != nullptr) {
            if (lateFeeMap.find(lateFeeDate())==lateFeeMap.end()) {
                lateFeeMap[lateFeeDate()] = new map<loan_app_installment_primitive_orm*, vector<new_lms_installmentlatefees_primitive_orm*>*>;
            }
            if (lateFeeMap[lateFeeDate()]->find(lai_orm)==lateFeeMap[lateFeeDate()]->end()) {
                lateFeeMap[lateFeeDate()]->operator[](lai_orm) = new vector<new_lms_installmentlatefees_primitive_orm*>;
            }
            (*lateFeeMap[lateFeeDate()])[lai_orm]->push_back(nlilf_orm);
        }
    }

    vector<new_lms_installmentlatefees_primitive_orm*>* latefeesVector;
    set<loan_app_installment_primitive_orm*>* installmentsVector;
    ledger_entry_primitive_orm* entry;
    BlnkTemplateManager* loopTemplateManager, *latefeeTemplateManager, *installmentTemplateManager;
    map<string, LedgerAmount*>* ledgerAmounts;
    BDate entryDate;

    //Create amounts for installments
    for (auto installmentsPerDate : installmentsMap) {
        time_t t = installmentsPerDate.first;
        entryDate = BDate(t);
        installmentsVector = installmentsPerDate.second;
        loopTemplateManager = templateManagerMap[entryDate()];
        entry = loopTemplateManager->get_entry();
        for(auto it=installmentsVector->begin(); it!=installmentsVector->end(); ++it) {
            lai_orm = (*it);
            nlie_orm = installmentToExtension[lai_orm];
            installmentTemplateManager = new BlnkTemplateManager(loopTemplateManager, partition_number);
            installmentTemplateManager->setEntry(entry);
            MarginalizeIncome marginalizeInstallment(lal_orm, lai_orm, nlie_orm, nullptr, true);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&marginalizeInstallment);
            marginalizeInstallment.setupLedgerClosureService(ledgerClosureService);
            ledgerAmounts = ledgerClosureService->inference();
            if (ledgerAmounts != nullptr) {
                installmentTemplateManager->setEntryData(ledgerAmounts);
                installmentTemplateManager->buildEntry(entryDate);
                map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms = installmentTemplateManager->get_ledger_amount_orms();
                if (ledger_amount_orms->size()) {
                    marginalizeInstallment.stampORMs(ledger_amount_orms);
                }
                else {
                    cerr << "No legs created\n";
                }
            }
            else {
                cout << "No Due to overdue amounts created\n";
            }
        }
    }

    //Create amounts for latefees
    for (auto latefeesPerDate : lateFeeMap) {
        time_t t = latefeesPerDate.first;
        entryDate = BDate(t);
        loopTemplateManager = templateManagerMap[entryDate()];
        entry = loopTemplateManager->get_entry();
        for (auto latefeesPerInstallment : *latefeesPerDate.second) {
            lai_orm = latefeesPerInstallment.first;
            nlie_orm = installmentToExtension[lai_orm];
            latefeesVector = latefeesPerInstallment.second;
            latefeeTemplateManager = new BlnkTemplateManager(loopTemplateManager, partition_number);
            latefeeTemplateManager->setEntry(entry);
            MarginalizeIncome marginalizeLateFee(lal_orm, lai_orm, nlie_orm, latefeesVector, false);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&marginalizeLateFee);
            marginalizeLateFee.setupLedgerClosureService(ledgerClosureService);
            ledgerAmounts = ledgerClosureService->inference();
            if (ledgerAmounts != nullptr) {
                latefeeTemplateManager->setEntryData(ledgerAmounts);
                latefeeTemplateManager->buildEntry(entryDate);
                map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms = latefeeTemplateManager->get_ledger_amount_orms();
                if (ledger_amount_orms->size()) {
                    marginalizeLateFee.stampORMs(ledger_amount_orms);
                }
                else {
                    cerr << "No legs created\n";
                }
            }
            else {
                cout << "No Due to overdue amounts created\n";
            }
        }
        
    }

    
    
}