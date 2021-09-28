#include "ModuleTaskManager.h"

ModuleTaskManager::ModuleTaskManager(Application* app, bool start_enabled) : 
	Module(app, start_enabled) {}

void ModuleTaskManager::ThreadMain()
{
	// Wait for new tasks to arrive
	while (true)
	{
		// Retrieve a task from scheduledTasks
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

		// Insert executed task into finishedTasks
		task->Execute();
		std::unique_lock<std::mutex> lock(mtx);
		finishedTasks.push(task);
	}
}

bool ModuleTaskManager::Init()
{
	// Create threads, that will execute ThreadMain()
	for (int i = 0; i < MAX_THREADS; ++i)
		threads[i] = std::thread(&ModuleTaskManager::ThreadMain, this);

	return true;
}

update_status ModuleTaskManager::Update(float dt)
{
	// Dispatch all finished tasks to their owner module
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
	// Notify all threads to finish and join them
	exitFlag = true;
	event.notify_all();

	for (int i = 0; i < MAX_THREADS; ++i)
		threads[i].join();

	return true;
}

void ModuleTaskManager::ScheduleTask(Task* task, Module* owner)
{
	// Insert the task into scheduledTasks so it is executed by some thread
	task->owner = owner;
	std::unique_lock<std::mutex> lock(mtx);
	scheduledTasks.push(task);
	event.notify_one();
}