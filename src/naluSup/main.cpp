#include "splitnalu.h"
#include<Windows.h>
using namespace vsnc::vnal;

int main(void)
{
	int num = 0;
	Parser paerser("test.264");
	bool flag = true;
	while (flag)
	{
		auto nalu = paerser.GetNextNalu();
		if (nalu.Length <= 0)
		{
			flag = !flag;
		}
		num++;
		Sleep(10);
	}
	std::cout << num << std::endl;
}
