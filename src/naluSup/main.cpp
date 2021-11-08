#include "splitnalu.h"
#include <Windows.h>

int main()
{
	vsnc::vnal::Parser parser("test.hevc");
	bool flag = true;
	int a = 0;
	int frame = 0;
	while (flag)
	{
		auto nalu = parser.GetNextNalu();
		if (nalu.Length <= 0)
		{
			flag = !flag;
			break;
		}
		if (parser.CheckNaluHead(nalu.Head) == 3)
		{
			a++;
			std::cout << "a: " << a << std::endl;
			Sleep(200);
		}
		frame++;
	}
	std::cout << "frame: " << frame << std::endl;
}

