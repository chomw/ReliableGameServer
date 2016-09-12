#include "Application.h"
#include <memory>

class App : public rgs::Application
{
public:
	unsigned long count = 0;
private:
	static std::unique_ptr<App> instance_;

public:
	static App& instance();

	virtual void create(rgs::Core core, const std::string& appName, const std::string& logPath, DWORD updateInterval);
	virtual void update();
};