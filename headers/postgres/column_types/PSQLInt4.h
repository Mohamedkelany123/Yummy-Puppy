#ifndef PSQLINT4_H
#define PSQLINT4_H

#include <AbstractDatabaseColumn.h>

class PSQLInt4: public AbstractDatabaseColumn{
    public:
        PSQLInt4();
        PSQLInt4(string column_name);
        static string get_native_type();
        string genDeclaration ();
        string genSetter (string class_name);
        string genGetter (string class_name);
        string genSetterDef ();
        string genGetterDef ();
        string genFieldConversion (string field);
        AbstractDatabaseColumn * clone (string column_name);
        ~PSQLInt4();
};

#endif