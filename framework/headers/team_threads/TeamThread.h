#ifndef TEAMTHREAD_H
#define TEAMTHREAD_H


#include <common.h>

enum THREADTYPE {TEAMTHREAD,TEAMLEAD,TEAMMEMBER};


class TeamThread: public std::thread
{
    inline static map <std::thread::id,TeamThread *> thread_mapping;
    inline static std::mutex thread_mapping_lock;
    protected:
        std::thread::id team_id;
        std::thread::id my_id;
        THREADTYPE type;
    public:
        TeamThread():std::thread(){
            team_id = this->get_id();
            my_id = this->get_id();
            type = TEAMTHREAD;
        }
        inline static TeamThread * getMapping()
        {
            if (TeamThread::thread_mapping.find(this_thread::get_id()) != TeamThread::thread_mapping.end()) 
                return TeamThread::thread_mapping[this_thread::get_id()];
            else return NULL;
        }
        inline static std::thread::id getTeamId()
        {
            if (TeamThread::thread_mapping.find(this_thread::get_id()) != TeamThread::thread_mapping.end()) 
                return TeamThread::thread_mapping[this_thread::get_id()]->getTeamId();
            else return 0;

        }
        template< class F, class... Args >
        virtual TeamThread * createTeamMemberThread ( F&& f, Args&&... args )=0;

        virtual THREADTYPE get_type()
        {
            return type;
        }
        template< class F, class... Args >
        explicit TeamThread( F&& f, Args&&... args ) : thread(std::forward<F>(f),std::forward<Args>(args) ...)
        {
            type = TEAMTHREAD;
            TeamThread::thread_mapping_lock.lock();
            TeamThread::thread_mapping[this->get_id()]=this;
            TeamThread::thread_mapping_lock.unlock();
        }

        std::thread::id getTeamId()
        {
            return team_id;
        }
        virtual ~TeamThread(){
            TeamThread::thread_mapping_lock.lock();
            TeamThread::thread_mapping.erase(my_id);
            TeamThread::thread_mapping_lock.unlock();
        }
};

class TeamMemberThread:public TeamThread
{

    protected:
    public:
        TeamMemberThread():TeamThread(){
            type = TEAMLEAD;
        }

        template< class F, class... Args >
        explicit TeamMemberThread( thread::id _team_id, F&& f, Args&&... args ) : TeamThread(std::forward<F>(f),std::forward<Args>(args) ...)
        {
            team_id = _team_id;
            type = TEAMMEMBER;
            // cout << "team_id: " << team_id << endl;
        }

        template< class F, class... Args >
        TeamThread * createTeamMemberThread ( F&& f, Args&&... args ){
            // cout << "TeamMemberThread::createTeamMemberThread" << endl;
            return new TeamMemberThread(team_id,std::forward<F>(f),std::forward<Args>(args) ...);
        }

        virtual ~TeamMemberThread(){

        }
};


class TeamLeadThread:public TeamThread
{

    protected:
    public:
        TeamLeadThread():TeamThread(){
                type = TEAMLEAD;
        }

        template< class F, class... Args >
        explicit TeamLeadThread( F&& f, Args&&... args ) : TeamThread(std::forward<F>(f),std::forward<Args>(args) ...)
        {
                type = TEAMLEAD;
                team_id = this->get_id();
        }

        template< class F, class... Args >
        TeamThread * createTeamMemberThread ( F&& f, Args&&... args ){
            // cout << "TeamLeadThread::createTeamMemberThread" << endl;
            return new TeamMemberThread(team_id,std::forward<F>(f),std::forward<Args>(args) ...);
        }
        virtual ~TeamLeadThread(){

        }
};

template< class F, class... Args >
TeamThread * createTeamLeadIfNot (F&& f, Args&&... args)
{
        TeamThread * current_thread = TeamThread::getMapping();
        if (current_thread == NULL) return new TeamLead(std::forward<F>(f),std::forward<Args>(args) ...);
        else return current_thread->createTeamMemberThread(std::forward<F>(f),std::forward<Args>(args) ...);
}


#endif