#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLPrimitiveORMGenerator.h>
#include <PSQLController.h>
// #include <crm_app_customer_primitive_orm.h>
// #include <loan_app_loan_primitive_orm.h>
// #include <auth_group_primitive_orm.h>
//g++ -shared -fPIC -std=c++20 ./factory/db_primitive_orm/sources/crm_app_customer_primitive_orm.cpp $(ls ./objects | grep -v orm_c++|awk '{print "./objects/" $1}') -o ./dso/crm_app_customer_primitive_orm.so -I./headers/postgres/ -I ./ -I./headers/abstract -I /usr/local/Cellar/libpq/16.0/include/ -I ./factory/db_primitive_orm/headers/ -L/usr/local/Cellar/libpq/16.0/lib -lpthread -ldl -lpq



typedef PSQLAbstractORM * create_object_routine();

int main (int argc, char ** argv)
{
/*
select * from (
    SELECT conrelid::regclass AS "FK_Table"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), 14, position(')' in pg_get_constraintdef(c.oid))-14) END AS "FK_Column"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), position(' REFERENCES ' in pg_get_constraintdef(c.oid))+12, position('(' in substring(pg_get_constraintdef(c.oid), 14))-position(' REFERENCES ' in pg_get_constraintdef(c.oid))+1) END AS "PK_Table"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), position('(' in substring(pg_get_constraintdef(c.oid), 14))+14, position(')' in substring(pg_get_constraintdef(c.oid), position('(' in substring(pg_get_constraintdef(c.oid), 14))+14))-1) END AS "PK_Column"
    FROM   pg_constraint c
    JOIN   pg_namespace n ON n.oid = c.connamespace
    WHERE  contype IN ('f', 'p') AND
    pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %'
    ORDER  BY pg_get_constraintdef(c.oid), conrelid::regclass::text, contype DESC) as t
where "PK_Table" = 'ledger_entry';

SELECT               
  pg_attribute.attname, 
  format_type(pg_attribute.atttypid, pg_attribute.atttypmod) 
FROM pg_index, pg_class, pg_attribute, pg_namespace 
WHERE 
  pg_class.oid = 'new_lms_installmentextension'::regclass AND 
  indrelid = pg_class.oid AND 
  nspname = 'public' AND 
  pg_class.relnamespace = pg_namespace.oid AND 
  pg_attribute.attrelid = pg_class.oid AND 
  pg_attribute.attnum = any(pg_index.indkey)
 AND indisprimary


SELECT c.conname                                 AS constraint_name,
   c.contype                                     AS constraint_type,
   sch.nspname                                   AS "self_schema",
   tbl.relname                                   AS "self_table",
   ARRAY_AGG(col.attname ORDER BY u.attposition) AS "self_columns",
   f_sch.nspname                                 AS "foreign_schema",
   f_tbl.relname                                 AS "foreign_table",
   ARRAY_AGG(f_col.attname ORDER BY f_u.attposition) AS "foreign_columns",
   pg_get_constraintdef(c.oid)                   AS definition
FROM pg_constraint c
       LEFT JOIN LATERAL UNNEST(c.conkey) WITH ORDINALITY AS u(attnum, attposition) ON TRUE
       LEFT JOIN LATERAL UNNEST(c.confkey) WITH ORDINALITY AS f_u(attnum, attposition) ON f_u.attposition = u.attposition
       JOIN pg_class tbl ON tbl.oid = c.conrelid
       JOIN pg_namespace sch ON sch.oid = tbl.relnamespace
       LEFT JOIN pg_attribute col ON (col.attrelid = tbl.oid AND col.attnum = u.attnum)
       LEFT JOIN pg_class f_tbl ON f_tbl.oid = c.confrelid
       LEFT JOIN pg_namespace f_sch ON f_sch.oid = f_tbl.relnamespace
       LEFT JOIN pg_attribute f_col ON (f_col.attrelid = f_tbl.oid AND f_col.attnum = f_u.attnum)
GROUP BY constraint_name, constraint_type, "self_schema", "self_table", definition, "foreign_schema", "foreign_table"
ORDER BY "self_schema", "self_table";

*/

    if (argc == 6 || argc == 7)
    {
        // PSQLConnectionManager * psqlConnectionManager = new PSQLConnectionManager();
        psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
        printf ("Datasource created\n"); 
        PSQLConnection * psqlConnection = psqlController.getPSQLConnection("main");
        printf ("PsqlConnection created: %p\n",psqlConnection);
        vector<string> tables = psqlConnection->getTableNames();
        PSQLPrimitiveORMGenerator * psqlPrimitiveORMGenerator = new PSQLPrimitiveORMGenerator();
        for ( int i = 0 ; i  < tables.size() ; i ++)
        {
            cout << "Generating " << tables[i] << endl;
            psqlPrimitiveORMGenerator->generate(tables[i],std::to_string(i));
        }
        if (argc == 7)
        {
            for ( int i = 0 ; i  < tables.size() && i < 20 ; i ++)
            {
                cout << "Compiling " << tables[i] << endl;
                psqlPrimitiveORMGenerator->compile(tables[i]);
            }
        }
        psqlController.releaseConnection("main",psqlConnection);
        delete (psqlPrimitiveORMGenerator);
    }
    else 
    {
        printf("usage: %s <address> <port_number> <database name> <username> <password>\n",argv[0]);
        return -1;
    }
    // else
    // {
    //     void * dso=dlopen ("./dso/auth_group_primitive_orm.so",RTLD_LAZY);
    //     if ( dso ){ 
    //         create_object_routine * my_func = (create_object_routine *) dlsym (dso,"create_object");
    //         printf ("DSO Loaded \n");
    //         auth_group_primitive_orm * x = (auth_group_primitive_orm *) my_func();
    //         printf ("Instantiated \n");
    //         for ( int i = 0 ; i < 10 ; i ++)
    //         {
    //             x->assignResults();
    //             cout << x->get_id() << endl;
    //         }
    //         delete (x);
    //         dlclose(dso);
    //     }
    // }
    // OROperator x 
    // (
    //     new ANDOperator(new UnaryOperator ("national_id",eq,"102910291029109"),
    //             new UnaryOperator ("national_id",eq,"102910291029109"),
    //             new UnaryOperator ("national_id",eq,"102910291029109")),
    //     new ANDOperator(new UnaryOperator ("national_id",eq,"102910291029109"),
    //             new UnaryOperator ("national_id",eq,"102910291029109"),
    //             new UnaryOperator ("national_id",eq,"102910291029109")));

    // cout << x.generate() << endl;
    // exit(1);

    // printf ("Hello :) This is ORMC++\n");
    // PSQLConnection * psqlConnection = new PSQLConnection ("localhost",5432,"django_ostaz_15082023_old","postgres","postgres");
    // AbstractDBQuery *psqlQuery = psqlConnection->executeQuery("select * from loan_app_loan limit 10");

    // for (int i = 0  ;  i < psqlQuery->getColumnCount() ; i ++)
    //     std::cout << psqlQuery->getColumnName(i)<< " : " << psqlQuery->getColumnType(i) << " : " << psqlQuery->getColumnSize(i)<< std::endl; 
    // psqlQuery->fetchNextRow();
    // map<string, vector<string>> results  = psqlQuery->getResultAsString();
    // for (auto result: results) {
    //     std::cout << "ttt" << std::endl;
    //     std::cout << result.first  << std::endl;
    //     for (auto value: result.second) 
    //         std::cout << "\t\t" << value << std::endl;
    // }
    // delete (psqlQuery);
    // delete (psqlConnection);
    return 0;
}