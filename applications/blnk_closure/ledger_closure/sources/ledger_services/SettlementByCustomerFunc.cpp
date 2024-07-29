#include <SettlementByCustomerFunc.h>


void settlementByCustomerFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    
    payments_loanorder_primitive_orm * pl_orm = ORML(payments_loanorder, orms_list, 0);
    loan_app_loan_primitive_orm* lal_orm = ORML(loan_app_loan, orms_list, 0);
    PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms_list, 0);

    loan_app_installment_primitive_orm* lai_orm = nullptr;
    new_lms_installmentextension_primitive_orm* nlie_orm = nullptr;
    new_lms_installmentlatefees_primitive_orm* nlilf_orm = nullptr;
    
    BlnkTemplateManager* blnkTemplateManager = ((SettlementByCustomerStruct*)extras)->blnkTemplateManager;
    BlnkTemplateManager* secTemplateManager = ((SettlementByCustomerStruct*)extras)->securitizationTemplateManager;
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(blnkTemplateManager, partition_number);
    BlnkTemplateManager* localSecTemplateManager = new BlnkTemplateManager(secTemplateManager, partition_number);

    map<loan_app_installment_primitive_orm*, new_lms_installmentextension_primitive_orm*> installmentToExtension;
    BDate entry_date = BDate(pl_orm->get_paid_at());

    map<payments_loanorder_primitive_orm*, set<loan_app_installment_primitive_orm*>*> installmentsMap;
    map<loan_app_installment_primitive_orm*, vector<new_lms_installmentlatefees_primitive_orm*>*> lateFeeMap;

    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        lai_orm = ORML(loan_app_installment, orms_list, i);
        nlie_orm = ORML(new_lms_installmentextension, orms_list, i);
        nlilf_orm = ORML(new_lms_installmentlatefees, orms_list, i);
        if (nlie_orm != nullptr) {
            if (installmentsMap.find(pl_orm)==installmentsMap.end()) {
                installmentsMap[pl_orm] = new set<loan_app_installment_primitive_orm*>;
            }
            installmentsMap[pl_orm]->insert(lai_orm);
            installmentToExtension[lai_orm] = nlie_orm;
        }

        if (nlilf_orm != nullptr) {
            if (lateFeeMap.find(lai_orm)==lateFeeMap.end()) {
                lateFeeMap[lai_orm] = new vector<new_lms_installmentlatefees_primitive_orm*>;
            }
            lateFeeMap[lai_orm]->push_back(nlilf_orm);
        }
    }

    BDate closing_date = ((SettlementByCustomerStruct*)extras)->closing_day;
    set<loan_app_installment_primitive_orm*>* installmentsVector;
    ledger_entry_primitive_orm* entry;
    
    float cash_in_escrow = ROUND(gorm->toFloat("ledger_paid_orders_amount")/100) - gorm->toFloat("principal_paid_amount") - gorm->toFloat("interest_paid_amount") - gorm->toFloat("early_paid_amount") - gorm->toFloat("extra_paid_amount") - gorm->toFloat("lf_paid_amount");
    float paid_sec_amount = 0.0f;
    bool is_entry_created = false;

    for (auto installment : installmentsMap){
        installmentsVector = installment.second;

        for(auto it=installmentsVector->begin(); it!=installmentsVector->end(); ++it) {
                lai_orm = (*it);
                nlie_orm = installmentToExtension[lai_orm];
                vector<new_lms_installmentlatefees_primitive_orm*>* latefeesVector = lateFeeMap[lai_orm];
                int last_status = gorm->toInt("last_status");;
                int unmarginalization_template_id = gorm->toInt("unmarginalization_template");;
                string settlement_day_str = gorm->get("settlement_day");
                string unmarginalization_template_str = gorm->get("unmarginalization_template");
                BDate settlement_day;
                if (settlement_day_str != ""){
                    settlement_day = BDate(settlement_day_str)();
                }
                int template_id = 14;
                SettlementByCustomer settlementByCustomer(lal_orm,pl_orm,lai_orm,nlie_orm,latefeesVector,
                unmarginalization_template_id, closing_date, settlement_day_str, last_status, cash_in_escrow);

                LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementByCustomer);
                settlementByCustomer.setupLedgerClosureService(ledgerClosureService);
                map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

                if (ledgerAmounts != nullptr) {
                    if (!is_entry_created){
                        localTemplateManager->createEntry(entry_date);
                        is_entry_created = true;
                    }
                    entry = localTemplateManager->get_entry();
                    localTemplateManager->setEntry(entry);
                    localTemplateManager->setEntryData(ledgerAmounts);
                    ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(entry_date);
                    map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();
                    if (entry) {
                        settlementByCustomer.stampORMs(leg_amounts);
                        cash_in_escrow = settlementByCustomer.get_cash_in_escrow();
                        paid_sec_amount += settlementByCustomer.get_paid_sec_amount();
                    }
                    else {
                        cerr << "Can not stamp ORM objects\n";
                        lal_orm->set_closure_status(-1*ledger_status::SETTLEMENT_BY_CUSTOMER);
                    }
                }
                delete (ledgerClosureService);
            }
    }
    if (paid_sec_amount > 0.0f){
        //TODO: Sec map per installment
        secTemplateManager->createEntry(entry_date);
        DueToSecuritizationBond dueToSecBond(pl_orm->get_loan_id(), paid_sec_amount, nlie_orm->get_bond_id());
        LedgerClosureService* ledgerClosureService = new LedgerClosureService(&dueToSecBond);
        dueToSecBond.setupLedgerClosureService(ledgerClosureService);
        map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

        if (ledgerAmounts != nullptr)
        {
            secTemplateManager->setEntryData(ledgerAmounts);

            ledger_entry_primitive_orm* entry = secTemplateManager->buildEntry(entry_date);
        }

        // delete secTemplateManager;
        delete ledgerClosureService;

    }
    
}

