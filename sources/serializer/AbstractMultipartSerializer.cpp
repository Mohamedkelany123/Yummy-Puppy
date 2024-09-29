#include <AbstractMultipartSerializer.h>


AbstractMultipartSerializer::AbstractMultipartSerializer():AbstractSerializer()
{

}
void AbstractMultipartSerializer::serialize_binary (char * binary_buffer,long binary_size)
{

    printf ("%lu\n%s\n",binary_size,binary_buffer);

}
void AbstractMultipartSerializer::serialize (string ss)
{

}
string AbstractMultipartSerializer::deserialize ()
{

}
AbstractMultipartSerializer::~AbstractMultipartSerializer()
{

}
