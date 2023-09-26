#ifndef PSQLABSTRACTQUERYITERATOR_H
#define PSQLABSTRACTQUERYITERATOR_H

#include <PSQLAbstractORM.h>
enum unary_operator { eq, gt, lt, gte,lte,ne,nand };

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
    public:
        UnaryOperator(string _name,unary_operator _op,string _value){
            name = _name;
            op = _op;
            value = _value;
        }
        UnaryOperator(string _name,unary_operator _op,int _value){
            name = _name;
            op = _op;
            value = to_string(_value);
        }
        UnaryOperator(string _name,unary_operator _op,long _value){
            name = _name;
            op = _op;
            value = _value;
        }
        string const generate() const
        {
            if ( op == eq ) return name+" = '"+value+"'";
            else if ( op == gt ) return name+" > '"+value+"'";
            else if ( op == gte ) return name+" >= '"+value+"'";
            else if ( op == lt ) return name+" < '"+value+"'";
            else if ( op == lte ) return name+" <= '"+value+"'";
            else if ( op == ne ) return name+" <> '"+value+"'";
            else return "";
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
    public:
        PSQLAbstractQueryIterator(string _data_source_name,string _table_name);
        void setNativeSQL(string _sql);
        void filter ( Expression const & e);
        bool execute();
        ~PSQLAbstractQueryIterator();
};

#endif