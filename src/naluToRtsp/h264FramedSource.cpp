#include "h264FramedSource.h"
#include "GroupsockHelper.hh"


H264FramedSource::H264FramedSource(UsageEnvironment& env, unsigned preferredFrameSize, unsigned playTimePerFrame)
	:FramedSource(env)
{	
	//parser.Init("test.264");
	//m_sNalu = parser.GetNextNalu();
	//removeNaluHeader(m_sNalu);
}

H264FramedSource* H264FramedSource::createNew(UsageEnvironment& env, unsigned preferredFrameSize, unsigned playTimePerFrame)
{
	H264FramedSource* newSource = new H264FramedSource(env, preferredFrameSize, playTimePerFrame);
	return newSource;
}

void H264FramedSource::dump(const vsnc::vnal::Nalu& nalu)
{
	if (m_lstNalu.size() > 10)
	{
		m_lstNalu.pop_front();
	}
	m_lstNalu.push_back(nalu);
}



H264FramedSource::~H264FramedSource()
{
   
}

void H264FramedSource::doGetNextFrame()
{
	// fFrameSize = m_sNalu.Length;
	//if (fFrameSize > fMaxSize)
	//{
	//	fNumTruncatedBytes = fFrameSize - fMaxSize;
	//	fFrameSize = fMaxSize;
	//}
	//else 
	//{
	//	fNumTruncatedBytes = 0;
	//}
	//memmove(fTo, m_sNalu.Head, fFrameSize);
	//if (fFrameSize > fMaxSize)
	//{
	//	m_sNalu.Head = m_sNalu.Head + fMaxSize;
	//	m_sNalu.Length = m_sNalu.Length - fMaxSize;
	//}
	//else
	//{
	//	m_sNalu = parser.GetNextNalu();
	//	if(m_sNalu.Length>0)
	//	{
	//		removeNaluHeader(m_sNalu);
	//	}
	//	gettimeofday(&fPresentationTime, NULL);//时间戳 
	//	fDurationInMicroseconds = 1000000 / 24;//控制播放速度
	//}
	if (m_sNalu.Head == nullptr)
	{
		if (m_lstNalu.size() > 0)
		{
			m_sNalu.Head = m_lstNalu.front().Head;
			m_sNalu.Length = m_lstNalu.front().Length;
			m_lstNalu.pop_front();
			removeNaluHeader(m_sNalu);
		}
	}
	if (m_sNalu.Length > 0)
	{
		fFrameSize = m_sNalu.Length;
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
			if (m_lstNalu.size() > 0)
			{
				m_sNalu.Head = m_lstNalu.front().Head;
				m_sNalu.Length = m_lstNalu.front().Length;
				m_lstNalu.pop_front();
				removeNaluHeader(m_sNalu);
			}
		}
		FramedSource::afterGetting(this);

	}
	
}

void H264FramedSource::removeNaluHeader(vsnc::vnal::Nalu& nalu)
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
