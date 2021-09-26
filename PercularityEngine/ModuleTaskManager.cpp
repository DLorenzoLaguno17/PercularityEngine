#include "ModuleTaskManager.h"

ModuleTaskManager::ModuleTaskManager(Application* app, bool start_enabled) : 
	Module(app, start_enabled) {}

void ModuleTaskManager::ThreadMain()
{
	while (true)
	{
		// TODO 3:
		// - Wait for new tasks to arrive
		// - Retrieve a task from scheduledTasks
		// - Execute it
		// - Insert it into finishedTasks

		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> lock(mtx);
			while (scheduledTasks.empty() && !exitFlag)
				event.wait(lock);

			if (exitFlag) break;
			else 
			{
				task = scheduledTasks.front();
				scheduledTasks.pop();
			}
		}

		task->Execute();
		std::unique_lock<std::mutex> lock(mtx);
		finishedTasks.push(task);
	}
}

bool ModuleTaskManager::Init()
{
	// TODO 1: Create threads (they have to execute threadMain())
	for (int i = 0; i < MAX_THREADS; ++i)
		threads[i] = std::thread(&ModuleTaskManager::ThreadMain, this);

	return true;
}

update_status ModuleTaskManager::Update(float dt)
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	std::unique_lock<std::mutex> lock(mtx);
	while (!finishedTasks.empty())
	{
		finishedTasks.front()->owner->OnTaskFinished(finishedTasks.front());
		finishedTasks.pop();
	}

	return UPDATE_CONTINUE;
}

bool ModuleTaskManager::CleanUp()
{
	// TODO 5: Notify all threads to finish and join them
	exitFlag = true;
	event.notify_all();

	for (int i = 0; i < MAX_THREADS; ++i)
		threads[i].join();

	return true;
}

void ModuleTaskManager::ScheduleTask(Task* task, Module* owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some thread
	std::unique_lock<std::mutex> lock(mtx);
	scheduledTasks.push(task);
	event.notify_one();
}