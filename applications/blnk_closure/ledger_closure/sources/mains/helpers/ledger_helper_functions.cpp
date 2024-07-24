#include <common_orm.h>

//<BuckedId,Percentage>
map<int,float> get_loan_status_provisions_percentage()
{
        //Query to return percentage from loan_app_provision
        PSQLJoinQueryIterator * psqlQueryJoinProvisions = new PSQLJoinQueryIterator ("main",
        {new loan_app_loanstatus_primitive_orm("main"),new loan_app_provision_primitive_orm("main")},
        {{{"loan_app_loanstatus","id"},{"loan_app_provision","status_id"}}});

        
        map<int,float> bucket_percentage;

        psqlQueryJoinProvisions->execute();
        map<string, PSQLAbstractORM *>* orms = psqlQueryJoinProvisions->next(true);
        loan_app_loanstatus_primitive_orm * lals_orm;
        loan_app_provision_primitive_orm * lap_orm; 
        while (orms != NULL){
            lals_orm = ORM(loan_app_loanstatus,orms);
            lap_orm = ORM(loan_app_provision,orms);
            bucket_percentage[lals_orm->get_id()] = lap_orm->get_percentage();
            delete(lals_orm);
            delete(lap_orm);
            delete(orms);
            orms = psqlQueryJoinProvisions->next(true);
        }
        delete (psqlQueryJoinProvisions);
        

        return bucket_percentage;
}

float get_iscore_nid_inquiry_fee(){
    ledger_global_primitive_orm_iterator * it = new ledger_global_primitive_orm_iterator("main");
    it->filter(
        ANDOperator(
        new UnaryOperator("ledger_global.name",eq,"iscore_nid_expense_fee")
        )
    );
    it->execute();
    ledger_global_primitive_orm * global_orm = it->next(true);
    if((global_orm->get_value())["amount"] != NULL){
        return global_orm->get_value()["amount"];
    }
    else cout << "ERROR in fetching NID iScore inquiry amount" << endl;

    return -1;
}

float get_iscore_credit_expense_fee(){
    ledger_global_primitive_orm_iterator * it = new ledger_global_primitive_orm_iterator("main");
    it->filter(
        ANDOperator(
        new UnaryOperator("ledger_global.name",eq,"iscore_credit_expense_fee")
        )
    );
    it->execute();
    ledger_global_primitive_orm * global_orm = it->next(true);
    if((global_orm->get_value())["amount"] != NULL){
        return global_orm->get_value()["amount"];
    }
    else cout << "ERROR in fetching iScore Credit expense amount" << endl;

    return -1;
}


vector<string> get_start_and_end_fiscal_year(){
    ledger_global_primitive_orm_iterator * it = new ledger_global_primitive_orm_iterator("main");
    it->filter(
        OROperator(
        new UnaryOperator("ledger_global.name",eq,"start_fiscal_year"),
        new UnaryOperator("ledger_global.name",eq,"end_fiscal_year")
        )
    );
    it->execute();
    vector<string> vector(2);
    for(int i=0; i< 2;i++){
        ledger_global_primitive_orm * global_orm = it->next(true);
        if(global_orm->get_name()== "start_fiscal_year"){
            vector[0] = global_orm->get_value()["date"];
        }
        else{
            vector[1] = global_orm->get_value()["date"];
        }
    }
    return vector;
}
