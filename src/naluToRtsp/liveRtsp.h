#ifndef __LIVE_RTSP_H_
#define __LIVE_RTSP_H_
#include <iostream>
#include "BasicUsageEnvironment.hh"

namespace vsnc
{
	namespace live
	{
		enum class CODEC
		{
			H264,
			HEVC
		};

		typedef struct Parameters
		{
			uint16_t    Port;
			std::string Username;
			std::string Password;
			std::string StreamName;
			std::string Description;
		};

		class LiveServer
		{
		public:
			void start(const CODEC& codec, const Parameters& param);
		private:
			UsageEnvironment* env;
		};



	}
}


#endif // !__LIVE_RTSP_H_

