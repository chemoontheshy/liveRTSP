#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "h264LiveVideoServerMediaSubssion.hh"

UsageEnvironment* env;

// True：后启动的客户端总是从当前第一个客户端已经播放到的位置开始播放
// False：每个客户端都从头开始播放影视频文件
Boolean reuseFirstSource = True;

//该函数打印相关信息
static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName);

int main(int argc, char** argv)
{

	//创建任务调度器并初始化使用环境
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	UserAuthenticationDatabase* authDB = NULL;

	//创建RTSP服务器，开始监听模客户端的连接
	//注意这里的端口号不是默认的554端口，因此访问URL时，需指定该端口号
	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
	if (rtspServer == NULL)
	{
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	char const* descriptionString = "Session streamed by \"h264LiveMediaServer\"";

	//流名字，媒体名
	char const* streamName = "h264ESVideoTest";

	//当客户点播时，要输入流名字streamName，告诉RTSP服务器点播的是哪个流。  
	//创建媒体会话，流名字和文件名的对应关系是通过增加子会话建立起来的。媒体会话对会话描述、会话持续时间、流名字等与会话有关的信息进行管理。  
	//第2个参数:媒体名、3:媒体信息、4:媒体描述  
	ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);

	//修改为自己实现的H264LiveVideoServerMediaSubssion
	sms->addSubsession(H264LiveVideoServerMediaSubssion::createNew(*env, reuseFirstSource));

	//为rtspserver添加session 
	rtspServer->addServerMediaSession(sms);

	//答应信息到标准输出
	announceStream(rtspServer, sms, streamName);

	//进入事件循环，对套接字的读取事件和对媒体文件的延时发送操作都在这个循环中完成。 
	env->taskScheduler().doEventLoop();

	return 0;
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName)
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();
	env << "\n\"" << streamName << "\" stream\"\n";
	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}