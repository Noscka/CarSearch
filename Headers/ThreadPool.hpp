#pragma once

#include <thread>
#include <mutex>

#include <NosLib/DynamicArray.hpp>

enum class WorkStatus
{
	Unfinished, /* Free to be worked on */
	Started,	/* A worker has picked the work item up */
	Finished,	/* work item is finished */
};

template<class WorkType>
class WorkHolder
{
protected:
	WorkType WorkItem;
	WorkStatus WorkItemStatus;

	inline static std::mutex WorkItemMutex;
public:
	WorkHolder(){}

	WorkHolder(const WorkType& workItem)
	{
		WorkItem = workItem;
		WorkItemStatus = WorkStatus::Unfinished;
	}

	WorkType GetWorkItem()
	{
		return WorkItem;
	}

	WorkStatus GetWorkStatus()
	{
		std::lock_guard<std::mutex> lock(WorkItemMutex);
		return WorkItemStatus;
	}

	void SetWorkStatus(const WorkStatus& newStatus)
	{
		std::lock_guard<std::mutex> lock(WorkItemMutex);
		WorkItemStatus = newStatus;
	}
};

template<class WorkType>
class DeviceDependentThreadPool
{
protected:
	inline static NosLib::DynamicArray<WorkHolder<WorkType>> WorkItemArray;
	inline static NosLib::DynamicArray<std::thread*> ThreadPool;

	inline static int ThreadPoolCount = 20; /* How many threads to use, will be made dynamic */

	inline static void ManageThreads()
	{
		for (int i = 0; i <= ThreadPool.GetLastArrayIndex(); /* ArrayIndex will go down */)
		{
			ThreadPool[i]->join();
			ThreadPool.Remove(i);
		}

		/* in theory, thread deletes itself here */
	}

	inline static void ThreadPoolManagement(void(*workFunc)(NosLib::DynamicArray<WorkHolder<WorkType>>*, Ui::MainWindow*), const NosLib::DynamicArray<WorkType>& work, Ui::MainWindow* ui) /* Implement Stopping */
	{
		for (WorkType workItem : work)
		{
			WorkItemArray.Append(WorkHolder<WorkType>(workItem));
		}

		for (int i = 0; i < ThreadPoolCount; i++)
		{
			ThreadPool.Append(new std::thread(workFunc, &WorkItemArray, ui));
			Sleep(10); /* desync threads */
		}

		ManageThreads();
	}

public:
	inline static void StartThreadPool(void(*workFunc)(NosLib::DynamicArray<WorkHolder<WorkType>>*, Ui::MainWindow*), NosLib::DynamicArray<WorkType>& work, Ui::MainWindow* ui)
	{
		std::thread thisThread(&DeviceDependentThreadPool<WorkType>::ThreadPoolManagement, workFunc, work, ui);
		thisThread.detach();
	}
};