#include <PSQLAbstractORM.h>

PSQLAbstractORM::PSQLAbstractORM (string _table_name,string _identifier_name)
{
    table_name = _table_name;
    identifier_name = _identifier_name;
    // psqlConnection = new PSQLConnection ("localhost",5432,"django_ostaz_15082023_old","postgres","postgres");
    // psqlQuery = NULL;
    // map<string, vector<string>> results  = psqlQuery->getResultAsString();
    // for (auto result: results) {
    //     std::cout << "ttt" << std::endl;
    //     std::cout << result.first  << std::endl;
    //     for (auto value: result.second) 
    //         std::cout << "\t\t" << value << std::endl;
    // }
}
string PSQLAbstractORM::getIdentifierName ()
{
    return identifier_name;
}

PSQLAbstractORM::~PSQLAbstractORM()
{
    cout << "PSQLAbstractORM::~PSQLAbstractORM()" << endl;
    // if (psqlConnection != NULL) delete (psqlConnection);
    // if (psqlQuery != NULL) delete (psqlQuery);
}
