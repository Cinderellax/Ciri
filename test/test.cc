#include "../ciri/Schedule.h"
#include "../ciri/Coroutine.h"
#include <iostream>
using namespace Ciri;
using namespace std;
struct args {
	char n;
};
Schedule::ptr S(new Schedule);
static void
foo(void *ud) {
	struct args * arg = (struct args*)ud;
	char start = arg->n;
	int i;
	for (i=0;i<5;i++) {
		cout<<"coroutine id: "<<S->getId()<<"\t echo: "<<(char)(start + i)<<endl;
		// 切出当前协程
		S->yield();
	}
    printf("success\n");
}

int main()
{
    struct args arg1 = { 'A' };
    struct args arg2 = { '0' };
    Coroutine::ptr co1(new Coroutine(std::bind(foo,&arg1),S));
    co1->addtosch();
    Coroutine::ptr co2(new Coroutine(std::bind(foo,&arg2),S));
    co2->addtosch();
    printf("main start\n");
	for(int i=0;i<6;++i){
	// 	// 使用协程co1
		co1->resume();
	// 	// 使用协程co2
		co2->resume();
	} 
	printf("main end\n");
    return 0;
}