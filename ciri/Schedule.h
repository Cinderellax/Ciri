#ifndef __CIRI_SCHEDULE_H__
#define __CIRI_SCHEDULE_H__

#include <memory>
#include <ucontext.h>
#include <unordered_map>

#define STACK_SIZE 1024*8

namespace Ciri{

class Coroutine;

class Schedule{
public:
    using ptr = std::shared_ptr<Schedule>;

public:
    Schedule(){};
    ~Schedule(){};

    void addCoroutine(std::shared_ptr<Coroutine> co);
    void removeCoroutine(int id){m_co.erase(id);}
    char* getStack() {return m_stack;}
    ucontext_t& getMain() {return m_main;}
    std::shared_ptr<Coroutine> getRunning() {return m_co[m_running];}
    int getId() const {return m_running;}
    void setRunning(int id){
        m_running=id;
    }
    void yield();
private:
    char m_stack[STACK_SIZE];
    ucontext_t m_main;
    int m_running = -1;
    std::unordered_map<int,std::shared_ptr<Coroutine>> m_co;
};
}

#endif
