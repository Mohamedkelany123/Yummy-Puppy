#include <common.h>
#include <common_orm.h>
#include <TestSerializer.h>
#include <PSQLController.h>

int main(int argc, char ** argv){
    
    cout << "Hellooooo" << endl;
    
    bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus","postgres","postgres");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(1);



    PSQLJoinQueryIterator * PSQLQueryJoin = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});

    PSQLQueryJoin->addExtraFromField("(select id from loan_app_loan lal where lal.id = loan_app_loan.id)","loan_id");
    PSQLQueryJoin->addExtraFromField("(select id from loan_app_loan lal where lal.id = loan_app_loan.id)","loan_id2");
    PSQLQueryJoin->addExtraFromField("(select id from loan_app_loan lal where lal.id = loan_app_loan.id)","loan_id3");
    
    PSQLQueryJoin->filter(
        ANDOperator (
            new UnaryOperator("loan_app_loan.id", eq, "1666")
        )
    );

    TestSerializer* testSerializer = new TestSerializer("./file.json", PSQLQueryJoin);
    testSerializer->generate(false);

}