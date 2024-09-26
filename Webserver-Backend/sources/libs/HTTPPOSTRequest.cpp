#include "HTTPPOSTRequest.h" 

// Constructor: handling the HTTP POST method
HTTPPOSTRequest::HTTPPOSTRequest(TCPSocket * p_tcpSocket): HTTPRequest(p_tcpSocket) {}

// Overriding the base class readAndParse method
void HTTPPOSTRequest::readAndParse(string initial_header)
{
    
    // invoke the base class readAndParse
    HTTPRequest::readAndParse(initial_header); 
    // Notice that while reading the header from the socket, more data that the header might be read from the socket; parts of the body.
    // Any extrea data read from the body after the end of header indicator "\r\n\r\n" will be saved in the body data member
    // Get the body size stored in the header field "content-length" and store it in stdin_size
    long stdin_size = atol(header["content-length"].c_str());
    // Keep on reading from the socket until the length of body is equal to stdin_size
    for ( ;body.length() < stdin_size; )
    {
        char buffer[1024]; // temporary buffer for reading
        memset ( buffer,0,1024); // Initialize buffer to zeros
        // read from socket and leave one character empty to identify the end of the string
        int just_read = tcpSocket->readFromSocket(buffer,1023); 
        if ( binary_body == NULL)  binary_body = (char *) calloc (binary_size+just_read+10,sizeof(char));
        else binary_body = (char *) realloc (binary_body,(binary_size+just_read+10*(sizeof(char))));
        memcpy(binary_body+binary_size,buffer,just_read);
        binary_size += just_read;

        // Append buffer to body
        body +=buffer;
    }
    //cout << body << endl;
}
// A cloner method
HTTPRequest * HTTPPOSTRequest::clone (TCPSocket * p_tcpSocket)
{
    // Creates a new object of the same type and return it to the caller
    return new HTTPPOSTRequest(p_tcpSocket);
}
// Destructor
HTTPPOSTRequest::~HTTPPOSTRequest()
{
}
