#ifndef PSQLBOOL_H
#define PSQLBOOL_H

#include <AbstractDatabaseColumn.h>

class PSQLBool: public AbstractDatabaseColumn{
    public:
        PSQLBool();
        PSQLBool(string _column_name);
        static string get_native_type(int index =0);
        string genDeclaration ();
        string genSetter (string class_name);
        string genGetter (string class_name);
        string genSetterDef ();
        string genGetterDef ();
        string genFieldConversion (string field);
        AbstractDatabaseColumn * clone (string _column_name);
        ~PSQLBool();
};

#endif