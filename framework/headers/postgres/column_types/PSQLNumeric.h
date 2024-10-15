#ifndef PSQLNUMERIC_H
#define PSQLNUMERIC_H

#include <AbstractDatabaseColumn.h>

class PSQLNumeric: public AbstractDatabaseColumn{
    public:
        PSQLNumeric();
        PSQLNumeric(string _column_name);
        static string get_native_type(int index =0);
        string genDeclaration ();
        string genSetter (string class_name,int col_index);
        string genGetter (string class_name);
        string genSetterDef ();
        string genGetterDef ();
        string genFieldConversion (string field,int col_index);
        AbstractDatabaseColumn * clone (string _column_name);
        ~PSQLNumeric();
};

#endif