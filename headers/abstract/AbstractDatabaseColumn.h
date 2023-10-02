#ifndef ABSTRACTDATABASECOLUMN_H
#define ABSTRACTDATABASECOLUMN_H

#include <common.h>

class AbstractDatabaseColumn{
    protected:
        string column_name;
        string field_name;
    public:
        AbstractDatabaseColumn(){ column_name = ""; }
        AbstractDatabaseColumn(string _column_name){ column_name = _column_name; field_name= "orm_"+column_name;}
        static string get_native_type(int index =0){return "";};
        virtual string genDeclaration () = 0;
        virtual string genSetter (string class_name) = 0;
        virtual string genGetter (string class_name) = 0;
        virtual string genSetterDef () = 0;
        virtual string genGetterDef () = 0;
        virtual string genFieldConversion (string field)=0;
        virtual AbstractDatabaseColumn * clone (string column_name) = 0;
        virtual ~AbstractDatabaseColumn(){}
};

#endif