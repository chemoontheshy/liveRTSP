#ifndef __H264FRAMEDSOURCE_H_
#define __H264FRAMEDSOURCE_H_


#include <FramedSource.hh>
#include "splitnalu.h"

namespace vsnc
{
	namespace live
	{
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
			static H264or5FramedSource* createNew(UsageEnvironment& env, const vsnc::vnal::Parser& parser);

		protected:
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="env">Ȩ��</param>
			H264or5FramedSource(UsageEnvironment& env, const vsnc::vnal::Parser& parser);
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
			/// ��������
			/// </summary>
			vsnc::vnal::Parser m_pParser;
			/// <summary>
			/// ������
			/// </summary>
			vsnc::vnal::Nalu   m_sNalu;
		};
	}
}

#endif