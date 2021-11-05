#include "h264or5FramedSource.h"
#include "GroupsockHelper.hh"

namespace vsnc
{
	namespace live
	{
		static void __remove_startcode(vsnc::vnal::Nalu& nalu) noexcept;
	}
}

void vsnc::live::__remove_startcode(vsnc::vnal::Nalu& nalu) noexcept
{
	if (*nalu.Head == 0x00 && *(nalu.Head + 1) == 0x00 && *(nalu.Head + 2) == 0x01)
	{
		nalu.Head = nalu.Head + 3;
		nalu.Length = nalu.Length - 3;
	}
	if (*nalu.Head == 0x00 && *(nalu.Head + 1) == 0x00 && *(nalu.Head + 2) == 0x00 && *(nalu.Head + 3) == 0x01)
	{
		nalu.Head = nalu.Head + 4;
		nalu.Length = nalu.Length - 4;
	}
}

vsnc::live::H264or5FramedSource::H264or5FramedSource(UsageEnvironment& env,const vsnc::vnal::Parser& parser)
	:FramedSource(env)
{	
	m_pParser = parser;
	m_sNalu = m_pParser.GetNextNalu();
	//去掉startcode
	__remove_startcode(m_sNalu);
}

vsnc::live::H264or5FramedSource* vsnc::live::H264or5FramedSource::createNew(UsageEnvironment& env,const vsnc::vnal::Parser& parser)
{
	H264or5FramedSource* newSource = new H264or5FramedSource(env, parser);
	return newSource;
}



vsnc::live::H264or5FramedSource::~H264or5FramedSource()
{
   
}

void vsnc::live::H264or5FramedSource::doGetNextFrame()
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
		m_sNalu = m_pParser.GetNextNalu();
		// 去掉startcode
		if(m_sNalu.Length>0)
		{
			__remove_startcode(m_sNalu);
		}
		gettimeofday(&fPresentationTime, NULL);//时间戳 
		fDurationInMicroseconds = 1000000 / 24;//控制播放速度
	}
	
	if (m_sNalu.Length > 0)
	{
		FramedSource::afterGetting(this);
	}
	
}

