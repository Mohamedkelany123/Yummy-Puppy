#ifndef PSQLINT8_H
#define PSQLINT8_H

#include <AbstractDatabaseColumn.h>

class PSQLInt8: public AbstractDatabaseColumn{
    public:
        PSQLInt8();
        PSQLInt8(string column_name);
        static string get_native_type();
        string genDeclaration ();
        string genSetter (string class_name);
        string genGetter (string class_name);
        string genSetterDef ();
        string genGetterDef ();
        string genFieldConversion (string field);
        AbstractDatabaseColumn * clone (string column_name);
        ~PSQLInt8();
};

#endif