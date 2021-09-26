#ifndef _MODULE_TASKMANAGER_H__
#define _MODULE_TASKMANAGER_H__

#include "Module.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class Task
{
public:
	virtual void Execute() = 0;
	Module* owner = nullptr;
};

class ModuleTaskManager : public Module
{
public:
	ModuleTaskManager(Application* app, bool start_enabled = true);
	~ModuleTaskManager() {}

	bool Init() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	// To schedule new tasks
	void ScheduleTask(Task* task, Module* owner);
	void ThreadMain();

private:

	static const int MAX_THREADS = 4;
	std::thread threads[MAX_THREADS];

	std::queue<Task*> scheduledTasks;
	std::queue<Task*> finishedTasks;
	std::condition_variable event;
	std::mutex mtx;

	bool exitFlag = false;
};

#endif