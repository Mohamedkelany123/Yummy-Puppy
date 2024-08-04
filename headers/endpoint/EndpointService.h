#include <PSQLController.h>
#include <common.h>
#include <PSQLAbstractQueryIterator.h>
#include <HTTPService.h>
#include <HTTPResponseHeader.h>


template <class I,class O>
class EndpointService : public HTTPService {

    private:
        std::function<void(string http_body,I * inputSerializer,O * outputSerializer)> lambda;
        string endpoint_entry(string http_body,std::function<void(string http_body,I * inputSerializer,O * outputSerializer)> f) {

                I * inputSerializer  = new I(); 
                O * outputSerializer  = new O(); 

                inputSerializer->serialize(http_body);
                f (http_body,inputSerializer,outputSerializer);
                string str_return = outputSerializer->deserialize();
                delete (inputSerializer);
                delete (outputSerializer);
                return str_return;
        }

    public:

        EndpointService(std::function<void(string http_body,I * inputSerializer,O * outputSerializer)> _lambda):HTTPService() 
        {
            lambda= _lambda;
        }
        bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket)
        {
            string data = p_httpRequest->getBody(); // get the HTTPRequest body data


            string reply = endpoint_entry(data,lambda);

            // string reply = "{\"msg\":\"Hello all\"}";
            HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"OK",200,"HTTP/1.1");
            httpResponseHeader->setHeader("Content-Type","application/json");//application/json is important to have here to be able to send arabic numerals/characters
            httpResponseHeader->setHeader("Connection","close"); 
            httpResponseHeader->setHeader("charset","utf-8");
            httpResponseHeader->setHeader("Content-Length",to_string(reply.length()));
            httpResponseHeader->respond(); // Write back the response to the client through the TCPSocket
            
            // Write back the file to the client through the TCPSocket
            p_tcpSocket->writeToSocket(reply.c_str(),reply.length());
            delete (httpResponseHeader); // Delete the HTTP Response
            return true; // return true
        }
        // A pure virtual method that should be implemented by all descendants to clone and create new object        
        HTTPService * clone ()
        {
                return new EndpointService<I,O>(lambda);
        }
         ~EndpointService()
        {

        }
};
