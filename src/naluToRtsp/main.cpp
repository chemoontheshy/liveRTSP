#include"liveRtsp.h"

int main()
{
	vsnc::live::Parameters param;
	param.Description = "test";
	param.Port = 554;
	param.StreamName = "testStream";
	vsnc::live::LiveServer liveSever;
	vsnc::vnal::Parser parser;
	parser.Init("test.264");
	liveSever.start(vsnc::live::CODEC::H264,param,parser);
}