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
		/// <summary>RTP�˿�</summary>
		static size_t RTP_PORT = 18888;
		/// <summary>RTCP�˿�</summary>
		static size_t RTCP_PORT = RTP_PORT + 1;
		/// <summary>TTL</summary>
		static uint8_t TTL = 255;
		/// <summary>in kbps;for RTCP b/w share</summary>
		static size_t ESTIMATED_SESSION_BANDWIDTH = 500;
		/// <summary>
		/// ����rtsp������
		/// </summary>
		/// <typeparam name="tVideoSink">������H264,����HEVC</typeparam>
		/// <typeparam name="tVdieoSource">������H264,����HEVC</typeparam>
		/// <param name="videoSink">RTP���</param>
		/// <param name="videoSource">��Ƶ����Դ</param>
		/// <param name="env">���</param>
		/// <param name="param">�������</param>
		/// <param name="rtcpGroupsock">RTCP�Ĳ���</param>
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

	// ��ӡ���ʵ�ַ
	if (rtspServer == NULL || sms == NULL) return; // sanuty check
	char* url = rtspServer->rtspURL(sms);
	std::cout << "Play this stream using the URL:" << url << std::endl;
	delete[] url;
	// ��ʽ���ã�������Ͳ��ϻ�ȡ��������
	videoSink->startPlaying(*videoSource, nullptr, videoSink);
	// ��������ʼ��ѯ�������ͻ��˵Ľ���
	env->taskScheduler().doEventLoop();
	t1 = std::thread([*env]() {env->taskScheduler().doEventLoop(); });
	t1.detach();
}
vsnc::live::LiveServer::~LiveServer()
{
}
void vsnc::live::LiveServer::start(const CODEC& codec, const Parameters& param, const vsnc::vnal::Parser& parser)
{
	

	// ����������
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	auto env = BasicUsageEnvironment::createNew(*scheduler);

	// ����RTP��RTCP
	struct sockaddr_storage destinationAddress;
	destinationAddress.ss_family = AF_INET;
	((struct sockaddr_in&)destinationAddress).sin_addr.S_un.S_addr = chooseRandomIPv4SSMAddress(*env);
	
	// ָ��RTP��RTCP�˿ں�
	const Port rtpPort(static_cast<portNumBits>(RTP_PORT));
	const Port rtcpPort(static_cast<portNumBits>(RTCP_PORT));

	// ��������,�ಥ
	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, TTL);
	rtpGroupsock.multicastSendOnly();
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, TTL);
	rtcpGroupsock.multicastSendOnly();

	// ��������ķ�Χ
	OutPacketBuffer::maxSize = 100000;
	// �̳�FramedSource���޸�����Դ
	auto videoFrame = H264or5FramedSource::createNew(*env, parser);
	switch (codec)
	{
	case vsnc::live::CODEC::H264:
	{
		H264VideoRTPSink* videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
		// ��ɢ֡��������������ֽ�������Ϊ���룬�����������Ľ��������ݸ��ƿ�����
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
