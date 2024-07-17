#include <curl/curl.h>
#include <common.h>



template <class I,class O>
class EndpointClient
{
    private:
        I * inputSerializer;
        O * outputSerializer;
        bool input_serializers_provided;
        bool output_serializers_provided;

            const char *readptr;
            size_t sizeleft;
            std::string readBuffer;

        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
        {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }

        static size_t read_callback(char *dest, size_t size, size_t nmemb, void *userp)
        {
            struct EndpointClient *wt = (struct EndpointClient *)userp;
            size_t buffer_size = size*nmemb;
            
            if(wt->sizeleft) {
                /* copy as much as possible from the source to the destination */
                size_t copy_this_much = wt->sizeleft;
                if(copy_this_much > buffer_size)
                copy_this_much = buffer_size;
                memcpy(dest, wt->readptr, copy_this_much);
            
                wt->readptr += copy_this_much;
                wt->sizeleft -= copy_this_much;
                return copy_this_much; /* we copied this many bytes */
            }            
            return 0; /* no more data left to deliver */
        }

        void curl_inference (string http_url,string data)
        {
            CURL *curl;
            CURLcode res;
            
            
            readptr = data.c_str();
            sizeleft = strlen(data.c_str());
            
            /* In windows, this inits the winsock stuff */
            res = curl_global_init(CURL_GLOBAL_DEFAULT);
            /* Check for errors */
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_global_init() failed: %s\n",
                        curl_easy_strerror(res));
                return;
            }
            
            /* get a curl handle */
            curl = curl_easy_init();
            if(curl) {
                /* First set the URL that is about to receive our POST. */
                curl_easy_setopt(curl, CURLOPT_URL, http_url.c_str());
            
                /* Now specify we want to POST data */
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
            
                /* we want to use our own read function */
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
            
                /* pointer to pass to our read function */
                curl_easy_setopt(curl, CURLOPT_READDATA, this);
            
                /* get verbose debug output please */
                // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            
                /*
                If you use POST to an HTTP 1.1 server, you can send data without knowing
                the size before starting the POST if you use chunked encoding. You
                enable this by adding a header like "Transfer-Encoding: chunked" with
                CURLOPT_HTTPHEADER. With HTTP 1.0 or without chunked transfer, you must
                specify the size in the request.
                */
            #ifdef USE_CHUNKED
                {
                struct curl_slist *chunk = NULL;
            
                chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
                res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
                /* use curl_slist_free_all() after the *perform() call to free this
                    list again */
                }
            #else
                /* Set the expected POST size. If you want to POST large amounts of data,
                consider CURLOPT_POSTFIELDSIZE_LARGE */
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)sizeleft);
            #endif
            
            #ifdef DISABLE_EXPECT
                /*
                Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue"
                header.  You can disable this header with CURLOPT_HTTPHEADER as usual.
                NOTE: if you want chunked transfer too, you need to combine these two
                since you can only set one list of headers with CURLOPT_HTTPHEADER. */
            
                /* A less good option would be to enforce HTTP 1.0, but that might also
                have other implications. */
                {
                struct curl_slist *chunk = NULL;
            
                chunk = curl_slist_append(chunk, "Expect:");
                res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
                /* use curl_slist_free_all() after the *perform() call to free this
                    list again */
                }
            #endif
            
                /* Perform the request, res gets the return code */
                res = curl_easy_perform(curl);
                /* Check for errors */
                if(res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
            
                /* always cleanup */
                curl_easy_cleanup(curl);
            }
            curl_global_cleanup();

        }
    public:
        EndpointClient(I * _inputSerializer= NULL,O * _outputSerializer=NULL)
        {
            if ( _inputSerializer== NULL)
            {
                inputSerializer  = new I(); 
                input_serializers_provided = false;
            }
            else
            {
                inputSerializer  = _inputSerializer; 
                input_serializers_provided = true;
            }

            if ( _outputSerializer== NULL)
            {
                outputSerializer  = new O(); 
                output_serializers_provided = false;
            }
            else
            {
                outputSerializer  = _outputSerializer; 
                output_serializers_provided = true;
            }
        }
        void setInputSerializer (string http_body)
        {
            inputSerializer->serialize(http_body);
        }
        O * fetch (string http_url)
        {
            curl_inference (http_url,inputSerializer->deserialize());
            outputSerializer->serialize (readBuffer);
            return outputSerializer;
        }
        
        ~EndpointClient()
        {
            if (!input_serializers_provided) delete (inputSerializer);
            if (!output_serializers_provided)  delete (outputSerializer);
        }

};