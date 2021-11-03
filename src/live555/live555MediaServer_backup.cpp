#include <BasicUsageEnvironment.hh>  
#include "liveMedia.hh"  


static ServerMediaSession* createNewSMS(UsageEnvironment& env, char const* fileName);


int main()
{
    TaskScheduler* scheduler;
    UsageEnvironment* env;
    RTSPServer* rtspServer;
    ServerMediaSession* sms;
    //�������������  
    scheduler = BasicTaskScheduler::createNew();


    //������������  
    env = BasicUsageEnvironment::createNew(*scheduler);


    //����RTSP������  
    rtspServer = RTSPServer::createNew(*env, 554);    //ʹ��554�˿�  
    if (rtspServer == NULL)
    {
        rtspServer = RTSPServer::createNew(*env, 8554);   //554�˿ڱ�ռ�ã���ʹ��8554�˿�  
    }


    //��ӡ��������ַ  
    *env << "Play streams from this server using the URL\n\t"
        << rtspServer->rtspURLPrefix() << "<filename>.\n";


    //����session  
    sms = createNewSMS(*env, "test.264");
    rtspServer->addServerMediaSession(sms);


    //��������ļ���Ӧ��session...  


    //�����¼�ѭ��  
    env->taskScheduler().doEventLoop(); // does not return  


    return 0;
}


static ServerMediaSession* createNewSMS(UsageEnvironment& env, char const* fileName)
{
    char const* extension = strrchr(fileName, '.');
    ServerMediaSession* sms = NULL;
    Boolean reuseSource = False;    //������source  


    sms = ServerMediaSession::createNew(env, fileName);  //����һ��session  


    //  
    //��session�����subssesion  
    //  

    if (strcmp(extension, ".264") == 0)
    {
        sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    }


    //�������ý���ʽ֧��...  


    return sms;
}