#include <CustomerPaymentFunc.h>

void receiveCustomerPaymentFunc(map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    map<int, BlnkTemplateManager*>* blnkTemplateManagerMap = ((ReceiveCustomerPaymentStruct*)extras)->blnkTemplateManagerMap;
    BDate closing_date = ((ReceiveCustomerPaymentStruct*)extras)->closing_date;
    payments_loanorder_primitive_orm * plo_orm = ORM(payments_loanorder,orms);
    payments_loanorderheader_primitive_orm* ploh_orm = ORM(payment_loanorderheader, orms);
    loan_app_loan_primitive_orm* lal_orm = ORM(loan_app_loan, orms);
    payments_paymentmethod_primitive_orm* ppm_orm = ORM(payments_paymentmethodm orms);
    payments_paymentprovider_primitive_orm* ppp_orm = ORM(payments_paymentprovider, orms);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
    string first_loan_order_id = stoi(gorm->getExtra("first_loan_order_id"));
    string payment_provider_name = ppp_orm->get_name();
    string payment_method_name = ppm_orm->get_name();

    string_to_lower(payment_provider_name);
    string_to_lower(payment_method_name);

    int template_id = 18;

    if (payment_provider_name.compare("fawry") == 0) {
        template_id = 19;
    }
    else if (payment_provider_name.compare("blnk") == 0){
        if (payment_method_name.compare("pay") == 0) {
            template_id = 44;
        }
        else if (payment_method_name.compare("alex") == 0) {
            template_id = 165;
        }
        else if (payment_method_name.compare("collection") == 0) {
            template_id = 53;
        }
        else if (payment_method_name.compare("blnk") == 0) {
            template_id = 119;
        }
        else if (payment_method_name.compare("wallet") == 0) {
            template_id = 133;
        }
    }
    BlnkTemplateManager* parentTemplateManager = (*blnkTemplateManagerMap)[template_id];
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(parentTemplateManager, partition_number);
    CustomerPayment customerPayment(lal_orm, plo_orm, ploh_orm, closing_date, template_id, first_loan_order_id);

    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&customerPayment);
    customerPayment.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

    if (ledgerAmounts != nullptr) {
        localTemplateManager->setEntryData(ledgerAmounts);
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(closing_date);
        if (entry) {
            customerPayment.stampORMS(entry);
        }
        else {
            lal_orm->set_closure_status(-1*ledger_status::REPAYMENT_BY_CUSTOMER);
        }
    }
    
}
