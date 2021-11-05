#include "h264or5FramedSource.h"
#include "GroupsockHelper.hh"


H264or5FramedSource::H264or5FramedSource(UsageEnvironment& env)
	:FramedSource(env)
{	
	parser.Init("test.264");
	m_sNalu = parser.GetNextNalu();
	//去掉startcode
	removeNaluHeader(m_sNalu);
}

H264or5FramedSource* H264or5FramedSource::createNew(UsageEnvironment& env)
{
	H264or5FramedSource* newSource = new H264or5FramedSource(env);
	return newSource;
}



H264or5FramedSource::~H264or5FramedSource()
{
   
}

void H264or5FramedSource::doGetNextFrame()
{
	 fFrameSize = m_sNalu.Length;
	 //如果nalu包大小大于fMaxSize，分批发送
	if (fFrameSize > fMaxSize)
	{
		fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	}
	else 
	{
		fNumTruncatedBytes = 0;
	}
	memmove(fTo, m_sNalu.Head, fFrameSize);
	if (fFrameSize > fMaxSize)
	{
		m_sNalu.Head = m_sNalu.Head + fMaxSize;
		m_sNalu.Length = m_sNalu.Length - fMaxSize;
	}
	else
	{
		//待发完一个nalu包的全部数据才获取新的nalu包
		m_sNalu = parser.GetNextNalu();
		// 去掉startcode
		if(m_sNalu.Length>0)
		{
			removeNaluHeader(m_sNalu);
		}
		gettimeofday(&fPresentationTime, NULL);//时间戳 
		fDurationInMicroseconds = 1000000 / 24;//控制播放速度
	}
	
	if (m_sNalu.Length > 0)
	{
		FramedSource::afterGetting(this);
	}
	

	
}

void H264or5FramedSource::removeNaluHeader(vsnc::vnal::Nalu& nalu)
{
	if (parser.CheckNaluHead(nalu.Head) == 4)
	{
		nalu.Head = nalu.Head + 4;
		nalu.Length = nalu.Length - 4;
	}
	if (parser.CheckNaluHead(nalu.Head) == 3)
	{
		nalu.Head = nalu.Head + 3;
		nalu.Length = nalu.Length - 3;
	}
}
