#include "liveRtsp.h"
#include "liveMedia.hh"  


namespace vsnc
{
	namespace live
	{
		/// <summary>
		/// True���������Ŀͻ������Ǵӵ�ǰ��һ���ͻ����Ѿ����ŵ���λ�ÿ�ʼ����
		/// False��ÿ���ͻ��˶���ͷ��ʼ����Ӱ��Ƶ�ļ�
		/// </summary>
		static Boolean REUSEFRIRSTSOURCE = True;
		/// <summary>
		/// ��׼���:��ӡ�����Ϣ
		/// </summary>
		/// <param name="rtspServer">RTSPServerȨ��</param>
		/// <param name="sms">ServerMediaSessionȨ��</param>
		/// <param name="streamName">��ý����</param>
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
	// �����������������ʼ������
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	// ��֤��Ϣ
	UserAuthenticationDatabase* authDB = nullptr;

	// ����RTSP����������ʼ�����ͻ��˵�����
	// Ĭ��ʹ��554
	RTSPServer* rtspServer = RTSPServer::createNew(*env, param.Port, authDB);
	if (rtspServer == nullptr)
	{
		std::cout << "Failed to create RTSP server: " << env->getResultMsg() << std::endl;
	}
	ServerMediaSession* sms = ServerMediaSession::createNew(*env, param.StreamName.c_str(), param.StreamName.c_str(), param.Description.c_str());

	// ���ڲ���
	sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env, "test.264", REUSEFRIRSTSOURCE));

	// ΪrtspServer���session
	rtspServer->addServerMediaSession(sms);

	//Ӧ����Ϣ����׼���
	__annouce_stream(rtspServer, sms, param.StreamName);
	//�����¼�ѭ�������׽��ֵĶ�ȡ�¼���ý���ļ�����ʱ���Ͳ����������ѭ������ɡ�
	env->taskScheduler().doEventLoop();
}
