#include "SettlementLoansWithMerchantFunc.h"



double _get_request_amount(settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm, settlement_dashboard_settlementrequest_primitive_orm* sds_orm, loan_app_loan_primitive_orm* lal_orm)
{
    int request_type = sds_orm->get_type();
    
    //getting variables from loan table
    double principle = lal_orm->get_principle();
    double cashier_fee = lal_orm->get_cashier_fee();
    // double merchant_commission = lal_orm->get_merchant_commission();
    double loan_upfront_fee = lal_orm->get_loan_upfront_fee();
    double blnk_to_merchant_fee = lal_orm->get_bl_t_mer_fee();
    double merchant_to_blnk_fee = lal_orm->get_bl_t_mer_fee();
    
    //calculate values from the loan
    double cashier_commision = (principle*cashier_fee)/100;
    cashier_commision = ROUND(cashier_commision);
    double merchant_commission = (principle*blnk_to_merchant_fee)/100;
    merchant_commission = ROUND(merchant_commission);
    double upfront_fee = (loan_upfront_fee/100)*principle;
    upfront_fee = ROUND(upfront_fee);
    double rebate_commission = (principle*merchant_to_blnk_fee)/100;
    rebate_commission = ROUND(rebate_commission);
    double amount = 0;
    switch (request_type)
    {
    case 0:
        amount = ROUND(principle);
        break;
    case 1:
        amount = cashier_commision;
        break;
    case 2:
        amount = merchant_commission ;
        break;
    case 3:
        amount = -upfront_fee;
        break;
    case 4:
        amount = -rebate_commission;
        break;
    }

    return amount;
}

double _get_request_amount(int request_type, double principle, double cashier_fee, double mer_t_bl_fee, double bl_t_mer_fee, double loan_upfront_fee)
{

    //calculate values from the loan
    double cashier_commision = (principle*cashier_fee)/100;
    cashier_commision = ROUND(cashier_commision);
    double merchant_commission = (principle*bl_t_mer_fee)/100;
    merchant_commission = ROUND(merchant_commission);
    double upfront_fee = (loan_upfront_fee/100)*principle;
    upfront_fee = ROUND(upfront_fee);
    double rebate_commission = (principle*mer_t_bl_fee)/100;
    rebate_commission = ROUND(rebate_commission);
    double amount = 0;
    switch (request_type)
    {
    case 0:
        amount = ROUND(principle);
        break;
    case 1:
        amount = cashier_commision;
        break;
    case 2:
        amount = merchant_commission ;
        break;
    case 3:
        amount = -upfront_fee;
        break;
    case 4:
        amount = -rebate_commission;
        break;
    }

    return amount;
}

