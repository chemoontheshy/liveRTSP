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
		/// 指定RTPSink的流
		/// </summary>
		enum class CODEC
		{
			/// <summary>h264</summary>
			H264,
			/// <summary>h264</summary>
			HEVC
		};

		/// <summary>
		/// 输入参数
		/// </summary>
		typedef struct Parameters
		{
			/// <summary>RTSP访问端口</summary>
			uint16_t    Port;
			/// <summary>用户名</summary>
			std::string Username;
			/// <summary>用户密码</summary>
			std::string Password;
			/// <summary>RTSP访问后缀</summary>
			std::string StreamName;
			/// <summary>流媒体描述</summary>
			std::string Description;
		}Parameters;

		/// <summary>
		/// RTSP服务器
		/// </summary>
		class LiveServer
		{
		public:
			~LiveServer();
			/// <summary>
			/// 开启RTSP服务器
			/// </summary>
			/// <param name="codec">输入是源是H264,还是HEVC</param>
			/// <param name="param">输入参数</param>
			void start(const CODEC& codec, const Parameters& param,const vsnc::vnal::Parser& parser);

			void Send(const vsnc::vnal::Nalu& nalu);

		};



	}
}


#endif // !__LIVE_RTSP_H_

