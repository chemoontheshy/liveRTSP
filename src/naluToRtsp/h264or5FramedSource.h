#ifndef __H264FRAMEDSOURCE_H_
#define __H264FRAMEDSOURCE_H_


#include <FramedSource.hh>
#include "splitnalu.h"

/// <summary>
/// 适合H264or5的Nalu包输入
/// 需要去掉StartCode,如果不去掉虽然能播放但是会报错，参考http://lists.live555.com/pipermail/live-devel/2012-July/015474.html
/// </summary>
class H264or5FramedSource : public FramedSource
{
public:
	/// <summary>
	/// 仿照live555其他的继承的创建方式
	/// </summary>
	/// <param name="env">权柄</param>
	/// <returns>新的H264FrameSource</returns>
	static H264or5FramedSource* createNew(UsageEnvironment& env);

protected:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="env">权柄</param>
	H264or5FramedSource(UsageEnvironment& env);
	/// <summary>
	/// 析构函数
	/// </summary>
	~H264or5FramedSource();


private:
	/// <summary>
	/// 重写doGetNextFrame
	/// </summary>
	virtual void       doGetNextFrame();

	/// <summary>
	/// 去掉StartCode，如果不去掉虽然能播放但是会报错，参考http://lists.live555.com/pipermail/live-devel/2012-July/015474.html
	/// </summary>
	/// <param name="nalu">带有NaluHeader</param>
	void               removeNaluHeader(vsnc::vnal::Nalu& nalu);
	/// <summary>
	/// 创建对象
	/// </summary>
	vsnc::vnal::Parser parser;
	/// <summary>
	/// 缓存区
	/// </summary>
	vsnc::vnal::Nalu   m_sNalu;
};

#endif