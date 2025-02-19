#include "pch.h"
#include "Utils/TimeUtils.h"

#ifdef _MSC_VER
#	define LOCALTIME(_Tm, _Time) localtime_s(_Tm, _Time)
#else
#	define LOCALTIME(_Tm, _Time) localtime_r(_Time, _Tm)
#endif

namespace
{
	tm GetTimeInfo()
	{
		time_t rawTime;
		time(&rawTime);

		tm timeInfo;
		LOCALTIME(&timeInfo, &rawTime);

		return timeInfo;
	}
}

namespace utils
{
	std::string GetDate()
	{
		const auto timeInfo = GetTimeInfo();

		std::ostringstream stringStream;
		stringStream << std::put_time(&timeInfo, "%d/%m/%Y");
		return stringStream.str();
	}

	std::string GetTime()
	{
		const auto timeInfo = GetTimeInfo();

		std::ostringstream stringStream;
		stringStream << std::put_time(&timeInfo, "%T");
		return stringStream.str();
	}

	std::string GetDateTime()
	{
		const auto timeInfo = GetTimeInfo();

		std::ostringstream stringStream;
		stringStream << std::put_time(&timeInfo, "%d/%m/%Y %T");
		return stringStream.str();
	}
}