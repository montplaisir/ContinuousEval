
#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include <vector>

#include "QueuePoint.hpp"

class MainThreadInfo
{
};

class Queue
{
private:
    // The Queue of Points
    std::vector<QueuePointPtr> _queue;
    bool _doneWithEval;         // All evaluations done. Queue can be destroyed.
    mutable omp_lock_t _lockForAdd;     // When we add, we do not want to eval.


public:
    // Constructor
    explicit Queue()
      : _queue(),
        _doneWithEval(false),
        _lockForAdd()
    {
        omp_init_lock(&_lockForAdd);
        //run();    // Do not start queue here: wait until we are in parallel zone.
    }

    // Destructor, could be needed to destroy lock.
    virtual ~Queue()
    {
        omp_destroy_lock(&_lockForAdd);
    }

    // Get/Set
    int getQueueSize() const { return _queue.size(); }

    // Other methods

    // Start evaluation
    void start() {}
    // Continuous evaluation
    bool run();
    // Stop evaluation
    void stop() { _doneWithEval = true; }

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
