/**
 * File: ConfigFile.h
 * Date: 9 Sep 2022
 * Company: Blnk Consumer Finance
 * Description: This class reads the configuration file and sets the appropriate parameters inside the program. Only one global object is created from 
 *              this class. 
 * Author: Ghada Gamal
 */
#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H
#include "includes.h"


class ConfigFile{
    private:
       static ConfigFile* conf_Instance;
    public:
        static ConfigFile* getConfInstance() throw ();
        static void setConfInstance(ConfigFile * conf);
        bool daemon;                                //daemon is a flag that represents whether the server runs as a daemon or not
        string listen_address;                      
        int port_number;
        int backlog_queue;
        vector<string> core_numbers;
        string post_max_size;
        string logfile;
        string loglevel; 
        string logtype; 
        json data;
        string New_ml_ConfigFile;
        

        ConfigFile(int argc, char** argv);
        json ReadConfigFile(string configfilename);
        void SetupEnvironment( ConfigFile * conf );
        ~ConfigFile();

};

#endif