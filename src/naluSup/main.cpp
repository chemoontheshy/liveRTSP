#include "splitnalu.h"
#include <Windows.h>
#include <thread>

using namespace vsnc::vnal;

bool flag =false;

std::thread t1;
void death(const std::string& data)
{
	std::cout << data << std::endl;
	
}
class Test
{
public:
	void start();



};

void Test::start()
{
	std::string data = "death";
	while (!flag)
	{
		Sleep(1000);
		death(data);
	}
}

void send()
{
	std::cout << "send" << std::endl;
}
int main(void)
{
	Test *test;
	t1 = std::thread([&test]() {test->start(); });
	t1.detach();
	while (!flag)
	{
		Sleep(2000);
		send();
	}
	return 0;
}
