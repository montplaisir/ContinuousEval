#include "Queue.hpp"

#include <algorithm>    // For sort
#include <unistd.h>     // For usleep

omp_lock_t _queueLock;

const bool debugLock = false;

void Queue::startAdding()
{
    if (debugLock) std::cout << "DEBUG: startAdding locks queue for thread " << omp_get_thread_num() << std::endl;
    omp_set_lock(&_queueLock);
}


// Add a point to the Queue
void Queue::addToQueue(const QueuePointPtr point)
{
    if (omp_test_lock(&_queueLock))
    {
        std::cerr << "Warning, tring to add an element to a queue that was not locked." << std::endl;
    }
    _queue.push_back(point);
}


void Queue::stopAdding()
{
    if (debugLock) std::cout << "DEBUG: stopAdding unlocks queue for thread " << omp_get_thread_num() << std::endl;
    omp_unset_lock(&_queueLock);
    // sort queue
    sort();
}


QueuePointPtr Queue::getTopPoint() const
{
    QueuePointPtr retPoint = nullptr;
    if (debugLock) std::cout << "DEBUG: getTopPoint locks queue for thread " << omp_get_thread_num() << std::endl;
    omp_set_lock(&_queueLock);
    if (!_queue.empty())
    {
        retPoint = _queue[_queue.size()-1];
    }
    if (debugLock) std::cout << "DEBUG: getTopPoint unlocks queue for thread " << omp_get_thread_num() << std::endl;
    omp_unset_lock(&_queueLock);
    return retPoint;
}


// Get the top Point from the Queue and pop it
// Return true if it worked, false if it failed.
bool Queue::popPoint(QueuePointPtr &point)
{
    bool success = false;
    // We need to set the lock before checking if
    // the queue is empty. Or else, we risk a seg fault.
    if (debugLock) std::cout << "DEBUG: popPoint locks queue for thread " << omp_get_thread_num() << std::endl;
    omp_set_lock(&_queueLock);  // the thread will wait until the lock is available.
    if (!_queue.empty())
    {
        // Remove last element, simulate a "pop".
        point = std::move(_queue[_queue.size()-1]);
        _queue.erase(_queue.end()-1);
        success = true;
    }
    if (debugLock) std::cout << "DEBUG: popPoint unlocks queue for thread " << omp_get_thread_num() << std::endl;
    omp_unset_lock(&_queueLock);

    return success;
}


bool Queue::run()
{
    bool successFound = false;

    // Queue runs forever on non-main threads.
    // On main threads, queue runs until stopMainEval() is true.
    bool conditionForStop = false;

    // conditionForStop is true if we are in a main thread and stopMainEval() returns true.
    while (!conditionForStop && !_doneWithEval)
    {
        #pragma omp critical(printInfo)
        {
            std::cout << "In Queue::run(). Thread: " << omp_get_thread_num() << std::endl;
        }
        // Check for stop conditions
        if (isMainThread(omp_get_thread_num()))
        {
            conditionForStop = stopMainEval();
        }

        if (!conditionForStop && !_queue.empty())
        {
            //std::cout << "Thread: " << omp_get_thread_num() << " Calling EvalSinglePoint()." << std::endl;
            successFound = evalSinglePoint();
        }
        else if (!_doneWithEval)
        {
            #pragma omp critical(printInfo)
            {
                std::cout << "Thread: " << omp_get_thread_num() << " Waiting for points." << std::endl;
            }
        }
        else // Queue is empty and we are doneWithEval
        {
            break;
        }
   
        usleep(100000);
    }   // End of while loop: Exit for main threads.
        // Other threads keep on looping.
    #pragma omp critical(printInfo)
    {
        std::cout << "Thread " << omp_get_thread_num() << " is out of while loop" << std::endl;
    }

    return successFound;
}


