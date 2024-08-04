#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLPrimitiveORMGenerator.h>
#include <PSQLController.h>
#include <reader.h>
// #include <crm_app_customer_primitive_orm.h>
// #include <loan_app_loan_primitive_orm.h>
// #include <auth_group_primitive_orm.h>
//g++ -shared -fPIC -std=c++20 ./factory/db_primitive_orm/sources/crm_app_customer_primitive_orm.cpp $(ls ./objects | grep -v orm_c++|awk '{print "./objects/" $1}') -o ./dso/crm_app_customer_primitive_orm.so -I./headers/postgres/ -I ./ -I./headers/abstract -I /usr/local/Cellar/libpq/16.0/include/ -I ./factory/db_primitive_orm/headers/ -L/usr/local/Cellar/libpq/16.0/lib -lpthread -ldl -lpq

typedef PSQLAbstractORM * create_object_routine();


int main (int argc, char ** argv)
{

    if (argc != 3 && argc != 4) {
        printf("usage: %s <command:generate,clean> <config file>\n",argv[0]);
        return -1;
    }

    ConfigReader conf = ConfigReader(argv[2]);
    string datasource_key = conf.GetValue("factory", "datasource");

    for (auto datasource : conf.GetValue("datasources")) {
        if ((string)datasource["key"] == datasource_key) {
            psqlController.addDataSource(datasource["key"],datasource["ip"],datasource["port"],datasource["dbname"],datasource["username"],datasource["password"]);
            cout << "Added datasource with key " << datasource_key << "\n";
            break;
        }
    }
    PSQLConnection * psqlConnection = psqlController.getPSQLConnection(datasource_key);
    cout << "PsqlConnection created: " <<  psqlConnection->get_name() << "\n";

    vector<string> tables = conf.GetValue("factory", "restrict_orms");
    if (tables.size() == 0) {
        tables = psqlConnection->getTableNames();
    }
    PSQLPrimitiveORMGenerator * psqlPrimitiveORMGenerator = new PSQLPrimitiveORMGenerator();
    for ( int i = 0 ; i  < tables.size() ; i ++)
    {
        cout << "Generating " << tables[i] << endl;
        psqlPrimitiveORMGenerator->generate(tables[i],std::to_string(i),tables);
    }


    if (argc == 4){
        for ( int i = 0 ; i  < tables.size() && i < 20 ; i ++)
        {
            cout << "Compiling " << tables[i] << endl;
            psqlPrimitiveORMGenerator->compile(tables[i]);
        }
    }

    psqlController.releaseConnection(datasource_key, psqlConnection);
    delete (psqlPrimitiveORMGenerator);

    return 0;
}