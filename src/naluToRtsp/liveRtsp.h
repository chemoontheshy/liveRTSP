#ifndef __LIVE_RTSP_H_
#define __LIVE_RTSP_H_
#include <iostream>
#include "BasicUsageEnvironment.hh"
#include "splitnalu.h"

namespace vsnc
{
	namespace live
	{
		/// <summary>
		/// ָ��RTPSink����
		/// </summary>
		enum class CODEC
		{
			/// <summary>h264</summary>
			H264,
			/// <summary>h264</summary>
			HEVC
		};

		/// <summary>
		/// �������
		/// </summary>
		typedef struct Parameters
		{
			/// <summary>RTSP���ʶ˿�</summary>
			uint16_t    Port;
			/// <summary>�û���</summary>
			std::string Username;
			/// <summary>�û�����</summary>
			std::string Password;
			/// <summary>RTSP���ʺ�׺</summary>
			std::string StreamName;
			/// <summary>��ý������</summary>
			std::string Description;
		}Parameters;

		/// <summary>
		/// RTSP������
		/// </summary>
		class LiveServer
		{
		public:
			~LiveServer();
			/// <summary>
			/// ����RTSP������
			/// </summary>
			/// <param name="codec">������Դ��H264,����HEVC</param>
			/// <param name="param">�������</param>
			void start(const CODEC& codec, const Parameters& param,const vsnc::vnal::Parser& parser);

			void Send(const vsnc::vnal::Nalu& nalu);

		};



	}
}


#endif // !__LIVE_RTSP_H_

