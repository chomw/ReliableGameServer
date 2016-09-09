#pragma once
#include <memory>
#include <string>
#include <map>

#include "thread/Synchronous.h"

namespace rgs
{
	namespace thread
	{
		template<typename... TArgs>
		class Thread;
	}

	namespace io
	{
		class Connection;
	}

	class UpDown;
	enum class Core;

	class Application
	{
	public:
		virtual ~Application() = default;

	private:
		void mainLoop();

	public:
		virtual void create(rgs::Core core, const std::string& appName, const std::string& version, int loggingLevel, bool consoleLog, DWORD updateInterval);
		virtual void destroy();

		void start();
		void stop();

		virtual void update();

	private:
		void updateConnection();

	public:
		void addConnection(const std::string& name, std::shared_ptr<rgs::io::Connection> connection);

		void removeConnection(const std::string& name);

		std::shared_ptr<rgs::io::Connection> getConnection(const std::string& name);

	private:
		std::string appName_;
		std::shared_ptr<rgs::UpDown> upDown_;
		std::shared_ptr<rgs::thread::Thread<>> mainLoop_;
		std::map<std::string, std::shared_ptr<rgs::io::Connection>> connections_;

		mutable thread::sync::SimpleLock lock_;
	};
}