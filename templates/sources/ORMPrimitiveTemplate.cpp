#include <%s.h>

%s


%s::%s(string _data_source_name):PSQLAbstractQueryIterator(_data_source_name,"%s"){

}
%s * %s::operator [] (long index)
{
    return NULL;
}
%s * %s::next ()
{
    if (psqlQuery->fetchNextRow())
    {
        %s * obj = new %s();
        obj->assignResults(psqlQuery);
        return obj;
    }
    else return NULL;
}
%s * %s::back ()
{
    return NULL;
}

void %s::process(std::function<void(%s * orm)> f)
{
    if (this->execute())
    {
        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(10);
        AbstractDBQuery * temp = this->psqlQuery;
        this->psqlQuery = (*p)[5];
        %s * orm = NULL;
        do {
            orm =this->next();
            if (orm != NULL) 
            {
                f(orm);
            }
        } while (orm != NULL);
        this->psqlQuery = temp;
    }
}


%s::~%s ()
{
    
}