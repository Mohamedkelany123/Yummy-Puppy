/**
 * File: ConfigFile.cpp 
 * Date: 9 Sep 2022
 * Company: Blnk Consumer Finance
 * Author: Ghada Gamal
 */
#include "ConfigFile.h"


/**
 * Class: ConfigFile
 * Method: Constructor
 * Parameters: argc(int), argv(char **)
 * Return Type: void
 * Description: The constructor reads the default conf_file which should be /etc/blnk-ml/blnk-ml.json  and sets the appropriate fields that were read from
 *              json configuration file. The configuration file can be entered as the second argument on the terminal
*/
ConfigFile* ConfigFile::conf_Instance = 0; 

ConfigFile::ConfigFile(int argc, char ** argv){
    bool config_file_exists;
    config_file_exists = true;
    string conf_file = "/etc/advanced-server/jsons/advanced_blnk_ml.json" ; // "/home/debian/BackGroundRemovalTensorRT/BackGroundRemovalTensorRT.json"  ; // ;           //This is the default location for the configuration file, it is put there using the make install
    if(argc==2){                                            //we read the default conf file if there are more than 2 arguments or only one argument(name of the executable on the terminal)
            conf_file = argv[1];                            //example for a conf file name: ./blnk-ml /home/name_of_conf_file.json
    }
    if(config_file_exists==true){
            data = ReadConfigFile(conf_file);
            listen_address = data["server_config"]["listen_address"];
            port_number = stoi((string)data["server_config"]["port_number"]);
            backlog_queue = stoi((string)data["server_config"]["listen queue"]);
            post_max_size = data["server_config"]["post_max_size"];
            logfile = data["server_config"]["logfile"];
            loglevel = data["server_config"]["loglevel"]; 
            logtype = data["server_config"]["logtype"];
            daemon = data["server_config"]["daemon"];
//            New_ml_ConfigFile = data["AdvancedBlnkML"]["New_BlnkML_ConfigFile"];
        }
}


/**
 * Class: ConfigFile
 * Method: setConfInstance
 * Parameters: 
 * Return Type: void
 * Description: This methods sets ConfigFile object
*/

void ConfigFile::setConfInstance(ConfigFile * conf){
  conf_Instance = conf; 
}
/**
 * Class: ConfigFile
 * Method: getConfInstance
 * Parameters: 
 * Return Type: void
 * Description: This methods gets ConfigFile object
*/

ConfigFile* ConfigFile::getConfInstance() throw ()
{
   return conf_Instance;
}
/**
 * Class: ConfigFile
 * Method: SetupEnvironment
 * Parameters: conf(ConfigFile *)
 * Return Type: void
 * Description: This methods sets any environment variables that were required
*/
void ConfigFile::SetupEnvironment( ConfigFile * conf ){
setenv("TESSDATA_PREFIX", ((string)conf->data["ML"]["tesseract models path"]).c_str(), 1); //This is needed to be set as an environment variable, because the Tesseract API uses this specific env variable).
setenv("ONNX_PREFIX_SERVER", ((string) conf->data["ML"]["onnx models path"]).c_str(), 1); //This env variable is no longer needed).
setenv("ONNX_DPLUR_SERVER", ((string) conf->data["ML"]["dblur onnx path"]).c_str(), 1);
setenv("ONNX_BGR_SERVER",((string) conf->data["ML"]["bgr onnx path"]).c_str(), 1);
setenv("CREDIT_SERVER",((string) conf->data["ML"]["credit path"]).c_str(), 1);
setenv("Engine_BGR_TensorRT" , ((string) conf->data["ML"]["engine bgr tensorrt"]).c_str() , 1 );
setenv("TF_PREFIX_MODEL" , ((string) conf->data["ML"]["tflite path"]).c_str() , 1 );
setenv("TENSORRT_ONNX" , ((string) conf->data["ML"]["onnx_for_tensorrt"]).c_str() , 1 );

}
/**
 * Class: ConfigFile
 * Method: ReadConfigFile
 * Parameters: ReadConfigFile(string configfilename)
 * Return Type: json
 * Description: This methods reads the file contents and returns them as a string and terminates the program if there is any error in reading the file
*/
json ConfigFile::ReadConfigFile(string configfilename){
    std::ifstream f(configfilename);
    json json_data = nlohmann::json::parse(f);
    return json_data;
}

/**
 * Class: ConfigFile
 * Method: destructor
 * Parameters: 
 * Return Type: void
 * Description:
*/
ConfigFile::~ConfigFile(){}