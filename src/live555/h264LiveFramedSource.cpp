#include "h264LiveFramedSource.hh"
#include "GroupsockHelper.hh"
#include <iostream>


H264LiveFramedSource::H264LiveFramedSource(UsageEnvironment& env, unsigned preferredFrameSize, unsigned playTimePerFrame)
	: ByteStreamFileSource(env, 0, preferredFrameSize, playTimePerFrame)
{
	parser.Init("test.264");
	m_sNalu = parser.GetNextNalu();
}

H264LiveFramedSource* H264LiveFramedSource::createNew(UsageEnvironment& env, unsigned preferredFrameSize, unsigned playTimePerFrame)
{
	H264LiveFramedSource* newSource = new H264LiveFramedSource(env, preferredFrameSize, playTimePerFrame);
	return newSource;
}

H264LiveFramedSource::~H264LiveFramedSource()
{
	
}

void H264LiveFramedSource::doGetNextFrame()
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
	memmove(fTo, m_sNalu.Head , fFrameSize);
	if (fFrameSize > fMaxSize)
	{
		m_sNalu.Head = m_sNalu.Head + fMaxSize;
		m_sNalu.Length = m_sNalu.Length - fMaxSize;
	}
	else
	{
		m_sNalu = parser.GetNextNalu();
	}
	gettimeofday(&fPresentationTime, NULL);//时间戳 
	fDurationInMicroseconds = 1000000 / 24;//控制播放速度
	//表示延迟0秒后再执行afterGetting函数,也可以直接用afterGetting(this)
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
	return;
}