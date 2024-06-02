#ifndef _LOGGER_H_
#define _LOGGER_H_


#include "includes.h"
#include <errno.h>
#include <pthread.h>

class ConfigFile;

   class Logger
   {
      public:
         // Direct Interface for logging into log file or console using MACRO(s)
         #define LOG_ERRORS(x)   (Logger::getInstance() != NULL )? Logger::getInstance()->error(x) :  (void(0));
         #define LOG_ALWAYS(x)	(Logger::getInstance() != NULL )? Logger::getInstance()->always(x) :  (void(0));
         #define LOG_INFO(x)     (Logger::getInstance() != NULL )? Logger::getInstance()->info(x) :  (void(0));
         #define LOG_DEBUG(x)    (Logger::getInstance() != NULL )? Logger::getInstance()->debug(x) :  (void(0));
         
         mutex log_mutex;
         
         // enum for LOG_LEVEL
         typedef enum LOG_LEVEL
         {
            DISABLE_LOG       = 1,
            LOG_LEVEL_INFO	   = 2,
            LOG_LEVEL_DEBUG   = 3,
            ENABLE_LOG        = 4,
         }LogLevel;

         // enum for LOG_TYPE
         typedef enum LOG_TYPE
         {
            NO_LOG            = 1,
            CONSOLE           = 2,
            FILE_LOG          = 3,
         }LogType; 

            static Logger* getInstance() throw ();
            static Logger* getInstanceInitial(ConfigFile * p_conf) throw ();
            static void setInstance(Logger * logger);
            void determine_log_level(std::string& loglevel);
            void determine_log_type(std::string& logtype);

            // Interface for Error Log 
            // Error log must be always enable
            // Hence, there is no interfce to control error logs
            void error(const char* text) throw();
            void error(std::string& text) throw();

            // Interface for Always Log 
            void always(const char* text) throw();
            void always(std::string& text) throw();

            // Interface for Info Log 
            void info(const char* text) throw();
            void info(std::string& text) throw();

            // Interface for Debug log 
            void debug(const char* text) throw();
            void debug(std::string& text) throw();

            // Interfaces to control log levels
            void updateLogLevel(LogLevel logLevel);
            void enaleLog();  // Enable all log levels
            void disableLog(); // Disable all log levels, except error and alarm

            // Interfaces to control log Types
            void updateLogType(LogType logType);
            void enableConsoleLogging();
            void enableFileLogging();

      protected:
            Logger(ConfigFile * p_conf);
            ~Logger();

            // Wrapper function for lock/unlock
            // For Extensible feature, lock and unlock should be in protected
            void lock();
            void unlock();

            std::string getCurrentTime();

      private:
            void logIntoFile(std::string& data);
            void logOnConsole(std::string& data);
            Logger(const Logger& obj) {}
            
            static Logger*          m_Instance;
            ConfigFile *            conf;
            std::ofstream           m_File;

            pthread_mutexattr_t     m_Attr; 
            pthread_mutex_t         m_Mutex;

            LogLevel                m_LogLevel;
            LogType                 m_LogType;
   };


#endif 

