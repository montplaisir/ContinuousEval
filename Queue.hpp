
#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include <map>
#include <set>
#include <vector>

#include "QueuePoint.hpp"

class MainThreadInfo
{
private:
    bool _doneWithEval;             // All evaluations done for this main thread

public:
    explicit MainThreadInfo()
      : _doneWithEval(false)
    {
    }

    void setDoneWithEval(const bool doneWithEval)
    {
        _doneWithEval = doneWithEval;
    }

    bool getDoneWithEval() const { return _doneWithEval; }

};


class Queue
{
private:
    std::vector<QueuePointPtr> _queue;  // The queue of points
    LowerPriority _comp;            // Comparison function used for sorting
    bool _doneWithEval;             // All evaluations done for all main threads. Queue can be destroyed.
    mutable omp_lock_t _queueLock;  // Do not launch new evaluations when queue is locked, e.g. for adding points.
    std::set<int> _mainThreads;     // Thread numbers of main threads
    std::map<int, MainThreadInfo> _mainThreadInfo;


public:
    // Constructor
    explicit Queue(LowerPriority comp)
      : _queue(),
        _comp(comp),
        _doneWithEval(false),
        _queueLock(),
        _mainThreads(),
        _mainThreadInfo()
    {
        omp_init_lock(&_queueLock);
        addMainThread(omp_get_thread_num());
        //run();    // Do not start queue here: wait until we are in parallel zone.
    }

    // Destructor, could be needed to destroy lock.
    virtual ~Queue()
    {
        omp_destroy_lock(&_queueLock);
    }

    // Get/Set
    int getQueueSize() const { return _queue.size(); }

    void addMainThread(const int threadNum)
    {
        _mainThreads.insert(threadNum);
        MainThreadInfo threadInfo;
        auto threadInfoPair = std::pair<const int, const MainThreadInfo&>(threadNum, threadInfo);
        _mainThreadInfo.insert(threadInfoPair);
    }
    bool isMainThread(const int threadNum) const { return (_mainThreads.end() != _mainThreads.find(threadNum)); }
    const std::set<int>& getMainThreads() const { return _mainThreads; }
    size_t getNbMainThreads() const { return _mainThreads.size(); }

    // Other methods

    // Start evaluation
    void start() {}
    // Continuous evaluation
    bool run();
    // Stop evaluation for the current thread (which should be a main thread)
    void stop();

    /// Sort the queue with respect to the comparison function comp.
    void sort(LowerPriority comp);

    /// Use the default comparison function _comp.
    void sort() { sort(_comp); }
  
    // Eval a single point (mock eval). Pop it from queue.
    // Return true (success) if eval is better than point's best eval.
    bool evalSinglePoint();

    // Did we reach a stop condition (for main thread)?
    bool stopMainEval() const;

    // Set all P1 to false.
    void setAllP1ToFalse();

    // Notify the queue that we will add points.
    void startAdding();
    // Notify the queue that we are done adding points.
    void stopAdding();
    // Add a single Point to the Queue
    void addToQueue(const QueuePointPtr point);

    QueuePointPtr getTopPoint() const;

    // get the top Point from the queue and pop it.
    // Return true if it worked, false otherwise.
    bool popPoint(QueuePointPtr &point);

    // Display all points in the queue
    // Clear it at the same time.
    void displayAndClear();


    void clearQueue();


};

#endif // __QUEUE_HPP__
