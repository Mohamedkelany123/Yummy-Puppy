#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>
#include <PSQLAbstractQueryIterator.h>
#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include <HTTPService.h>
#include <HTTPResponseHeader.h>

class AbstractSerializer
{
    private:
    public:
        AbstractSerializer(){}
        virtual void serialize (string ss) = 0;
        virtual string deserialize () = 0;
        virtual ~AbstractSerializer(){}
};

class KeyValue: public AbstractSerializer
{
    private:
        string key;
        string value;    
        friend struct ThorsAnvil::Serialize::Traits<KeyValue>;
    public:
        KeyValue(): AbstractSerializer(){}
        void setKey (string _key){key=_key;}
        string getKey () {return key;} 
        void setValue (string _value){value=_value;}
        string getValue () {return value;} 
        void serialize (string ss) {}
        string deserialize () {return "";}
        ~KeyValue(){}
};

ThorsAnvil_MakeTrait(KeyValue, key, value);

class POSTFawryInquiryInput: public AbstractSerializer
{
    private:
        string msgCode;
        string sender;
        string receiver;
        string custLang;
        // string ClientDt          types.BlnkTime `json:"clientDt,omitempty" valid:"Required"`
        string clientDt;
        string rqUID;
        string asyncRqUID;
        string terminalID;
        string billingAcct;
        string bankID;
        string billTypeCode;
        string deliveryMethod;
        string signature;
        vector<KeyValue>  extraBillingAccts;
        vector<KeyValue>  customProperties;
    public:
        POSTFawryInquiryInput() : AbstractSerializer(){}
        void serialize (string ss) {}
        string deserialize () {return "";}
        string getMSGCode () {return msgCode;}
        vector<KeyValue> & getCustomProperties() {return customProperties;}
        ~POSTFawryInquiryInput(){} 


        friend struct ThorsAnvil::Serialize::Traits<POSTFawryInquiryInput>;
};

ThorsAnvil_MakeTrait(POSTFawryInquiryInput, msgCode, sender, receiver, custLang, clientDt, rqUID, asyncRqUID, terminalID, billingAcct, bankID, billTypeCode, deliveryMethod, signature, extraBillingAccts,  customProperties);


#define   SERIALIZER_TEMPLATE     void serialize (string str) { \
            std::stringstream json_string (str); \
            json_string >> ThorsAnvil::Serialize::jsonImporter((*this)); \
        }

#define   DESERIALIZER_TEMPLATE     string deserialize () { \
            std::ostringstream ss; \
            ss << ThorsAnvil::Serialize::jsonExporter((*this)); \
            return ss.str(); \
        }


class FawryInquiry: public AbstractSerializer
{
    private:
        POSTFawryInquiryInput postFawryInquiryInput;
    public:
        FawryInquiry() : AbstractSerializer(){}
        SERIALIZER_TEMPLATE
        DESERIALIZER_TEMPLATE
        POSTFawryInquiryInput & getPOSTFawryInquiryInput() {return postFawryInquiryInput;} 
        ~FawryInquiry(){} 
    friend struct ThorsAnvil::Serialize::Traits<FawryInquiry>;


};

ThorsAnvil_MakeTrait(FawryInquiry,postFawryInquiryInput);



class POSTFawryInquiryOutput: public AbstractSerializer
{
    private:
        string msgCode;
    public:
        POSTFawryInquiryOutput() : AbstractSerializer(){}
        void serialize (string ss) {}
        string deserialize () {return "";}
        string getMSGCode () {return msgCode;}
        void setMSGCode (string _msgCode) {msgCode=_msgCode;}
        ~POSTFawryInquiryOutput(){} 


        friend struct ThorsAnvil::Serialize::Traits<POSTFawryInquiryOutput>;
};

ThorsAnvil_MakeTrait(POSTFawryInquiryOutput, msgCode);


class FawryInquiryResponse: public AbstractSerializer
{
    private:
        POSTFawryInquiryOutput postFawryInquiryOutput;
    public:
        FawryInquiryResponse() : AbstractSerializer(){}
        SERIALIZER_TEMPLATE
        DESERIALIZER_TEMPLATE
        POSTFawryInquiryOutput & getPOSTFawryInquiryOutput() {return postFawryInquiryOutput;} 
        ~FawryInquiryResponse(){} 
    friend struct ThorsAnvil::Serialize::Traits<FawryInquiryResponse>;

};

ThorsAnvil_MakeTrait(FawryInquiryResponse,postFawryInquiryOutput);

template <class I,class O>
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


void serve_endpoint (char * http_request,char * http_reply)
{
    string str = endpoint_entry <FawryInquiry,FawryInquiryResponse> (http_request,[] (string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer) {

        cout << inputSerializer->getPOSTFawryInquiryInput().getMSGCode() << "\n";
        cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getKey() << "\n";
        cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getValue() << "\n";
        ((FawryInquiry *)inputSerializer)->getPOSTFawryInquiryInput().getCustomProperties()[0].setValue("01001091779");
        // jsonData.transactions.transaction[1].payment_id = 100;
        outputSerializer->getPOSTFawryInquiryOutput().setMSGCode("Hello all");
    });
    strcpy (http_reply,str.c_str());
}

template <class I,class O>
class EndpointService : public HTTPService {

    private:
        std::function<void(string http_body,I * inputSerializer,O * outputSerializer)> lambda;
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

std::function<void(string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer)> l = [] (string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer) {

                cout << inputSerializer->getPOSTFawryInquiryInput().getMSGCode() << "\n";
                cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getKey() << "\n";
                cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getValue() << "\n";
                inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].setValue("01001091779");
                // jsonData.transactions.transaction[1].payment_id = 100;
                outputSerializer->getPOSTFawryInquiryOutput().setMSGCode("Hello all from serializer"+inputSerializer->getPOSTFawryInquiryInput().getMSGCode());
            };

extern "C" HTTPService *create_object() // extern "c" not garbling function names
{       
    return new EndpointService <FawryInquiry,FawryInquiryResponse>(l);
}   
