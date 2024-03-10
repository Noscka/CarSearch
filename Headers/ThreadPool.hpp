#pragma once

#include <thread>
#include <mutex>

#include <NosLib/DynamicArray.hpp>

enum class WorkStatus
{
	Unfinished, /* Free to be worked on */
	Started,	/* A worker has picked the work item up */
	Finished,	/* work item is finished */
	Failed,		/* There was some error */
};

template<class WorkType>
class WorkHolder
{
protected:
	inline static std::mutex WorkItemMutex;

	WorkType WorkItem;
	WorkStatus WorkItemStatus;
	uint16_t ErrorCount;
	inline static const uint16_t MaxErrorCount = 3;

public:
	WorkHolder(){}

	WorkHolder(const WorkType& workItem)
	{
		WorkItem = workItem;
		WorkItemStatus = WorkStatus::Unfinished;
	}

	WorkType GetWorkItem() const
	{
		return WorkItem;
	}

	WorkStatus GetWorkStatus() const
	{
		std::lock_guard<std::mutex> lock(WorkItemMutex);
		return WorkItemStatus;
	}

	void SetWorkStatus(const WorkStatus& newStatus)
	{
		std::lock_guard<std::mutex> lock(WorkItemMutex);
		if (newStatus == WorkStatus::Failed)
		{
			ErrorCount++;
		}

		WorkItemStatus = newStatus;
	}

	uint16_t GetErrorCount() const
	{
		return ErrorCount;
	}

	uint16_t GetMaxErrorCount() const
	{
		return MaxErrorCount;
	}
};

template<class WorkType>
class DeviceDependentThreadPool
{
protected:
	inline static NosLib::DynamicArray<WorkHolder<WorkType>> WorkItemArray;
	inline static NosLib::DynamicArray<std::thread*> ThreadPool;

	inline static bool StopSignal = false;

	inline static int ThreadPoolCount = 20; /* How many threads to use, will be made dynamic */

	inline static void ManageThreads()
	{
		for (int i = 0; i <= ThreadPool.GetLastArrayIndex(); /* ArrayIndex will go down */)
		{
			ThreadPool[i]->join();
			ThreadPool.Remove(i);
		}

		WorkItemArray.Clear();
		/* in theory, thread deletes itself here */
	}

	inline static void ThreadPoolManagement(void(*workFunc)(NosLib::DynamicArray<WorkHolder<WorkType>>*, Ui::MainWindow*, bool*), const NosLib::DynamicArray<WorkType>& work, Ui::MainWindow* ui) /* Implement Stopping */
	{
		for (WorkType workItem : work)
		{
			WorkItemArray.Append(WorkHolder<WorkType>(workItem));
		}

		for (int i = 0; i < ThreadPoolCount; i++)
		{
			ThreadPool.Append(new std::thread(workFunc, &WorkItemArray, ui, &StopSignal));
			Sleep(10); /* desync threads */
		}

		ManageThreads();
	}

public:
	inline static void StartThreadPool(void(*workFunc)(NosLib::DynamicArray<WorkHolder<WorkType>>*, Ui::MainWindow*, bool*), NosLib::DynamicArray<WorkType>& work, Ui::MainWindow* ui)
	{
		std::thread thisThread(&DeviceDependentThreadPool<WorkType>::ThreadPoolManagement, workFunc, work, ui);
		thisThread.detach();
	}
};