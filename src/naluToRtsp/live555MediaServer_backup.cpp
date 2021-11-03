#include"liveRtsp.h"


int main()
{
    vsnc::live::Parameters param;
    param.Description = "Session streamd by test";
    param.Port = 554;
    param.StreamName = "liveServerTest";
    param.Password = "";
    param.Username = "";
    vsnc::live::LiveServer liveServer;
    liveServer.start(vsnc::live::CODEC::H264, param);
    return 0;
}

