#include <Unmarginalize.h>


Unmarginalize::Unmarginalize(loan_app_loan_primitive_orm * loan,int ins_id,new_lms_installmentextension_primitive_orm * installment_ext, float unmarginalized_inst_amount ,vector<new_lms_installmentlatefees_primitive_orm *>* lf_list)
{
    lal_orm = loan;
    installment_id = ins_id;
    installment_extension = installment_ext;
    late_fees = lf_list;
    unmarginalized_amount = unmarginalized_inst_amount;
}


void Unmarginalize::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Reversing marginalized late repayment fee income, if applicable",_unmarginalize_late_fee);
    ledgerClosureService->addHandler("Reversing marginalized interest income, if applicable",_unmarginalize_interest);
}

LedgerAmount*  Unmarginalize::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setInstallmentId(installment_id);
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}

LedgerAmount * Unmarginalize::_unmarginalize_late_fee(LedgerClosureStep *unmarginalize){
    vector<new_lms_installmentlatefees_primitive_orm*>* late_fees = ((Unmarginalize*)unmarginalize)->get_late_fees();
    LedgerAmount * la = ((Unmarginalize*)unmarginalize)->_init_ledger_amount();
    double amount = 0.0;
    if(late_fees->size() > 0){
        for(const auto& element : *late_fees){
            amount += element->get_amount();
        }
    }
    la->setAmount(amount);
    return la;
}

LedgerAmount * Unmarginalize::_unmarginalize_interest(LedgerClosureStep *unmarginalize){
    new_lms_installmentextension_primitive_orm * installment_extension =  ((Unmarginalize*)unmarginalize)->get_installment_extension();
    LedgerAmount * la = ((Unmarginalize*)unmarginalize)->_init_ledger_amount();
    double amount = 0.0;
    // cout << "UNMARGINALIZE AMOUNT IN INFERENCE FUNC : " << ((Unmarginalize*)unmarginalize)->get_unmarginalized_amount() << endl;
    la->setAmount(((Unmarginalize*)unmarginalize)->get_unmarginalized_amount());

    return la;
}

new_lms_installmentextension_primitive_orm * Unmarginalize::get_installment_extension(){
    return installment_extension;
}

vector<new_lms_installmentlatefees_primitive_orm*> * Unmarginalize::get_late_fees(){
    return late_fees;
}

int Unmarginalize::get_installment_id(){
    return installment_id;
}

float Unmarginalize::get_unmarginalized_amount(){
    return unmarginalized_amount;
}

void Unmarginalize::stampORMs(map<string,LedgerCompositLeg*> * amounts){
    for(const auto& pair : *amounts){
        ledger_amount_primitive_orm* first_leg_amount = pair.second->getLedgerCompositeLeg()->first;
        if(first_leg_amount != NULL){
            if(pair.second->getLegId() == 1){
                for(new_lms_installmentlatefees_primitive_orm * late_fee : *late_fees){
                    late_fee->setUpdateRefernce("unmarginalization_ledger_amount_id",first_leg_amount);
                }
        }
        else if(pair.second->getLegId() == 2){
            installment_extension->setUpdateRefernce("unmarginalization_ledger_amount_id",first_leg_amount);
        }
        }
        else{
            cout << "ERROR in fetching first leg of the entry " << endl;
        }   
    }
}

