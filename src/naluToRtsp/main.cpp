#include"liveRtsp.h"
#include <thread>

std::thread runner;

int main()
{
	vsnc::live::Parameters param;
	param.Description = "test";
	param.Port = 554;
	param.StreamName = "testStream";
	vsnc::live::LiveServer liveSever;
	liveSever.start(vsnc::live::CODEC::H264, param);
	vsnc::vnal::Parser parser;
	parser.Init("test.264");
	bool flag = true;
	std::cout << "test" << std::endl;
	while (flag)
	{
		auto nalu = parser.GetNextNalu();
		if (nalu.Length <= 0)
		{
			flag = !flag;
		}
		if (parser.CheckNaluHead(nalu.Head) == 4)
		{
			Sleep(40);
		}
		liveSever.dump(nalu);
	}
}