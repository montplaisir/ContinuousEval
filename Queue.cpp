#include "Queue.hpp"

#include <unistd.h> // For usleep

omp_lock_t _lockForAdd;


void Queue::startAdding()
{
    omp_set_lock(&_lockForAdd);
}


// Add a point to the Queue
void Queue::addToQueue(const QueuePointPtr point)
{
    if (omp_test_lock(&_lockForAdd))
    {
        std::cerr << "Warning, tring to add an element to a queue that was not locked." << std::endl;
    }
    _queue.push_back(point);
}


void Queue::stopAdding()
{
    // TODO: Here, sort queue.
    omp_unset_lock(&_lockForAdd);
}


QueuePointPtr Queue::getTopPoint() const
{
    QueuePointPtr retPoint = nullptr;
    omp_set_lock(&_lockForAdd);
    if (!_queue.empty())
    {
        retPoint = _queue[_queue.size()-1];
    }
    omp_unset_lock(&_lockForAdd);
    return retPoint;
}


// Get the top Point from the Queue and pop it
// Return true if it worked, false if it failed.
bool Queue::popPoint(QueuePointPtr &point)
{
    bool success = false;
    // We need to set the lock before checking if
    // the queue is empty. Or else, we risk a seg fault.
    omp_set_lock(&_lockForAdd);  // the thread will wait until the lock is available.
    if (!_queue.empty())
    {
        // Remove last element, simulate a "pop".
        point = std::move(_queue[_queue.size()-1]);
        _queue.erase(_queue.end()-1);
        success = true;
    }
    omp_unset_lock(&_lockForAdd);

    return success;
}


bool Queue::run()
{
    bool successFound = false;
    bool doStopMainEval = false;

    // Queue runs forever on non-master threads.
    // On master, queue runs until stopMainEval() is true.
    while (!doStopMainEval && !_doneWithEval)
    {
        std::cout << "In Queue::run(). Thread: " << omp_get_thread_num() << std::endl;
        if (!_queue.empty())
        {
            //std::cout << "Thread: " << omp_get_thread_num() << " Calling EvalSinglePoint()." << std::endl;
            successFound = evalSinglePoint();
        }
        else if (!_doneWithEval)
        {
            std::cout << "Thread: " << omp_get_thread_num() << " Waiting for points." << std::endl;
        }
        else // Queue is empty and we are doneWithEval
        {
            break;
        }
   
        usleep(100000);
        omp_set_lock(&_lockForAdd);
        doStopMainEval = stopMainEval();
        omp_unset_lock(&_lockForAdd);
    }
    std::cout << "Thread " << omp_get_thread_num() << " is out of while loop" << std::endl;

    return successFound;
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
        //std::cout << "In thread: " << omp_get_thread_num() << " Eval point " << *point << " to " << eval << std::endl;
        point->setEval(eval);
        if (eval < point->getBestEval())
        {
            success = true;
            std::cout << "Thread " << omp_get_thread_num() << ". New success found: " << *point << std::endl;
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
    // Master thread
    if (omp_get_thread_num() == 0)
    {
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
    }

    return stop;
}


void Queue::setAllP1ToFalse()
{
    omp_set_lock(&_lockForAdd);
    if (!_queue.empty())
    {
        // For all P1 points - they are always at the front -
        // Pop, Set P1 to false, and push back.
        // Pop all P1 points (they are all at the front).
        // Set P1 to false.
        QueuePointPtr point;
        while (getTopPoint()->getP1() && popPoint(point))
        {
            std::cout << "Pop P1" << std::endl;
            point->setP1(false);
            _queue.push_back(point);
        }
        // VRM TODO sort queue here? / rewrite this method since we don't use a priority_queue anymore.
    }
    omp_unset_lock(&_lockForAdd);
}


void Queue::clearQueue()
{
    omp_set_lock(&_lockForAdd);
    _queue.clear();
    omp_unset_lock(&_lockForAdd);
}


void Queue::displayAndClear()
{
    omp_set_lock(&_lockForAdd);
    QueuePointPtr point;
    while (!_queue.empty() && popPoint(point))
    {
        std::cout << *point << std::endl;
    }
    omp_unset_lock(&_lockForAdd);
}



std::ostream& operator<<(std::ostream& out, const QueuePoint& point)
{
    //out << "X " << point.getX() << " Y " << point.getY() << " P1 " << point.getP1() << " best eval " << point.getBestEval();
    out << "X " << point.getX() << " Y " << point.getY() << " eval " << point.getEval();
    return out;
}





