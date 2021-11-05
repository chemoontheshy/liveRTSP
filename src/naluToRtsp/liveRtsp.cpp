#include "liveRtsp.h"
#include <liveMedia.hh>
#include "h264or5FramedSource.h"
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <thread>
#include <functional>

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
		/// <summary>
		/// 建立rtsp服务器
		/// </summary>
		/// <typeparam name="tVideoSink">输入是H264,还是HEVC</typeparam>
		/// <typeparam name="tVdieoSource">输入是H264,还是HEVC</typeparam>
		/// <param name="videoSink">RTP句柄</param>
		/// <param name="videoSource">视频输入源</param>
		/// <param name="env">句柄</param>
		/// <param name="param">输入参数</param>
		/// <param name="rtcpGroupsock">RTCP的参数</param>
		template<typename tVideoSink,typename tVdieoSource>
		static void __build_server(BasicUsageEnvironment* env, tVideoSink* videoSink,tVdieoSource* videoSource,
			                       const Parameters& param, Groupsock& rtcpGroupsock);

		std::thread t1;
		
	}
}

template<typename tVideoSink, typename tVdieoSource>
void vsnc::live::__build_server(BasicUsageEnvironment* env, tVideoSink* videoSink, tVdieoSource* videoSource,
	                            const Parameters& param, Groupsock& rtcpGroupsock)

{

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
		std::cout << "Failed to create RTSP server: " << *env->getResultMsg() << std::endl;
	}

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, param.StreamName.c_str(), nullptr, param.Description.c_str(), True);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	rtspServer->addServerMediaSession(sms);

	// 打印访问地址
	if (rtspServer == NULL || sms == NULL) return; // sanuty check
	char* url = rtspServer->rtspURL(sms);
	std::cout << "Play this stream using the URL:" << url << std::endl;
	delete[] url;
	// 正式调用，开启后就不断获取数据流。
	videoSink->startPlaying(*videoSource, nullptr, videoSink);
	// 调用器开始轮询，监听客户端的接入
	env->taskScheduler().doEventLoop();
	t1 = std::thread([*env]() {env->taskScheduler().doEventLoop(); });
	t1.detach();
}
vsnc::live::LiveServer::~LiveServer()
{
}
void vsnc::live::LiveServer::start(const CODEC& codec, const Parameters& param, const vsnc::vnal::Parser& parser)
{
	

	// 创建调度器
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	auto env = BasicUsageEnvironment::createNew(*scheduler);

	// 创建RTP和RTCP
	struct sockaddr_storage destinationAddress;
	destinationAddress.ss_family = AF_INET;
	((struct sockaddr_in&)destinationAddress).sin_addr.S_un.S_addr = chooseRandomIPv4SSMAddress(*env);
	
	// 指定RTP和RTCP端口号
	const Port rtpPort(static_cast<portNumBits>(RTP_PORT));
	const Port rtcpPort(static_cast<portNumBits>(RTCP_PORT));

	// 网络设置,多播
	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, TTL);
	rtpGroupsock.multicastSendOnly();
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, TTL);
	rtcpGroupsock.multicastSendOnly();

	// 输出包最大的范围
	OutPacketBuffer::maxSize = 100000;
	// 继承FramedSource，修改输入源
	auto videoFrame = H264or5FramedSource::createNew(*env, parser);
	switch (codec)
	{
	case vsnc::live::CODEC::H264:
	{
		H264VideoRTPSink* videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
		// 离散帧（而不是任意的字节流）作为输入，避免了完整的解析和数据复制开销。
		H264VideoStreamDiscreteFramer* videoSource = H264VideoStreamDiscreteFramer::createNew(*env, videoFrame);
		__build_server<H264VideoRTPSink, H264VideoStreamDiscreteFramer>(env,videoSink, videoSource, param, rtcpGroupsock);
		break;
	}
	case vsnc::live::CODEC::HEVC:
	{
		H265VideoRTPSink* videoSink = H265VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
		H265VideoStreamDiscreteFramer* videoSource = H265VideoStreamDiscreteFramer::createNew(*env, videoFrame);
		__build_server<H265VideoRTPSink, H265VideoStreamDiscreteFramer>(env,videoSink, videoSource, param, rtcpGroupsock);
		break;
	}
	default:
		std::cout << "The CODEC format is errror.";
		exit(-1);
	}
}

void vsnc::live::LiveServer::Send(const vsnc::vnal::Nalu& nalu)
{
	std::cout << "test" << std::endl;
}
