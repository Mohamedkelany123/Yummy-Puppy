/**
 * File: MiniService.cpp 
 * Date: Mar 2022
 * Company: Blnk Consumer Finance
 * Author: Ghada Gamal
 */

#include "Logger.h"
#include "ConfigFile.h"


using namespace std;

Logger* Logger::m_Instance = 0;

string logFileName ;

/**
 * Class: Logger
 * Method: Constructor
 * Parameters: 
 * Return Type: 
 * Description:  
*/

Logger::Logger(ConfigFile * p_conf)
{  
   conf = p_conf;
   logFileName = conf->logfile;

   m_File.open(logFileName.c_str(), ios::out|ios::app);
   
  determine_log_level(conf->loglevel);
  determine_log_type(conf->logtype);         

}

/**
 * Class: Logger
 * Method: Destructor
 * Parameters: 
 * Return Type: 
 * Description:  
*/

Logger::~Logger()
{
   m_File.close();
   pthread_mutexattr_destroy(&m_Attr);
   pthread_mutex_destroy(&m_Mutex);
}

/**
 * Class: Logger
 * Method: setInstance
 * Parameters: 
 * Return Type: void
 * Description: Set Instance of the logger object. 
*/

void Logger::setInstance(Logger * logger){
  m_Instance = logger; 
}

/**
 * Class: Logger
 * Method: getInstanceInitial
 * Parameters: 
 * Return Type: Logger*
 * Description: Create an initial instance of the logger object. 
*/

Logger* Logger::getInstanceInitial(ConfigFile * p_conf) throw ()
{
   if (m_Instance !=  NULL) delete(m_Instance); 
   m_Instance = new Logger(p_conf);
   return m_Instance;
}

/**
 * Class: Logger
 * Method: getInstance
 * Parameters: 
 * Return Type: Logger*
 * Description: get Instance of the logger object. 
*/

Logger* Logger::getInstance() throw ()
{

   return m_Instance;
}

/**
 * Class: Logger
 * Method: lock
 * Parameters: 
 * Return Type: void
 * Description: lock mutex. 
*/

void Logger::lock()
{

   pthread_mutex_lock(&m_Mutex);
}

/**
 * Class: Logger
 * Method: unlock
 * Parameters: 
 * Return Type: void
 * Description: unlock mutex. 
*/

void Logger::unlock()
{
   pthread_mutex_unlock(&m_Mutex);

}

/**
 * Class: Logger
 * Method: logIntoFile
 * Parameters: data(std::string&)
 * Return Type: void
 * Description: write logs on the file. 
*/

void Logger::logIntoFile(std::string& data)
{
   log_mutex.lock();
   m_File << getCurrentTime() << "  " << data << endl;
   log_mutex.unlock();
}

/**
 * Class: Logger
 * Method: logOnConsole
 * Parameters: data(std::string&)
 * Return Type: void
 * Description: write logs on the console. 
*/

void Logger::logOnConsole(std::string& data)
{
   cout << getCurrentTime() << "  " << data << endl;
}

/**
 * Class: Logger
 * Method: getCurrentTime
 * Parameters: 
 * Return Type: string
 * Description: Get the current type. 
*/

string Logger::getCurrentTime()
{
   string currTime;
   //Current date/time based on current time
   time_t now = time(0); 
   // Convert current time to string
   currTime.assign(ctime(&now));

   // Last charactor of currentTime is "\n", so remove it
   string currentTime = currTime.substr(0, currTime.size()-1);
   return currentTime;
}

/**
 * Class: Logger
 * Method: determine_log_level
 * Parameters:  loglevel(std::string&)
 * Return Type: void
 * Description: Determine the Log level. 
*/

   void Logger::determine_log_level(std::string& loglevel){
   if (loglevel == "DISABLE_LOG") Logger::updateLogLevel(DISABLE_LOG);
   else if (loglevel == "LOG_LEVEL_INFO") Logger::updateLogLevel(LOG_LEVEL_INFO);
   else if (loglevel == "LOG_LEVEL_DEBUG") Logger::updateLogLevel(LOG_LEVEL_DEBUG);
   else if (loglevel == "ENABLE_LOG") Logger::updateLogLevel(ENABLE_LOG);
}

/**
 * Class: Logger
 * Method: determine_log_type
 * Parameters:  logtype(std::string&)
 * Return Type: void
 * Description: Determine the Log type. 
*/

   void Logger::determine_log_type(std::string& logtype){
   if (logtype == "NO_LOG") Logger::updateLogType(NO_LOG);
   else if (logtype == "CONSOLE") Logger::updateLogType(CONSOLE);
   else if (logtype == "FILE_LOG") Logger::updateLogType(FILE_LOG);
   
}

