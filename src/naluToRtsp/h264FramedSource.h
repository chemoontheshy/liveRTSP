#ifndef __H264FRAMEDSOURCE_H_
#define __H264FRAMEDSOURCE_H_


#include <FramedSource.hh>
#include "splitnalu.h"
#include <list>

class H264FramedSource : public FramedSource
{
public:
	static H264FramedSource* createNew(UsageEnvironment& env, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
	void dump(const vsnc::vnal::Nalu& nalu);

protected:
	H264FramedSource(UsageEnvironment& env, unsigned preferredFrameSize, unsigned playTimePerFrame);
	~H264FramedSource();


private:
	//重定义虚函数
	virtual void doGetNextFrame();
	void removeNaluHeader(vsnc::vnal::Nalu& nalu);
	vsnc::vnal::Parser parser;
	vsnc::vnal::Nalu m_sNalu;
	std::list<vsnc::vnal::Nalu> m_lstNalu;
};

#endif