PSQLJoinQueryIterator* Unmarginalize::aggregator(QueryExtraFeilds * query_fields){

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_primitive_orm("main"), new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"),new new_lms_installmentlatefees_primitive_orm("main")},
        { {{"loan_app_loan", "id"}, {"loan_app_installment", "loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}, {{"new_lms_installmentlatefees", "installment_extension_id"}, {"loan_app_installment", "id"}}});

        psqlQueryJoin->filter(
            ANDOperator(
                new OROperator(
                    new ANDOperator(
                        // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::REVERSE_MARGINALIZATION-1)),
                        new UnaryOperator ("loan_app_loan.id" , ne, "14312"),
                        new UnaryOperator ("loan_app_loan.id" , in, "2"),
                        new UnaryOperator ("new_lms_installmentlatefees.unmarginalization_ledger_amount_id" , isnull, "",true),
                        new UnaryOperator ("new_lms_installmentlatefees.marginalization_ledger_amount_id" , isnotnull, "",true),


                        new OROperator(
                            new ANDOperator(
                                new UnaryOperator ("new_lms_installmentlatefees.is_paid" , eq, true),
                                new UnaryOperator ("new_lms_installmentlatefees.paid_at" ,lte,query_fields->closure_date_string)
                            ),
                            new ANDOperator(
                                new UnaryOperator ("new_lms_installmentlatefees.is_cancelled" , eq, true),
                                new UnaryOperator ("new_lms_installmentlatefees.cancellation_date" ,lte,query_fields->closure_date_string)
                            ) 
                        ) 
                    ),
                    new ANDOperator (
                        // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::REVERSE_MARGINALIZATION-1)),
                        new UnaryOperator ("loan_app_loan.id" , ne, "14312"),
                        new UnaryOperator ("loan_app_loan.id" , in, "2"),
                        new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,true),
                        new UnaryOperator ("new_lms_installmentextension.interest_paid_at",lte,query_fields->closure_date_string),
                        new UnaryOperator ("new_lms_installmentextension.unmarginalization_ledger_amount_id",isnull,"",true),
                        new OROperator(                
                            new UnaryOperator ("new_lms_installmentextension.marginalization_ledger_amount_id",isnotnull,"",true),
                            new UnaryOperator ("new_lms_installmentextension.partial_marginalization_ledger_amount_id",isnotnull,"",true)
                        )
                    )
                ),
                query_fields->isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,query_fields->mod_value,eq,query_fields->offset) : new BinaryOperator(),
                query_fields->isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, query_fields->loan_ids) : new UnaryOperator()
            )
        );

        psqlQueryJoin->addExtraFromField("(select amount from ledger_amount la where la.id = new_lms_installmentextension.partial_marginalization_ledger_amount_id limit 1)","partial_marginalization_amount");
        psqlQueryJoin->addExtraFromField("(select amount from ledger_amount la where la.id = new_lms_installmentextension.marginalization_ledger_amount_id limit 1)","marginalization_amount");

        psqlQueryJoin->setAggregates({
            {"loan_app_loan", {"id", 1}}});

        psqlQueryJoin->setOrderBy("loan_app_loan.id asc,  new_lms_installmentextension.installment_ptr_id asc, new_lms_installmentlatefees.id asc");

        return psqlQueryJoin;
}


 void Unmarginalize::update_step(){

        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::REVERSE_MARGINALIZATION)}}
        );
        psqlUpdateQuery.update();   
}


