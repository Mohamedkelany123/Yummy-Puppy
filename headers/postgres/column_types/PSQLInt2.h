#ifndef PSQLINT2_H
#define PSQLINT2_H

#include <AbstractDatabaseColumn.h>

class PSQLInt2: public AbstractDatabaseColumn{
    public:
        PSQLInt2();
        PSQLInt2(string _column_name);
        static string get_native_type(int index =0);
        string genDeclaration ();
        string genSetter (string class_name);
        string genGetter (string class_name);
        string genSetterDef ();
        string genGetterDef ();
        string genFieldConversion (string field);
        AbstractDatabaseColumn * clone (string _column_name);
        ~PSQLInt2();
};

#endif