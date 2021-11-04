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
		/// <summary>RTP�˿�</summary>
		static size_t RTP_PORT = 18888;
		/// <summary>RTCP�˿�</summary>
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
	// ����������
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	auto env = BasicUsageEnvironment::createNew(*scheduler);

	// ����RTP��RTCP
	struct sockaddr_storage destinationAddress;
	destinationAddress.ss_family = AF_INET;
	((struct sockaddr_in&)destinationAddress).sin_addr.S_un.S_addr = chooseRandomIPv4SSMAddress(*env);
	
	// ָ��RTP��RTCP�˿ں�
	const Port rtpPort(RTP_PORT);
	const Port rtcpPort(RTCP_PORT);

	// ��������,ָ���ಥ��
	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, TTL);
	rtpGroupsock.multicastSendOnly();
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, TTL);
	rtcpGroupsock.multicastSendOnly();

	// ��������ķ�Χ
	OutPacketBuffer::maxSize = 100000;

	// ��ʼ��RTP���
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
	// ��ӡ���ʵ�ַ
	auto url = rtspServer->rtspURL(sms);
	std::cout << "Play this stream using the URL:" << url << std::endl;

	// ��ȡ֡���̳���vFrameSource
	videoFrame = H264FramedSource::createNew(*env, 0, 0);

	// ��ɢ֡��������������ֽ�������Ϊ���룬�����������Ľ��������ݸ��ƿ�����
	H264VideoStreamDiscreteFramer* videoSource = H264VideoStreamDiscreteFramer::createNew(*env, videoFrame);
	
	videoSink->startPlaying(*videoSource, nullptr, videoSink);
	
	env->taskScheduler().doEventLoop();
}

void vsnc::live::LiveServer::dump(const vsnc::vnal::Nalu& nalu)
{
	videoFrame->dump(nalu);
}