void settleLoansWithMerchantFunc(vector<map<string, PSQLAbstractORM *> *> *orms_list, int partition_number, mutex *shared_lock, void *extras)
{
    BlnkTemplateManager* templateManager = (BlnkTemplateManager*)(((SettlementLoansWithMerchantStruct*)extras)->blnkTemplateManager);
    BlnkTemplateManager* reverseTemplateManager = (BlnkTemplateManager*)(((SettlementLoansWithMerchantStruct*)extras)->reverseTemplateManager);
    BlnkTemplateManager* paymentTemplateManager = (BlnkTemplateManager*)(((SettlementLoansWithMerchantStruct*)extras)->paymentTemplateManager);
    BlnkTemplateManager* receiveTemplateManager = (BlnkTemplateManager*)(((SettlementLoansWithMerchantStruct*)extras)->receiveTemplateManager);
    settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm;
    settlement_dashboard_settlementrequest_primitive_orm* sds_orm;
    loan_app_loan_primitive_orm* lal_orm;
    PSQLGeneric_primitive_orm * gorm;
    set<loan_app_loan_primitive_orm*> loans, cancelled_loans;
    // map of links to map of loan ids to vector of maps of orms
    map<int, map<int, vector<map<string, PSQLAbstractORM*>*>*>*>* orm_vector_map = new map<int, map<int, vector<map<string, PSQLAbstractORM*>*>*>*>;

    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, i);
        sds_orm = ORML(settlement_dashboard_settlementrequest, orms_list, i);
        lal_orm = ORML(loan_app_loan, orms_list, i);
        int link = sds_orm->get_link();
        if (orm_vector_map->find(link) == orm_vector_map->end()) {
            (*orm_vector_map)[link] = new map<int, vector<map<string, PSQLAbstractORM*>*>*>;
        }

        int loanId = lal_orm->get_id();
        
        if ((*orm_vector_map)[link]->find(loanId) == (*orm_vector_map)[link]->end()) {
            (*(*orm_vector_map)[link])[loanId] = new vector<map<string, PSQLAbstractORM*>*>;
        }

        (*(*orm_vector_map)[link])[loanId]->push_back((*orms_list)[i]);
    }

    for (auto linkIterator : *orm_vector_map) {
        int category;
        BDate entry_date;
        int activation_user_id;
        double total_amount = 0;
        int link = linkIterator.first;
        bool has_unsettled = false;
        for (auto loanIterator : *linkIterator.second) {
            double loan_value = 0;
            double parent_loan_value = 0;
            int loanId = loanIterator.first;
            for (auto ormIterator : *loanIterator.second) {
                lal_orm = ORM(loan_app_loan, ormIterator);
                sdm_orm = ORM(settlement_dashboard_merchantpaymentrequest, ormIterator);
                sds_orm = ORM(settlement_dashboard_settlementrequest, ormIterator);
                gorm = ORM(PSQLGeneric,ormIterator);
                string type = gorm->get("type");
                double parent_principle = gorm->toDouble("parent_principle");
                double parent_cashier_fee = gorm->toDouble("parent_cashier_fee");
                double parent_mer_t_bl_fee = gorm->toDouble("parent_mer_t_bl_fee");
                double parent_bl_t_mer_fee = gorm->toDouble("parent_bl_t_mer_fee");
                double parent_loan_upfront_fee = gorm->toDouble("parent_loan_upfront_fee");

                int request_type = sds_orm->get_type();


                int category = sds_orm->get_category();
                int activation_user_id = sds_orm->get_activation_user_id();

                loan_value += _get_request_amount(sdm_orm, sds_orm, lal_orm);
                if (type == "Partial Refund") {
                    parent_loan_value += _get_request_amount(request_type, parent_principle, parent_cashier_fee, parent_mer_t_bl_fee, parent_bl_t_mer_fee, parent_loan_upfront_fee);
                }
                if (sds_orm->get_is_settled() == false) {
                    has_unsettled = true;
                }
            }
            string type = gorm->get("type");
            double balance_106 = gorm->toDouble("balance_106"), balance_179 = gorm->toDouble("balance_179");
            bool check_bool = gorm->toBool("check_bool");
            bool can_settle_bool = gorm->toBool("check_bool");
            int settled_pay_id = lal_orm->get_settled_pay_id();
            int category = sds_orm->get_category();
            int parent_loan_id = gorm->toInt("parent_loan_id");
            int parent_merchant_id = gorm->toInt("parent_merchant_id");
            int parent_settle_pay_id = gorm->toInt("parent_settle_pay_id");
            double parent_balance_106 = gorm->toDouble("parent_balance_106");
            double parent_balance_179 = gorm->toDouble("parent_balance_179");
            BlnkTemplateManager *localTemplateManager, *parentTemplateManager;
            
            if (check_bool && can_settle_bool) {
                category = sds_orm->get_category();
                entry_date = BDate(sds_orm->get_entry_date());
                activation_user_id = sds_orm->get_activation_user_id();
                bool is_reverse;
                if (type == "Full Refund") {
                    if (category == 1) {
                        localTemplateManager = new BlnkTemplateManager(templateManager, partition_number);
                        is_reverse = false;
                    }
                    else {
                        localTemplateManager = new BlnkTemplateManager(reverseTemplateManager, partition_number);
                        is_reverse = true;
                    }
                    double amount = loan_value;
                    bool check_flag = true;
                    if (balance_106 == 0 && balance_179 == 0 && settled_pay_id != 0) {
                        check_flag = false;
                    }
                    if (check_flag) {
                        SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id(), is_reverse);
                        LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
                        settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
                        map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
                        localTemplateManager->setEntryData(ledgerAmounts);
                        localTemplateManager->buildEntry(entry_date);
                    }
                    ledger_entry_primitive_orm* entry = localTemplateManager->get_entry();
                    settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
                    sdl_orm->setUpdateRefernce("entry", entry);
                    sdl_orm->setUpdateRefernce("request", sdm_orm);
                    sdl_orm->setUpdateRefernce("loan", lal_orm);
                    sdl_orm->set_link(sds_orm->get_link());
                    lal_orm->setUpdateRefernce("settled_cancel", sdm_orm);
                    total_amount -= amount;
                    cancelled_loans.insert(lal_orm);
                }
                else if (type == "Partial Refund") {
                    if (category == 1) {
                        parentTemplateManager = new BlnkTemplateManager(templateManager, partition_number);
                        is_reverse = false;
                    }
                    else {
                        parentTemplateManager = new BlnkTemplateManager(reverseTemplateManager, partition_number);
                        is_reverse = true;
                    }
                    double amount = parent_loan_value;
                    bool check_flag = true;
                    if (balance_106 == 0 && balance_179 == 0 && parent_settle_pay_id != 0) {
                        check_flag = false;
                    }
                    if (check_flag) {
                        SettlementLoansWithMerchant settlementWithMerchant(loan_value, parent_loan_id, sdm_orm->get_id(), parent_merchant_id, lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id(), is_reverse);
                        LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
                        settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
                        map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
                        parentTemplateManager->setEntryData(ledgerAmounts);
                        parentTemplateManager->buildEntry(entry_date);
                    }
                    ledger_entry_primitive_orm* entry_parent = parentTemplateManager->get_entry();
                    settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm_parent = new settlement_dashboard_loanentryrequest_primitive_orm("main");
                    sdl_orm_parent->setUpdateRefernce("entry", entry_parent);
                    sdl_orm_parent->set_request_id(sdm_orm->get_id());
                    sdl_orm_parent->set_loan_id(parent_loan_id);
                    sdl_orm_parent->set_link(sds_orm->get_link());
                    ORMVector <loan_app_loan_primitive_orm> ormVector = loan_app_loan_primitive_orm::fetch("main", UnaryOperator("id", eq, parent_loan_id));
                    loan_app_loan_primitive_orm* parent_lal_orm = ormVector[0];
                    parent_lal_orm->set_settled_cancel_id(sdm_orm->get_id());
                    total_amount -= amount;
                    cancelled_loans.insert(parent_lal_orm);

                    if (category == 0) {
                        localTemplateManager = new BlnkTemplateManager(templateManager, partition_number);
                        is_reverse = false;
                    }
                    else {
                        localTemplateManager = new BlnkTemplateManager(reverseTemplateManager, partition_number);
                        is_reverse = true;
                    }

                    amount = loan_value;
                    SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id(), is_reverse);
                    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
                    settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
                    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
                    localTemplateManager->setEntryData(ledgerAmounts);
                    localTemplateManager->buildEntry(entry_date);
                    ledger_entry_primitive_orm* entry = localTemplateManager->get_entry();
                    settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
                    sdl_orm->setUpdateRefernce("entry", entry);
                    sdl_orm->set_request_id(sdm_orm->get_id());
                    sdl_orm->set_loan_id(parent_loan_id);
                    sdl_orm->set_link(sds_orm->get_link());
                    lal_orm->set_settled_cancel_id(sdm_orm->get_id());
                    total_amount += amount;
                    loans.insert(lal_orm);
                }
                else {
                    if (category == 0) {
                        localTemplateManager = new BlnkTemplateManager(templateManager, partition_number);
                        is_reverse = false;
                    }
                    else {
                        localTemplateManager = new BlnkTemplateManager(reverseTemplateManager, partition_number);
                        is_reverse = true;
                    }
                    double amount = loan_value;     
                    SettlementLoansWithMerchant settlementWithMerchant(amount, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id(), is_reverse);
                    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
                    settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
                    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
                    localTemplateManager->setEntryData(ledgerAmounts);
                    localTemplateManager->buildEntry(entry_date);
                    ledger_entry_primitive_orm* entry = localTemplateManager->get_entry();
                    settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
                    sdl_orm->setUpdateRefernce("entry", entry);
                    sdl_orm->set_request_id(sdm_orm->get_id());
                    sdl_orm->set_loan_id(parent_loan_id);
                    sdl_orm->set_link(sds_orm->get_link());
                    lal_orm->set_settled_cancel_id(sdm_orm->get_id());
                    total_amount += amount;
                    loans.insert(lal_orm);
                }
            }
        }
        int bank = sdm_orm->get_bank_id();
        int bank_to = sdm_orm->get_bank_to_id();
        int sub_payment_bank = sds_orm->get_bank_id();
        if ((bank == 0 || sub_payment_bank == 0) && bank != 0) {
            bank = sub_payment_bank;
        }
        if ((bank_to == 0 || sub_payment_bank != bank_to) && bank_to != 0) {
            bank_to = sub_payment_bank;
        }
        cout << "Total amount: " << total_amount << endl;
        ledger_entry_primitive_orm* entry;
        if (category == 0) {
            SettleTransaction settleTransaction(lal_orm->get_merchant_id(), bank, bank_to, sdm_orm->get_id(), category, total_amount);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settleTransaction);
            settleTransaction.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(paymentTemplateManager, partition_number);
            localTemplateManager->setEntryData(ledgerAmounts);
            localTemplateManager->buildEntry(entry_date);
            entry = localTemplateManager->get_entry();
        }
        else {
            SettleTransaction settleTransaction(lal_orm->get_merchant_id(), bank, bank_to, sdm_orm->get_id(), category, total_amount);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settleTransaction);
            settleTransaction.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(receiveTemplateManager, partition_number);
            localTemplateManager->setEntryData(ledgerAmounts);
            localTemplateManager->buildEntry(entry_date);
            entry = localTemplateManager->get_entry();
        }
        settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
        sdl_orm->setUpdateRefernce("entry", entry);
        sdl_orm->setUpdateRefernce("request", sdm_orm);
        sdl_orm->set_link(sds_orm->get_link());
        stringstream cancelled_loans_stream, loans_stream;
        loans_stream << "{";
        for (auto loan : loans) {
            loans_stream << loan->get_id();
            if (loan != *loans.rbegin()) {
                loans_stream << ", ";
            }
        }
        loans_stream << "}";
        sdm_orm->set_loans(loans_stream.str());
        cancelled_loans_stream << "{";
        for (auto loan : cancelled_loans) {
            cancelled_loans_stream << loan->get_id();
            if (loan != *loans.rbegin()) {
                cancelled_loans_stream << ", ";
            }
        }
        cancelled_loans_stream << "}";
        sdm_orm->set_canceled_loans(cancelled_loans_stream.str());

        if (!has_unsettled) {
            sdm_orm->set_is_settled(true);
            sdm_orm->set_status(1);
        }

        /*        
        unsettled_settlement_requests=SettlementRequest.objects.filter(request = payment_request, is_settled=False)
        payment_request.loans = self.loans
        payment_request.canceled_loans = self.canceled_loans
        
        if unsettled_settlement_requests.count() ==0:
            payment_request.is_settled=True
            payment_request.status=1
            payment_request.save()
        */
        

       
    }

    // double loan_value = 0, parent_loan_value = 0;
    // settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm;
    // settlement_dashboard_settlementrequest_primitive_orm* sds_orm;
    // loan_app_loan_primitive_orm* lal_orm;
    // PSQLGeneric_primitive_orm * gorm;


    // for (int i=0; i<ORML_SIZE(orms_list); i++) {
    //     sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, i);
    //     sds_orm = ORML(settlement_dashboard_settlementrequest, orms_list, i);
    //     lal_orm = ORML(loan_app_loan, orms_list, i);
    //     gorm = ORML(PSQLGeneric,orms_list,i);
    //     string type = gorm->get("type");
    //     double parent_principle = gorm->toDouble("parent_principle");
    //     double parent_cashier_fee = gorm->toDouble("parent_cashier_fee");
    //     double parent_mer_t_bl_fee = gorm->toDouble("parent_mer_t_bl_fee");
    //     double parent_bl_t_mer_fee = gorm->toDouble("parent_bl_t_mer_fee");
    //     double parent_loan_upfront_fee = gorm->toDouble("parent_loan_upfront_fee");

    //     int request_type = sds_orm->get_type();


    //     int category = sds_orm->get_category();
    //     int activation_user_id = sds_orm->get_activation_user_id();

    //     loan_value += _get_request_amount(sdm_orm, sds_orm, lal_orm);
    //     if (type == "Partial Refund") {
    //         parent_loan_value += _get_request_amount(request_type, parent_principle, parent_cashier_fee, parent_mer_t_bl_fee, parent_bl_t_mer_fee, parent_loan_upfront_fee);
    //     }
    // }
    // gorm = ORML(PSQLGeneric,orms_list,0);
    // sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, 0);
    // sds_orm = ORML(settlement_dashboard_settlementrequest, orms_list, 0);
    // lal_orm = ORML(loan_app_loan, orms_list, 0);
    // string entry_date_string = sds_orm->get_entry_date();
    // BDate entry_date(entry_date);
    // string type = gorm->get("type");
    // double balance_106 = gorm->toDouble("balance_106"), balance_179 = gorm->toDouble("balance_179");
    // bool check_bool = gorm->toBool("check_bool");
    // bool can_settle_bool = gorm->toBool("check_bool");
    // int settled_pay_id = lal_orm->get_settled_pay_id();
    // int category = sds_orm->get_category();
    // int parent_loan_id = gorm->toInt("parent_loan_id");
    // int parent_merchant_id = gorm->toInt("parent_merchant_id");
    // int parent_settle_pay_id = gorm->toInt("parent_settle_pay_id");
    // double parent_balance_106 = gorm->toDouble("parent_balance_106");
    // double parent_balance_179 = gorm->toDouble("parent_balance_179");
    // BlnkTemplateManager *localTemplateManager = new BlnkTemplateManager(templateManager, partition_number);
    // if (check_bool && can_settle_bool) {
    //     if (type == "Full Refund") {
    //         double amount = loan_value;
    //         bool check_flag = true;
    //         if (balance_106 == 0 && balance_179 == 0 && settled_pay_id != 0) {
    //             check_flag = false;
    //         }
    //         if (check_flag) {
    //             SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
    //             LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
    //             settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
    //             map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    //             localTemplateManager->setEntryData(ledgerAmounts);
    //             localTemplateManager->buildEntry(entry_date);
    //         }
    //         settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
    //         sdl_orm->setUpdateRefernce("request", sdm_orm);
    //         sdl_orm->setUpdateRefernce("loan", lal_orm);
    //         sdl_orm->set_link(sds_orm->get_link());
    //         lal_orm->setUpdateRefernce("settled_cancel", sdm_orm);
    //     }
    //     else if (type == "Partial Refund") {
    //         double amount = parent_loan_value;
    //         bool check_flag = true;
    //         if (balance_106 == 0 && balance_179 == 0 && parent_settle_pay_id != 0) {
    //             check_flag = false;
    //         }
    //         if (check_flag) {
    //             SettlementLoansWithMerchant settlementWithMerchant(loan_value, parent_loan_id, sdm_orm->get_id(), parent_merchant_id, lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
    //             LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
    //             settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
    //             map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    //             localTemplateManager->setEntryData(ledgerAmounts);
    //             localTemplateManager->buildEntry(entry_date);
    //         }
    //         settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm_parent = new settlement_dashboard_loanentryrequest_primitive_orm("main");
    //         sdl_orm_parent->set_request_id(sdm_orm->get_id());
    //         sdl_orm_parent->set_loan_id(parent_loan_id);
    //         sdl_orm_parent->set_link(sds_orm->get_link());
    //         // stamp parent loan and not child loan
    //         //lal_orm->set_settled_cancel_id(sdm_orm->get_id());

    //         amount = loan_value;     
    //         SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
    //         LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
    //         settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
    //         map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    //         localTemplateManager->setEntryData(ledgerAmounts);
    //         localTemplateManager->buildEntry(entry_date);
    //         settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
    //         sdl_orm->set_request_id(sdm_orm->get_id());
    //         sdl_orm->set_loan_id(parent_loan_id);
    //         sdl_orm->set_link(sds_orm->get_link());
    //         lal_orm->set_settled_cancel_id(sdm_orm->get_id());
    //     }
    //     else {
    //         double amount = loan_value;     
    //         SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
    //         LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
    //         settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
    //         map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    //         localTemplateManager->setEntryData(ledgerAmounts);
    //         localTemplateManager->buildEntry(entry_date);
    //         settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
    //         sdl_orm->set_request_id(sdm_orm->get_id());
    //         sdl_orm->set_loan_id(parent_loan_id);
    //         sdl_orm->set_link(sds_orm->get_link());
    //         lal_orm->set_settled_cancel_id(sdm_orm->get_id());
    //     }
    // }
}
