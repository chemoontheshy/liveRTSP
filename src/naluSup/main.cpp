#include "splitnalu.h"
#include<Windows.h>
using namespace vsnc::vnal;

int main(void)
{
	Parser paerser("test.264");
	bool flag = true;
	while (flag)
	{
		auto nalu = paerser.GetNextNalu();
		if (nalu.Length < 0)
		{
			flag = !flag;
		}
		std::cout << nalu.Length << std::endl;
		Sleep(100);
	}

}
