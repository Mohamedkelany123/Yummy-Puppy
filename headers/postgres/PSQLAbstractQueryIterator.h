#ifndef PSQLABSTRACTQUERYITERATOR_H
#define PSQLABSTRACTQUERYITERATOR_H

#include <PSQLAbstractORM.h>
enum unary_operator { eq, gt, lt, gte,lte,ne,nand, in,nin };

class Expression{

    private:
    public:
        Expression(){}
        virtual string const generate () const = 0  ;
        virtual ~Expression(){}
};

class LogicalOperator : public Expression {
    protected:
        vector <Expression *> expressions;
        string op;

    public:
        LogicalOperator(): Expression() {}
        LogicalOperator(string _op,Expression * expression, ...): Expression() {
            op = _op;
            expressions.push_back(expression);
            va_list expression_list;
            va_start (expression_list, expression);
            while (true) {
                Expression * e = va_arg (expression_list, Expression *);
                if (e == NULL) break;
                expressions.push_back (e);
            }
            va_end(expression_list);
        }
        string const generate() const {
            string exp = "";
            for (Expression * e:expressions)
            {
                if (exp == "") exp = "(";
                else exp += " "+op+" ";
                exp += e->generate();
            }
            if (exp != "") exp += ")";
            return exp;
        }
        virtual ~LogicalOperator()
        {
            for (Expression * e:expressions)
                delete(e);
        }
};

class OROperator : public LogicalOperator {
    private:
    public:
        template<typename ... Args>
        OROperator(Expression * e,Args&& ... args) : LogicalOperator("OR",e,std::forward<Args>(args) ...,NULL){}
        virtual ~OROperator(){}
};

class ANDOperator : public LogicalOperator {
    private:
    public:
        template<typename ... Args>
        ANDOperator(Expression * e,Args&& ... args) : LogicalOperator("AND",e,std::forward<Args>(args) ...,NULL){}
        virtual ~ANDOperator() {}
};



class UnaryOperator : public Expression {
    private:
        string name;
        unary_operator op;
        string value;
        bool value_is_field_name;
    public:
        UnaryOperator(string _name,unary_operator _op,string _value,bool _value_is_field_name=false){
            name = _name;
            op = _op;
            value = _value;
            value_is_field_name = _value_is_field_name;
        }
        UnaryOperator(string _name,unary_operator _op,int _value,bool _value_is_field_name=false){
            name = _name;
            op = _op;
            value = to_string(_value);
            value_is_field_name = _value_is_field_name;
        }
        UnaryOperator(string _name,unary_operator _op,long _value,bool _value_is_field_name=false){
            name = _name;
            op = _op;
            value = _value;
            value_is_field_name = _value_is_field_name;
        }
        string const generate() const
        {
            if ( value_is_field_name)
            {
                if ( op == eq ) return name+" = "+value;
                else if ( op == gt ) return name+" > "+value;
                else if ( op == gte ) return name+" >= "+value;
                else if ( op == lt ) return name+" < "+value;
                else if ( op == lte ) return name+" <= "+value;
                else if ( op == ne ) return name+" <> "+value;
                else if ( op == in ) return name+" in ("+value+")";
                else if ( op == nin ) return name+" not in ("+value+")";
                else return "";
            }
            else
            {
                if ( op == eq ) return name+" = '"+value+"'";
                else if ( op == gt ) return name+" > '"+value+"'";
                else if ( op == gte ) return name+" >= '"+value+"'";
                else if ( op == lt ) return name+" < '"+value+"'";
                else if ( op == lte ) return name+" <= '"+value+"'";
                else if ( op == ne ) return name+" <> '"+value+"'";
                else if ( op == in ) return name+" in ("+value+")";
                else if ( op == nin ) return name+" not in ("+value+")";
                else return "";
            }
        }
        virtual ~UnaryOperator(){}
};


class PSQLAbstractQueryIterator {

    protected:
        PSQLConnection * psqlConnection;
        AbstractDBQuery * psqlQuery;
        string data_source_name;
        string table_name;
        string conditions;
        string sql;
        string from_string;
        string orderby_string;
    public:
        PSQLAbstractQueryIterator(string _data_source_name,string _table_name);
        void setNativeSQL(string _sql);
        void filter ( Expression const & e);
        bool execute();
        long getResultCount();
        virtual ~PSQLAbstractQueryIterator();
};


class PSQLJoinQueryIterator: public PSQLAbstractQueryIterator {

    protected:
        string column_names = "";
        string join_string = "";
        vector <PSQLAbstractORM *> * orm_objects;
        void unlock_orms (map <string,PSQLAbstractORM *> *  orms);
        static void process_internal(PSQLJoinQueryIterator * me,PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock)> f);
    public:
        PSQLJoinQueryIterator(string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields);
        map <string,PSQLAbstractORM *> * next ();
        void process(int partitions_count,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock)> f);
        // void setNativeSQL(string _sql);
        // void filter ( Expression const & e);
        // bool execute();
        ~PSQLJoinQueryIterator();
};


template <class T>
class PSQLQueryPartitionIterator {
    private:
        AbstractDBQuery * psqlQuery;
    public:
        PSQLQueryPartitionIterator (AbstractDBQuery * _psqlQuery){ psqlQuery = _psqlQuery;}
        T * next ()
        {
            if (psqlQuery->fetchNextRow())
            {
                T * obj = new T();
                // printf ("----- cloning ORM %p \n",obj);
                obj->assignResults(psqlQuery);
                return obj;
            }
            else return NULL;
        }
        ~PSQLQueryPartitionIterator (){}
};

class PSQLJoinQueryPartitionIterator {
    private:
        AbstractDBQuery * psqlQuery;
        vector <PSQLAbstractORM *> * orm_objects;
    public:
        PSQLJoinQueryPartitionIterator (AbstractDBQuery * _psqlQuery,vector <PSQLAbstractORM *> * _orm_objects){ 
                psqlQuery = _psqlQuery;
                orm_objects = _orm_objects;
        }
        map <string,PSQLAbstractORM *> * next ()
        {
            if (psqlQuery->fetchNextRow())
            {
                map <string,PSQLAbstractORM *> * results  = new map <string,PSQLAbstractORM *>();
                for (auto orm_object: *orm_objects) 
                {
                    PSQLAbstractORM * orm = orm_object->clone();
                    // printf ("cloning ORM %p from %p\n",orm,orm_object);
                    // cout << "before assignresults" << endl;
                    orm->assignResults(psqlQuery);
                    // cout << "after assignresults" << endl;
                    (*results)[orm->getTableName()] = orm;
                }
                return results;
            }
            else return NULL;
        }
        ~PSQLJoinQueryPartitionIterator (){}
};


#endif