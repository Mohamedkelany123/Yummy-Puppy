#include <AbstractMultipartSerializer.h>


AbstractMultipartSerializer::AbstractMultipartSerializer():AbstractSerializer()
{
    

}
void AbstractMultipartSerializer::serialize_binary (char * binary_buffer,long binary_size)
{
    // printf ("%lu\n%s\n",binary_size,binary_buffer);
    printf ("%.400s\n", binary_buffer);

    vector <char *> tokens;
    vector <long> tokens_size;
    char * ptr = binary_buffer;
    for ( int  i = 0 ; ptr != NULL && binary_buffer+binary_size-ptr !=0; i++)
    {
        if ( strncmp (ptr,"\r\n",2) == 0) 
        {
            ptr[0] = 0;
            ptr[1] = 0;
            ptr += 2;
            // printf (">>>>>> %.80s\n", ptr);
            tokens.push_back(ptr);
            for ( ;strncmp (ptr,tokens[0],strlen(tokens[0])) != 0 ; )
                ptr++;
            if ( ptr[-1] == '\n' )  ptr[-1]=0;
            if ( ptr[-2] == '\r' )  ptr[-2]=0;
            long token_sz = ptr - tokens[tokens.size()-1]-2;
            tokens_size.push_back(token_sz);

        }
        else {
            tokens.push_back(ptr);
            char * ptr1 = (char*) memchr (ptr,'\r',binary_buffer+binary_size-ptr);
            char * ptr2 = (char*) memchr (ptr,';',binary_buffer+binary_size-ptr);
            if ( ptr1 == NULL && ptr2 == NULL) break;

            if ( ptr1 < ptr2 || ptr2 == NULL )
            {
                ptr = ptr1;
            }
            else ptr = ptr2;

            long token_sz = ptr - tokens[tokens.size()-1];
            tokens_size.push_back(token_sz);

            ptr[0] = 0;
            ptr ++;
            if ( ptr[0] == '\n' || ptr[0] == ' ')
            {
                ptr[0] = 0;
                ptr ++;
            }
        }
    }

    cout << "# of tokens: " << tokens.size() << endl;
    for ( int  i = 0 ; i < tokens.size()-1 ; i ++) {
        printf ("[%d] %.80s [%lu]\n", i,tokens[i],tokens_size[i]);
    }

    FILE* ff = fopen ("/Users/kmsobh/test.png","wb");
    fwrite(tokens[9],tokens_size[9],1,ff);
    fclose (ff);

}
void AbstractMultipartSerializer::serialize (string ss)
{

}
string AbstractMultipartSerializer::deserialize ()
{
    return "";
}
AbstractMultipartSerializer::~AbstractMultipartSerializer()
{

}
