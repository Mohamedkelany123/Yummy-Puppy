#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLPrimitiveORMGenerator.h>
#include <PSQLController.h>
#include <reader.h>
//g++ -shared -fPIC -std=c++20 ./factory/db_primitive_orm/sources/crm_app_customer_primitive_orm.cpp $(ls ./objects | grep -v orm_c++|awk '{print "./objects/" $1}') -o ./dso/crm_app_customer_primitive_orm.so -I./headers/postgres/ -I ./ -I./headers/abstract -I /usr/local/Cellar/libpq/16.0/include/ -I ./factory/db_primitive_orm/headers/ -L/usr/local/Cellar/libpq/16.0/lib -lpthread -ldl -lpq

typedef PSQLAbstractORM * create_object_routine();

void generate(const ConfigReader& conf);
void clean(const ConfigReader& conf);

int main (int argc, char ** argv)
{

    if (argc != 3 && argc != 4) {
        printf("usage: %s <command:generate,clean> <config file>\n",argv[0]);
        return -1;
    }

    string command = argv[1];
    ConfigReader conf = ConfigReader(argv[2]);


    if (command == "generate") {
        generate(conf);
    } else if (command == "clean") {
        clean(conf);
    } else {
        printf("%s is not a command. Possible commands are: generate, clean\n", command.c_str());
        return -1;
    }
    
    // if (argc == 4){
    //     for ( int i = 0 ; i  < tables.size() && i < 20 ; i ++)
    //     {
    //         cout << "Compiling " << tables[i] << endl;
    //         psqlPrimitiveORMGenerator->compile(tables[i]);
    //     }
    // }

    return 0;
}

void generate(const ConfigReader& conf) {
    
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


    vector<string> views = psqlConnection->getViewNames();


    string orm_folder = conf.GetValue("factory", "directory");
    PSQLPrimitiveORMGenerator * psqlPrimitiveORMGenerator = new PSQLPrimitiveORMGenerator(datasource_key, orm_folder);
    for ( int i = 0 ; i  < tables.size() ; i ++)
    {
        cout << "Generating Table " << tables[i] << endl;
        psqlPrimitiveORMGenerator->generate(tables[i],std::to_string(i),tables);
    }


    for ( int i = tables.size() ; i  < views.size() ; i ++)
    {
        cout << "Generating View " << views[i] << endl;
        psqlPrimitiveORMGenerator->generate(views[i],std::to_string(i),views,true);
    }

    psqlController.releaseConnection(datasource_key, psqlConnection);
    delete (psqlPrimitiveORMGenerator);
}

void clean(const ConfigReader& conf) {

    string orm_folder = conf.GetValue("factory", "directory");

    string delete_headers = "rm -rf " + orm_folder + H_FOLDER + "/*.h";
    string delete_sources = "rm -rf " + orm_folder + CPP_FOLDER + "/*.cpp";

    cout << delete_headers << endl;
    int status1 = system (delete_headers.c_str());
    if (status1 != 0) {
        cout << "Failed to delete headers\n";
    }

    cout << delete_sources << endl;
    int status2 = system (delete_sources.c_str());
    if (status2 != 0) {
        cout << "Failed to delete sources\n";
    }
}