map <string,map<int,pair<pair<new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *> *> * Unmarginalize::get_date_map(vector<map <string,PSQLAbstractORM *> * > * orms){
    map <string,map<int,pair<pair<new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *> *> * date_map = new map <string,map<int,pair<pair< new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *> *>  ();
    new_lms_installmentextension_primitive_orm * ext = ORML(new_lms_installmentextension,orms,0);
    new_lms_installmentlatefees_primitive_orm * lf = ORML(new_lms_installmentlatefees,orms,0);

    int first_id = 0;
    int last_id = 0;
        for ( int i = 0 ;i < ORML_SIZE(orms) ; i ++)
        {   
            ext = ORML(new_lms_installmentextension,orms,i);
            // cout << "INSTALLMENT ID: " << ext->get_installment_ptr_id() << endl;
            PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms,i);
            float partial_marginalization_amount = gorm->toFloat("partial_marginalization_amount");
            // cout << "PARTIAL MARGINALIZATION AMOUNT : " << partial_marginalization_amount << endl;
            float marginalization_amount = gorm->toFloat("marginalization_amount");
            // cout << "MARGINALIZATION AMOUNT : " << marginalization_amount << endl;
            float unmarginalized_amount = 0.0;
            if(ext->get_partial_marginalization_ledger_amount_id() == ext->get_marginalization_ledger_amount_id()){
                unmarginalized_amount = partial_marginalization_amount;
            }
            else{
                unmarginalized_amount = partial_marginalization_amount + marginalization_amount;
            }
            // cout << "AMOUNT TO BE UNMARGINALIZED : " << unmarginalized_amount << endl;
            string curr_date = ext->get_unmarginalization_date();
            int ext_unmarginalization_amount_id = ext->get_unmarginalization_ledger_amount_id();
            int curr_id = ext->get_installment_ptr_id();
            lf = ORML(new_lms_installmentlatefees,orms,i);
            int curr_lf_inst_id = lf->get_installment_extension_id();
            string  curr_lf_date=lf->get_unmarginalization_date();
            int lf_unmarginalization_amount_id = lf->get_unmarginalization_ledger_amount_id();
            if (curr_date!="" && ext_unmarginalization_amount_id == 0){
                auto iter = date_map->find(curr_date);
               
                if (iter!= date_map->end()){ 
                    auto iter_nest =  iter->second->find(curr_id);
                    if(iter_nest== iter->second->end()){
                      pair< pair< new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> * partition = new pair<pair<new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> ;
                      partition->first = new pair<new_lms_installmentextension_primitive_orm*,float>();
                      partition->first->first = ext;
                      partition->first->second = unmarginalized_amount;
                      partition->second= new vector<new_lms_installmentlatefees_primitive_orm*> ();
                      date_map->operator[](curr_date)->operator[](curr_id)= partition;

                    }
                }
                else{
                    map<int, pair< pair<new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *>* new_map = new map<int,pair<pair< new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *>();
                    pair<pair< new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> * partition = new pair<pair< new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *>() ;
                    partition->first = new pair<new_lms_installmentextension_primitive_orm*,float>();
                    partition->first->first=ext;
                    partition->first->second = unmarginalized_amount;
                    partition->second= new vector<new_lms_installmentlatefees_primitive_orm*>();
                    new_map->operator[](curr_id)=partition;
                    date_map->operator[](curr_date)=new_map;
                }

            }
            if (curr_lf_date!="" && lf_unmarginalization_amount_id == 0 ){
                auto iter = date_map->find(curr_date);
                if (iter!= date_map->end()){


                    auto iter_nest =  iter->second->find(curr_lf_inst_id);
                    if(iter_nest== iter->second->end()){
                        pair<pair< new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> * partition = new pair< pair<new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> ;
                        partition->first =  new pair<new_lms_installmentextension_primitive_orm*,float>();
                        partition->first->first=nullptr ;
                        partition->first->second = 0.0;
                        partition->second= new vector<new_lms_installmentlatefees_primitive_orm*> ();
                        partition->second->push_back(lf);
                        date_map->operator[](curr_date)->operator[](curr_lf_inst_id)= partition;

                    }
                    else{
                        date_map->operator[](curr_date)->operator[](curr_lf_inst_id)->second->push_back(lf);
                    }
                }
                else{
                    map<int, pair<pair< new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *>* new_map = new map<int,pair<pair< new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *>();
                    pair<pair< new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> * partition = new pair<pair< new_lms_installmentextension_primitive_orm *,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> () ;
                    partition->first =  new pair<new_lms_installmentextension_primitive_orm*,float>();
                    partition->first->first=nullptr ;
                    partition->first->second = 0.0;
                    partition->second= new vector<new_lms_installmentlatefees_primitive_orm*> ();
                    partition->second->push_back(lf);
                    new_map->operator[](curr_lf_inst_id)=partition;
                    date_map->operator[](curr_date)=new_map;
                }

             }
    }
    return date_map;
}

Unmarginalize::~Unmarginalize(){}