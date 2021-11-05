#ifndef __H264FRAMEDSOURCE_H_
#define __H264FRAMEDSOURCE_H_


#include <FramedSource.hh>
#include "splitnalu.h"

/// <summary>
/// �ʺ�H264or5��Nalu������
/// ��Ҫȥ��StartCode,�����ȥ����Ȼ�ܲ��ŵ��ǻᱨ���ο�http://lists.live555.com/pipermail/live-devel/2012-July/015474.html
/// </summary>
class H264or5FramedSource : public FramedSource
{
public:
	/// <summary>
	/// ����live555�����ļ̳еĴ�����ʽ
	/// </summary>
	/// <param name="env">Ȩ��</param>
	/// <returns>�µ�H264FrameSource</returns>
	static H264or5FramedSource* createNew(UsageEnvironment& env);

protected:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="env">Ȩ��</param>
	H264or5FramedSource(UsageEnvironment& env);
	/// <summary>
	/// ��������
	/// </summary>
	~H264or5FramedSource();


private:
	/// <summary>
	/// ��дdoGetNextFrame
	/// </summary>
	virtual void       doGetNextFrame();

	/// <summary>
	/// ȥ��StartCode�������ȥ����Ȼ�ܲ��ŵ��ǻᱨ���ο�http://lists.live555.com/pipermail/live-devel/2012-July/015474.html
	/// </summary>
	/// <param name="nalu">����NaluHeader</param>
	void               removeNaluHeader(vsnc::vnal::Nalu& nalu);
	/// <summary>
	/// ��������
	/// </summary>
	vsnc::vnal::Parser parser;
	/// <summary>
	/// ������
	/// </summary>
	vsnc::vnal::Nalu   m_sNalu;
};

#endif