#include "pch.h"
#include "Application/Notification.h"

#include "Utils/TimeUtils.h"

/*static*/ Notification Notification::Info(const std::string& message)
{
	ASSERT(!message.empty());
	return { message, NotificationType::Info };
}

/*static*/ Notification Notification::Warning(const std::string& message)
{
	ASSERT(!message.empty());
	return { message, NotificationType::Warning };
}

/*static*/ Notification Notification::Error(const std::string& message)
{
	ASSERT(!message.empty());
	return { message, NotificationType::Error };
}

Notification::Notification(const std::string& message, const NotificationType type)
	: Message('[' + utils::GetTime() + "] " + message)
	, Type(type)
{
}