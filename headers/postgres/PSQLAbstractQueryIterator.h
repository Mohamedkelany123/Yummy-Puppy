#ifndef PSQLABSTRACTQUERYITERATOR_H
#define PSQLABSTRACTQUERYITERATOR_H

#include <PSQLAbstractORM.h>
#include <PSQLController.h>

enum unary_operator { eq, gt, lt, gte,lte,ne,nand, in,nin,isnull,isnotnull };
enum math_operator { mod, plus };

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
        UnaryOperator(){
            name = "true";
            op = unary_operator::eq;
            value = "true";
            value_is_field_name = true;
        }
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
                else if ( op == isnull ) return name+" is null";
                else if ( op == isnotnull ) return name+" is not null";
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
                else if ( op == isnull ) return name+" is null";
                else if ( op == isnotnull ) return name+" is not null";
                else return "";
            }
        }
        virtual ~UnaryOperator(){}
};


class BinaryOperator : public Expression {
    private:
        string name;
        math_operator op1;
        string value1;
        unary_operator op2;
        string value2;
        bool value_is_field_name;
    public:
        BinaryOperator(){
            name = "0";
            op1 = math_operator::plus;
            value1 = "0";
            op2 = unary_operator::eq;
            value2 = "0";
            value_is_field_name = true;
        }
        BinaryOperator(string _name, math_operator _op1, string _value1, unary_operator _op2, string _value2, bool _value_is_field_name=false){
            name = _name;
            op1 = _op1;
            value1 = _value1;
            op2 = _op2;
            value2 = _value2;
            value_is_field_name = _value_is_field_name;
        }
        BinaryOperator(string _name,math_operator _op1, int _value1, unary_operator _op2, int _value2,bool _value_is_field_name=false){
            name = _name;
            op1 = _op1;
            value1 = to_string(_value1);
            op2 = _op2;
            value2 = to_string(_value2);
            value_is_field_name = _value_is_field_name;
        }
        BinaryOperator(string _name,math_operator _op1, double _value1, unary_operator _op2, double _value2,bool _value_is_field_name=false){
            name = _name;
            op1 = _op1;
            value1 = to_string(_value1);
            op2 = _op2;
            value2 = to_string(_value2);
            value_is_field_name = _value_is_field_name;
        }
        string const generate() const
        {
            if ( op1 == mod ) return UnaryOperator(name + " % " + value1,op2,value2, value_is_field_name).generate();
            else if ( op1 == math_operator::plus ) return UnaryOperator(name + " + " + value1,op2,value2, value_is_field_name).generate();
            else return "";
        }
        virtual ~BinaryOperator(){}
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
        string distinct;
        map <string,string> extras;
        int partition_number;
    public:
        PSQLAbstractQueryIterator(string _data_source_name,string _table_name, int _partition_number=-1);
        void setNativeSQL(string _sql);
        void filter ( Expression const & e,bool print_sql=false);
        bool execute();
        long getResultCount();
        void setOrderBy(string _orderby_string);
        void setDistinctString(string _distinct_string);
        void addExtraFromField (string field, string field_name);
        virtual ~PSQLAbstractQueryIterator();
};


class PSQLJoinQueryIterator: public PSQLAbstractQueryIterator {

    protected:
        bool aggregate_flag;
        map <string,string> aggregate_map;
        string column_names = "";
        string join_string = "";
        vector <PSQLAbstractORM *> * orm_objects;
        map <string,PSQLAbstractORM *> orm_objects_map;
        void unlock_orms (map <string,PSQLAbstractORM *> *  orms);
        void adjust_orms_list (vector<map <string,PSQLAbstractORM *> *> * orms_list);
        static void process_internal_aggregate(string data_source_name, PSQLJoinQueryIterator * me,PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extras,std::function<void(vector<map <string,PSQLAbstractORM *> *> * orms_list,int partition_number,mutex * shared_lock,void * extras)> f);
        static void process_internal(string data_source_name, PSQLJoinQueryIterator * me,PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extras,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras)> f);
    public:
        PSQLJoinQueryIterator(string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields);
        map <string,PSQLAbstractORM *> * next (bool read_only=false);
        void process(int partitions_count,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras=NULL);
        void process_sequential(std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras=NULL);
        void process_aggregate(int partitions_count,std::function<void(vector<map <string,PSQLAbstractORM *> *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras=NULL);
        void process_aggregate_sequential(std::function<void(vector<map <string,PSQLAbstractORM *> *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras=NULL);
        // void setNativeSQL(string _sql);
        // void filter ( Expression const & e);
        // bool execute();
        bool setDistinct (vector<pair<string,string>> _distinct_map);
        bool setAggregates (map <string,string> _aggregate_map);
        ~PSQLJoinQueryIterator();
};


template <class T>
class PSQLQueryPartitionIterator {
    private:
        AbstractDBQuery * psqlQuery;
        string data_source_name;
        int partition_number;

    public:
        PSQLQueryPartitionIterator (AbstractDBQuery * _psqlQuery, string _data_source_name, void * _extras, int _partition_number=-1){ 
            psqlQuery = _psqlQuery;
            data_source_name = _data_source_name;
            partition_number = _partition_number;
        }
        T * next ()
        {
            if (psqlQuery->fetchNextRow())
            {
                T * obj = new T(data_source_name, false,true,partition_number);
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
        map <string,string> extras;
        int partition_number;

    public:
        PSQLJoinQueryPartitionIterator (AbstractDBQuery * _psqlQuery,vector <PSQLAbstractORM *> * _orm_objects, map <string,string> _extras,int _partition_number){ 
            psqlQuery = _psqlQuery;
            orm_objects = _orm_objects;
            extras = _extras;
            partition_number = _partition_number;
        }
        void reverse()
        {
            psqlQuery->fetchPrevRow();
        }

        string exploreNextAggregate ()
        {
            return psqlQuery->getNextValue("aggregate");
        }
        map <string,PSQLAbstractORM *> * next ()
        {
            int counter = 0 ;
            if (psqlQuery->fetchNextRow())
            {
                map <string,PSQLAbstractORM *> * results  = new map <string,PSQLAbstractORM *>();
                for (auto orm_object: *orm_objects) 
                {
                    PSQLAbstractORM * orm= psqlController.addToORMCache(orm_object,psqlQuery,partition_number,orm_object->get_data_source_name());
                    (*results)[orm->getTableName()] = orm;

/*                    PSQLAbstractORM * orm = orm_object->clone();
                    orm->set_enforced_partition_number(partition_number);
                    // printf ("cloning ORM %p from %p\n",orm,orm_object);
                    // cout << "before assignresults" << endl;
                    orm->assignResults(psqlQuery);
                    // cout << "after assignresults" << endl;
                    (*results)[orm->getTableName()] = orm;*/
                }
                if (extras.size() > 0)
                {
                    PSQLGeneric_primitive_orm * orm = new PSQLGeneric_primitive_orm("");
                    for (auto e : extras)
                        orm->add(e.first,psqlQuery->getValue(e.first));
                    (*results)["PSQLGeneric"] = orm;            
                }
                else (*results)["PSQLGeneric"] = NULL;
                return results;
            }
            else return NULL;
        }
        ~PSQLJoinQueryPartitionIterator (){}
};


#endif