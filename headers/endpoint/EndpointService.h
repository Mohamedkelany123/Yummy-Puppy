#include <PSQLController.h>
#include <common.h>
#include <PSQLAbstractQueryIterator.h>
#include <HTTPService.h>
#include <HTTPResponseHeader.h>
#include <Poco/JWT/JWT.h>
#include <Poco/JWT/Token.h>
#include <Poco/JWT/Signer.h>

using namespace Poco::JWT;

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
        bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket,vector<Middleware *> * middlewares=NULL)
        {
            string data = p_httpRequest->getBody(); // get the HTTPRequest body data
            
            vector <Middleware *> local_middlewares;
            for (int i = 0 ; i < middlewares->size(); i ++ )
                local_middlewares.push_back((*middlewares)[i]->clone());

            for (int i = 0 ; i < local_middlewares.size(); i ++ )
                local_middlewares[i]->pre();

            // Poco::JWT::Token token;
            // token.setType("JWT");
            // token.setSubject("1234567890");
            // token.payload().set("name", std::string("John Doe"));
            // token.setIssuedAt(Poco::Timestamp());

            // Poco::JWT::Signer signer("0123456789ABCDEF0123456789ABCDEF");
            // std::string jwt = signer.sign(token, Poco::JWT::Signer::ALGO_HS256);
            // cout << "XXXXXxXXXXXXXX:    " << jwt << endl;
            string reply = endpoint_entry(data,lambda);



            // string reply = "{\"msg\":\"Hello all\"}";
            HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"OK",200,"HTTP/1.1");
            httpResponseHeader->setHeader("Content-Type","application/json");//application/json is important to have here to be able to send arabic numerals/characters
            httpResponseHeader->setHeader("Connection","close"); 
            httpResponseHeader->setHeader("charset","utf-8");
            httpResponseHeader->setHeader("Content-Length",to_string(reply.length()));
            httpResponseHeader->respond(); // Write back the response to the client through the TCPSocket

            for (int i = 0 ; i < local_middlewares.size(); i ++ )
                local_middlewares[i]->post();

            // Write back the file to the client through the TCPSocket
            p_tcpSocket->writeToSocket(reply.c_str(),reply.length());
            delete (httpResponseHeader); // Delete the HTTP Response
            for (int i = 0 ; i < local_middlewares.size(); i ++ )
                delete (local_middlewares[i]);
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