/**
 * Class: Logger
 * Method: error
 * Parameters:  text(const char*)
 * Return Type: void
 * Description: Interface for Error Log. 
*/

void Logger::error(const char* text) throw()
{
   string data;
   data.append("[ERROR]: ");
   data.append(text);

   // ERROR must be capture
   if(m_LogType == FILE_LOG)
   {
      logIntoFile(data);
   }
   else if(m_LogType == CONSOLE)
   {
      logOnConsole(data);
   }
}

/**
 * Class: Logger
 * Method: error
 * Parameters:  text(std::string& text)
 * Return Type: void
 * Description: Interface for Error Log. 
*/

void Logger::error(std::string& text) throw()
{
   error(text.data());
}

/**
 * Class: Logger
 * Method: always
 * Parameters:  text(const char*)
 * Return Type: void
 * Description: Interface for Always Log. 
*/

void Logger::always(const char* text) throw()
{
   string data;
   data.append("[ALWAYS]: ");
   data.append(text);

   // No check for ALWAYS logs
   if(m_LogType == FILE_LOG)
   {
      logIntoFile(data);
   }
   else if(m_LogType == CONSOLE)
   {
      logOnConsole(data);
   }
}

/**
 * Class: Logger
 * Method: always
 * Parameters:  text(std::string&)
 * Return Type: void
 * Description: Interface for Always Log. 
*/

void Logger::always(std::string& text) throw()
{
   always(text.data());
}

/**
 * Class: Logger
 * Method: info
 * Parameters:  text(const char*)
 * Return Type: void
 * Description: Interface for Info Log. 
*/

void Logger::info(const char* text) throw()
{
   string data;
   data.append("[INFO]: ");
   data.append(text);

   if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_INFO))
   {
      logIntoFile(data);
   }
   else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_INFO))
   {
      logOnConsole(data);
   }
}

/**
 * Class: Logger
 * Method: info
 * Parameters:  text(std::string&)
 * Return Type: void
 * Description: Interface for Info Log. 
*/

void Logger::info(std::string& text) throw()
{
   info(text.data());
}

/**
 * Class: Logger
 * Method: debug
 * Parameters:  text(const char*)
 * Return Type: void
 * Description: Interface for Debug Log. 
*/ 

void Logger::debug(const char* text) throw()
{
   string data;
   data.append("[DEBUG]: ");
   data.append(text);

   if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_DEBUG))
   {
      logIntoFile(data);
   }
   else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_DEBUG))
   {
      logOnConsole(data);
   }
}

/**
 * Class: Logger
 * Method: debug
 * Parameters:  text(std::string&)
 * Return Type: void
 * Description: Interface for Debug Log. 
*/ 

void Logger::debug(std::string& text) throw()
{
   debug(text.data());
}

/**
 * Class: Logger
 * Method: updateLogLevel
 * Parameters:  
 * Return Type: void
 * Description: Interfaces to control log levels. 
*/

void Logger::updateLogLevel(LogLevel logLevel)
{
   m_LogLevel = logLevel;
}

/**
 * Class: Logger
 * Method: enaleLog
 * Parameters:  
 * Return Type: void
 * Description: Enable all log levels. 
*/

void Logger::enaleLog()
{
   m_LogLevel = ENABLE_LOG; 
}

/**
 * Class: Logger
 * Method: disableLog
 * Parameters:  
 * Return Type: void
 * Description: Disable all log levels, except error. 
*/

void Logger:: disableLog()
{
   m_LogLevel = DISABLE_LOG;
}

/**
 * Class: Logger
 * Method: updateLogType
 * Parameters:  logType(LogType)
 * Return Type: void
 * Description: Update the log type. 
*/

// Interfaces to control log Types
void Logger::updateLogType(LogType logType)
{
   m_LogType = logType;
}

/**
 * Class: Logger
 * Method: enableConsoleLogging
 * Parameters:  
 * Return Type: void
 * Description: Enable writing logs on the console. 
*/

void Logger::enableConsoleLogging()
{
   m_LogType = CONSOLE; 
}

/**
 * Class: Logger
 * Method: enableFileLogging
 * Parameters:  
 * Return Type: void
 * Description: Enable writing logs in the LogFile. 
*/

void Logger::enableFileLogging()
{
   m_LogType = FILE_LOG ;
}