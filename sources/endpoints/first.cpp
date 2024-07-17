#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include <EndpointService.h>
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

// template <class I,class O>
// string endpoint_entry(string http_body,std::function<void(string http_body,I * inputSerializer,O * outputSerializer)> f) {

//         I * inputSerializer  = new I(); 
//         O * outputSerializer  = new O(); 

//         inputSerializer->serialize(http_body);
//         f (http_body,inputSerializer,outputSerializer);
//         string str_return = outputSerializer->deserialize();
//         delete (inputSerializer);
//         delete (outputSerializer);
//         return str_return;
// }


// void serve_endpoint (char * http_request,char * http_reply)
// {
//     string str = endpoint_entry <FawryInquiry,FawryInquiryResponse> (http_request,[] (string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer) {

//         cout << inputSerializer->getPOSTFawryInquiryInput().getMSGCode() << "\n";
//         cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getKey() << "\n";
//         cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getValue() << "\n";
//         ((FawryInquiry *)inputSerializer)->getPOSTFawryInquiryInput().getCustomProperties()[0].setValue("01001091779");
//         // jsonData.transactions.transaction[1].payment_id = 100;
//         outputSerializer->getPOSTFawryInquiryOutput().setMSGCode("Hello all");
//     });
//     strcpy (http_reply,str.c_str());
// }


// std::function<void(string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer)> l = [] (string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer) {
//     cout << inputSerializer->getPOSTFawryInquiryInput().getMSGCode() << "\n";
//     cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getKey() << "\n";
//     cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getValue() << "\n";
//     inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].setValue("01001091779");
//     // jsonData.transactions.transaction[1].payment_id = 100;
//     outputSerializer->getPOSTFawryInquiryOutput().setMSGCode("Hello all from serializer"+inputSerializer->getPOSTFawryInquiryInput().getMSGCode());
// };

extern "C" HTTPService *create_object() // extern "c" not garbling function names
{       
    bool connect = psqlController.addDataSource("main","localhost",5432,"django_ostaz_08072024","postgres","postgres");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    int threadsCount = 1;
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);

    return new EndpointService <FawryInquiry,FawryInquiryResponse>(
            [] (string http_body,FawryInquiry * inputSerializer,FawryInquiryResponse * outputSerializer) {


        int threadsCount = 1;
        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
            {   
                new loan_app_loan_primitive_orm("main"),
                new tms_app_loaninstallmentfundingrequest_primitive_orm("main"),
                new loan_app_installment_primitive_orm("main"),
                new new_lms_installmentextension_primitive_orm("main")
            },
            {
                {{{"loan_app_loan","id"},{"tms_app_loaninstallmentfundingrequest","loan_id"}},JOIN_TYPE::left},
                {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}},JOIN_TYPE::right},
                {{{"loan_app_installment","id"},{"tms_app_loaninstallmentfundingrequest","installment_id"}},JOIN_TYPE::aux},
                {{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},JOIN_TYPE::inner},
            });

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("loan_app_installment.loan_id",lte,100)
            )
        );
        
        psqlQueryJoin->setOrderBy("loan_app_installment.id asc");
        psqlQueryJoin->process(threadsCount, [](map <string,PSQLAbstractORM*> * orms,int partition_number,mutex * shared_lock,void * extras) {

                loan_app_loan_primitive_orm * lal = ORM(loan_app_loan, orms);
                loan_app_installment_primitive_orm * lai = ORM(loan_app_installment, orms);
                tms_app_loaninstallmentfundingrequest_primitive_orm * tlai = ORM(tms_app_loaninstallmentfundingrequest, orms);

                cout << "BOND ID->" <<  tlai->get_funding_facility_id() << " LOAN ID->" <<  lai->get_loan_id() << " Installment ID->" <<  lai->get_id() << endl;
            });

            delete (psqlQueryJoin);
            cout << inputSerializer->getPOSTFawryInquiryInput().getMSGCode() << "\n";
            cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getKey() << "\n";
            cout << inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].getValue() << "\n";
            inputSerializer->getPOSTFawryInquiryInput().getCustomProperties()[0].setValue("01001091779");
            // jsonData.transactions.transaction[1].payment_id = 100;
            outputSerializer->getPOSTFawryInquiryOutput().setMSGCode("Hello all from serializer"+inputSerializer->getPOSTFawryInquiryInput().getMSGCode());                
    });
}   