void Queue::stop()
{
    int threadNum = omp_get_thread_num();
    _mainThreadInfo.at(threadNum).setDoneWithEval(true);

    // Go through all main thread info to see if _doneWithEval must be set.
    // This part is not optimized, we can probably do better, but it does not 
    // seem like a bottleneck.
    bool allDone = true;
    for (int mainThreadNum : _mainThreads)
    {
        if (allDone && !_mainThreadInfo.at(mainThreadNum).getDoneWithEval())
        {
            allDone = false;
        }
    }

    if (allDone)
    {
        _doneWithEval = true;
    }

}


void Queue::sort(LowerPriority comp)
{
    if (!_queue.empty())
    {
        if (debugLock) std::cout << "DEBUG: sort locks queue for thread " << omp_get_thread_num() << std::endl;
        omp_set_lock(&_queueLock);

        std::sort(_queue.begin(), _queue.end(), comp);

        if (debugLock) std::cout << "DEBUG: sort unlocks queue for thread " << omp_get_thread_num() << std::endl;
        omp_unset_lock(&_queueLock);
    }
}


bool Queue::evalSinglePoint()
{
    bool success = false;

    QueuePointPtr point;
    bool pointAvailable = popPoint(point);

    // Simulate evaluation
    if (pointAvailable && 0 == point->getEval())
    {
        // Eval is between 1 and 50
        double eval = 1+std::rand()/((RAND_MAX + 1u)/50);
        #pragma omp critical(printInfo)
        {
            std::cout << "In thread: " << omp_get_thread_num() << " Eval point " << *point << " to " << eval << std::endl;
        }
        point->setEval(eval);
        if (eval < point->getBestEval())
        {
            success = true;
            #pragma omp critical(printInfo)
            {
                std::cout << "Thread " << omp_get_thread_num() << ". New success found: " << *point << std::endl;
            }
            // In NOMAD, we would update new point's best eval to this value.
            // Not done here - best eval is static.
        }
    }
    else
    {
        // else do nothing. No point available: either queue is empty, 
        // queue is locked, or point is already evaluated.
    }

    return success;
}


bool Queue::stopMainEval() const
{
    bool stop = false;
    // This method was called from a main thread. No need to verify
    // thread number.
    if (_queue.empty())
    {
        stop = true;
    }
    else
    {
        // Are we still evaluating P1s?
        // If we have a P1, we have not evaluated all P1.
        // If we don't have a P1, we can stop evaluation for
        // the main thread.
        bool stillInP1 = getTopPoint()->getP1();
        stop = !stillInP1;
    }

    return stop;
}


void Queue::setAllP1ToFalse()
{
    //omp_set_lock(&_queueLock);
    if (!_queue.empty())
    {
        // For all P1 points - they are always at the front -
        // Pop, Set P1 to false, and push back.
        // Pop all P1 points (they are all at the front).
        // Set P1 to false.
        QueuePointPtr point;
        while (getTopPoint()->getP1() && popPoint(point))
        {
            #pragma omp critical(printInfo)
            {
                std::cout << "Pop P1" << std::endl;
            }
            point->setP1(false);
            _queue.push_back(point);
        }
        // re-sort queue
        sort();
    }
    //omp_unset_lock(&_queueLock);
}


void Queue::clearQueue()
{
    if (debugLock) std::cout << "DEBUG: clearQueue locks queue for thread " << omp_get_thread_num() << std::endl;
    omp_set_lock(&_queueLock);
    _queue.clear();
    if (debugLock) std::cout << "DEBUG: clearQueue unlocks queue for thread " << omp_get_thread_num() << std::endl;
    omp_unset_lock(&_queueLock);
}


void Queue::displayAndClear()
{
    //omp_set_lock(&_queueLock);
    QueuePointPtr point;
    while (!_queue.empty() && popPoint(point))
    {
        #pragma omp critical(printInfo)
        {
            std::cout << *point << std::endl;
        }
    }
    //omp_unset_lock(&_queueLock);
}



std::ostream& operator<<(std::ostream& out, const QueuePoint& point)
{
    //out << "X " << point.getX() << " Y " << point.getY() << " P1 " << point.getP1() << " best eval " << point.getBestEval();
    out << "X " << point.getX() << " Y " << point.getY() << " eval " << point.getEval();
    return out;
}





