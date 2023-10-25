#ifndef PSQLJSON_H
#define PSQLJSON_H

#include <AbstractDatabaseColumn.h>

class PSQLJson: public AbstractDatabaseColumn{
    public:
        PSQLJson();
        PSQLJson(string _column_name);
        static string get_native_type(int index =0);
        string genDeclaration ();
        string genSetter (string class_name,int col_index);
        string genGetter (string class_name);
        string genSetterDef ();
        string genGetterDef ();
        string genFieldConversion (string field);
        AbstractDatabaseColumn * clone (string _column_name);
        ~PSQLJson();
};

#endif