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
%s::~%s ()
{
    
}