#ifndef HTTPRESPONSEHEADER_H
#define HTTPRESPONSEHEADER_H
#include "common_ws.h"
#include "TCPSocket.h"


class HTTPResponseHeader // A class represening an HTTP response header
{
    private:
        std::map <string,string,ignorecase> header; //A map to store header tuples 
        string status; // Respose header status string
        int status_code; // Response header status code
        string protocol; // HTTP protocol user
    public:
        // Constructor seeting up the differen data members 
        HTTPResponseHeader(string p_status,int p_status_code,string p_protocol);
        void setHeader(string p_key,string p_value); // Setting a header tuple
        void respond (TCPSocket * p_tcpSocket); // responding to the client with the header over the tcpSocket
        void setStatus(string p_status);
        void setStatusCode(int p_status_code);
        void setProtocol(string p_protocol);
        ~HTTPResponseHeader(); // Destructor
};

#endif // HTTPRESPONSEHEADER_H
