#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include <EndpointService.h>
#include <AbstractSerializer.h>
// #include <loan_app_loan_primitive_orm.h>


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



extern "C" HTTPService *create_object() // extern "c" not garbling function names
{       
   

    return new EndpointService <FawryInquiry,FawryInquiryResponse>(
            [] (string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer) {


            cout << inputSerializer->getPOSTFawryInquiryInput().getMSGCode() << "\n";
            cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getKey() << "\n";
            cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getValue() << "\n";
            inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].setValue("01001091779");
            // jsonData.transactions.transaction[1].payment_id = 100;
            outputSerializer->getPOSTFawryInquiryOutput().setMSGCode("Hello all from serializer"+inputSerializer->getPOSTFawryInquiryInput().getMSGCode());                
    });
}   
