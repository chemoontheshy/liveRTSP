#include "liveRtsp.h"
#include "liveMedia.hh"  


namespace vsnc
{
	namespace live
	{
		/// <summary>
		/// True：后启动的客户端总是从当前第一个客户端已经播放到的位置开始播放
		/// False：每个客户端都从头开始播放影视频文件
		/// </summary>
		static Boolean REUSEFRIRSTSOURCE = True;
		/// <summary>
		/// 标准输出:打印相关信息
		/// </summary>
		/// <param name="rtspServer">RTSPServer权柄</param>
		/// <param name="sms">ServerMediaSession权柄</param>
		/// <param name="streamName">流媒体名</param>
		static void __annouce_stream(RTSPServer* rtspServer, ServerMediaSession* sms, const std::string& streamName);
	}
}

void vsnc::live::__annouce_stream(RTSPServer* rtspServer, ServerMediaSession* sms, const std::string& streamName)
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& _env = rtspServer->envir();
	_env << "\n\"" << streamName.c_str() << "\" stream\"\n";
	_env << "Play this stream using the URL \"" << url;
	delete[] url;
}

void vsnc::live::LiveServer::start(const CODEC& codec, const Parameters& param)
{
	// 创建任务调度器并初始化环境
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	// 验证信息
	UserAuthenticationDatabase* authDB = nullptr;

	// 创建RTSP服务器，开始监听客户端的链接
	// 默认使用554
	RTSPServer* rtspServer = RTSPServer::createNew(*env, param.Port, authDB);
	if (rtspServer == nullptr)
	{
		std::cout << "Failed to create RTSP server: " << env->getResultMsg() << std::endl;
	}
	ServerMediaSession* sms = ServerMediaSession::createNew(*env, param.StreamName.c_str(), param.StreamName.c_str(), param.Description.c_str());

	// 用于测试
	sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env, "test.264", REUSEFRIRSTSOURCE));

	// 为rtspServer添加session
	rtspServer->addServerMediaSession(sms);

	//应答信息到标准输出
	__annouce_stream(rtspServer, sms, param.StreamName);
	//进入事件循环，对套接字的读取事件和媒体文件的延时发送操作都在这个循环中完成。
	env->taskScheduler().doEventLoop();
}
