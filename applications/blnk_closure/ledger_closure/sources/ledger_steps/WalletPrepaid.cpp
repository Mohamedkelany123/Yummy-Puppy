#include <WalletPrepaid.h>


WalletPrepaid::WalletPrepaid(new_lms_customerwallettransaction_primitive_orm * _transaction_orm, BDate _closing_day)
{
    transaction_orm = _transaction_orm;
    customer_id =  stoi(_transaction_orm->getExtra("customer_id"));
    closing_day = _closing_day;
    provider_name = _transaction_orm->getExtra("payment_provider_name");
    for(auto& x:provider_name){ 
        
          x = (char)tolower(x); 
    } 
    // cout << "provider name"<<provider_name<<endl;
    method_name = _transaction_orm->getExtra("payment_method_name");
     for(auto& x:method_name){ 
        
          x = (char)tolower(x); 
    } 
    // cout << "method name"<<method_name<<endl;

    template_id = 134;
}


new_lms_customerwallettransaction_primitive_orm* WalletPrepaid::get_wallet_transaction(){return transaction_orm;}
int  WalletPrepaid::get_customer_id(){return customer_id;}
BDate WalletPrepaid::get_closing_day(){return closing_day;}

LedgerAmount * WalletPrepaid::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(get_customer_id());
    return lg;
}

LedgerAmount * WalletPrepaid::_get_wallet_amount(LedgerClosureStep *walletPrepaid)
{
    double amount = ((WalletPrepaid*)walletPrepaid)->get_wallet_transaction()->get_amount();
    LedgerAmount * ledgerAmount = ((WalletPrepaid*)walletPrepaid)->_init_ledger_amount();
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}
void WalletPrepaid::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{ 
    if (provider_name.compare("paymob")==0){
        cout<<"inside paymob"<<endl;
        ledgerClosureService->addHandler("Repayment from accept to wallet", WalletPrepaid::_get_wallet_amount);

    }
    else if (provider_name.compare("fawry")==0){
        cout<<"inside fawry"<<endl;

        ledgerClosureService->addHandler("Repayment from fawry to wallet", WalletPrepaid::_get_wallet_amount);

    }
    else if (provider_name.compare("blnk")==0){
        cout<<"inside blnk"<<endl;

        if (method_name.find("pay") != std::string::npos) {

            ledgerClosureService->addHandler("Repayment from finance to wallet", WalletPrepaid::_get_wallet_amount);

        }
        else if (method_name.find("alex") != std::string::npos){
            ledgerClosureService->addHandler("Repayment from collection Alex to wallet prepaid", WalletPrepaid::_get_wallet_amount);

        }
        else if (method_name.find("collection") != std::string::npos){
            ledgerClosureService->addHandler("Repayment from collection to wallet", WalletPrepaid::_get_wallet_amount);

        }
        else if (method_name.find("blnk") != std::string::npos){
            ledgerClosureService->addHandler("Repayment from blnk to wallet prepaid", WalletPrepaid::_get_wallet_amount);

        }
    }
    
    
}


void WalletPrepaid::stampORMs(ledger_amount_primitive_orm * la_orm)
 
{
   transaction_orm->setUpdateRefernce("wallet_ledger_amount_id", la_orm);
}

new_lms_customerwallettransaction_primitive_orm_iterator* WalletPrepaid::aggregator(QueryExtraFeilds * query_fields)
{
    new_lms_customerwallettransaction_primitive_orm_iterator * wallet_transactions_iterator = new new_lms_customerwallettransaction_primitive_orm_iterator ("main");


        wallet_transactions_iterator->filter(
            ANDOperator 
            (
                new UnaryOperator ("wallet_ledger_amount_id",isnull,"",true),
                new UnaryOperator("amount", gt, 0),
                new UnaryOperator("order_id", isnull,"",true),
                new UnaryOperator("created_at::date",lte, query_fields->closure_date_string),
                query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
                query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
              
            )
        );

        wallet_transactions_iterator->addExtraFromField("(select nlc2.customer_id  from  new_lms_customerwallet nlc2 where  new_lms_customerwallettransaction.customer_wallet_id = nlc2.id )","customer_id");
        wallet_transactions_iterator->addExtraFromField("(select pp2.name  from payments_paymentmethod pp inner join payments_paymentprovider pp2 on pp2.id = pp.provider_id where pp.id = new_lms_customerwallettransaction.payment_method_id)","payment_provider_name");
        wallet_transactions_iterator->addExtraFromField("(select pp.name  from  payments_paymentmethod pp where  pp.id = new_lms_customerwallettransaction.payment_method_id)","payment_method_name");

        return wallet_transactions_iterator;

}   


void WalletPrepaid::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::WALLET_PREPAID)}}

        );
    psqlUpdateQuery.update(); 
}



WalletPrepaid::~WalletPrepaid(){}
