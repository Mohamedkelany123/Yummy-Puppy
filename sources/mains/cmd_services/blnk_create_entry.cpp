#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>

int main (int argc, char ** argv)
{
    int threadsCount = 1;
    bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus","postgres","postgres");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);
    
    // json entry_json = { {"Booking new loan - short term; and", { {"amount" ,2000.00}, {"loan_id", 133416}, {"customer_id", 359786}, {"cashier_id", 126935}, {"merchant_id", 2} }} };
    LedgerAmount demo(1, "Demo", 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201.50, 1301, true);
    std::map<std::string, LedgerAmount> temp;
    temp["Booking new loan - short term; and"] = demo;



    BlnkTemplateManager blnkTemplateManager(4, temp);

    return 0;
}

// int main (int argc, char ** argv)
// {
//     int threadsCount = 1;
//     psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus","postgres","postgres");
//     // cout << "Connected to " << database_name << endl;
    
//     psqlController.addDefault("created_at","now()",true,true);
//     psqlController.addDefault("updated_at","now()",true,true);
//     psqlController.addDefault("updated_at","now()",false,true);
//     psqlController.setORMCacheThreads(threadsCount);

//     PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
//         {new lms_entrytemplate_primitive_orm("main"),new ledger_entry_primitive_orm("main")},
//         {{{"lms_entrytemplate","id"},{"ledger_entry","template_id"}}});



//         psqlQueryJoin->filter(
//             ANDOperator 
//             (
//                 new UnaryOperator ("lms_entrytemplate.id",eq,4)
//             )
//         );

//     psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
//         lms_entrytemplate_primitive_orm * lep_orm = ORM(lms_entrytemplate, orms);
//         json _template = lep_orm->get_template();
//         cout << "TEMPLATE" << _template.dump();
//     });
    
//     cout << "hi" << endl;
//     return 0;
// }
