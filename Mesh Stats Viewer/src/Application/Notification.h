#pragma once

enum class NotificationType : uint8_t
{
	Info,
	Warning,
	Error
};

struct Notification
{
	const std::string Message;
	const NotificationType Type;

	static Notification Info(const std::string& message);
	static Notification Warning(const std::string& message);
	static Notification Error(const std::string& message);

private:
	Notification(const std::string& message, const NotificationType Type);
};