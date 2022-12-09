#include "Task1.h"

Task1::Task1()
{

}

Task1::~Task1()
{

}

void Task1::work(int t, int id)
{
	std::cout << id << " start \n" << std::endl;
	//printf("%d start \n", id);
	
	std::this_thread::sleep_for(std::chrono::seconds(t));

	std::cout << id << " end after "<< t << "\n" <<std::endl;
	//printf("%d end after %ds\n", id, t);
}
