#include "liveRtsp.h"
#include <liveMedia.hh>
#include "h264FramedSource.h"
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <thread>

namespace vsnc
{
	namespace live
	{
		/// <summary>RTP端口</summary>
		static size_t RTP_PORT = 18888;
		/// <summary>RTCP端口</summary>
		static size_t RTCP_PORT = RTP_PORT + 1;
		/// <summary>TTL</summary>
		static uint8_t TTL = 255;
		/// <summary>in kbps;for RTCP b/w share</summary>
		static size_t ESTIMATED_SESSION_BANDWIDTH = 500;
		H264FramedSource* videoFrame;
	}
}

void vsnc::live::LiveServer::start(const CODEC& codec, const Parameters& param)
{
	// 创建调度器
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	auto env = BasicUsageEnvironment::createNew(*scheduler);

	// 创建RTP和RTCP
	struct sockaddr_storage destinationAddress;
	destinationAddress.ss_family = AF_INET;
	((struct sockaddr_in&)destinationAddress).sin_addr.S_un.S_addr = chooseRandomIPv4SSMAddress(*env);
	
	// 指定RTP和RTCP端口号
	const Port rtpPort(RTP_PORT);
	const Port rtcpPort(RTCP_PORT);

	// 网络设置,指定多播？
	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, TTL);
	rtpGroupsock.multicastSendOnly();
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, TTL);
	rtcpGroupsock.multicastSendOnly();

	// 输出包最大的范围
	OutPacketBuffer::maxSize = 100000;

	// 初始化RTP打包
	auto videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';
	RTCPInstance* rtcp = RTCPInstance::createNew(*env, &rtcpGroupsock,
		ESTIMATED_SESSION_BANDWIDTH, CNAME,
		videoSink, nullptr, TRUE);

	RTSPServer* rtspServer = RTSPServer::createNew(*env, param.Port);
	if (rtspServer == nullptr)
	{
		std::cout << "Failed to create RTSP server: " << env->getResultMsg() << std::endl;
	}

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, param.StreamName.c_str(), nullptr, param.Description.c_str(), True);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	rtspServer->addServerMediaSession(sms);
	// 打印访问地址
	auto url = rtspServer->rtspURL(sms);
	std::cout << "Play this stream using the URL:" << url << std::endl;

	// 获取帧，继承与vFrameSource
	videoFrame = H264FramedSource::createNew(*env, 0, 0);

	// 离散帧（而不是任意的字节流）作为输入，避免了完整的解析和数据复制开销。
	H264VideoStreamDiscreteFramer* videoSource = H264VideoStreamDiscreteFramer::createNew(*env, videoFrame);
	
	videoSink->startPlaying(*videoSource, nullptr, videoSink);
	
	env->taskScheduler().doEventLoop();
}

void vsnc::live::LiveServer::dump(const vsnc::vnal::Nalu& nalu)
{
	videoFrame->dump(nalu);
}
