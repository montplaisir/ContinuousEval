
#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include <functional>   // For std::function
#include <iostream>
#include <memory>       // For shared_ptr
#include <omp.h>
#include <queue>        // For priority_queue

class QueuePoint
{
private:
    // In fact, the QueuePoint must have a pointer to some kind
    // of point (e.g. EvalPoint), not replicate its members.
    // I.e. _x,_y but also _eval should be hidden, and the _comp
    // of the Queue is related to that outside point class.
    // Not sure where _bestEval would reside.
    // _P1 remains here.

    // Point has 2 coordinates
    double  _x;
    double  _y;
    // Value of evaluation
    double _eval;
    // Value to which evaluation will be compared
    double  _bestEval;
    // Is this a "priority 1" point to eval?
    // P1 points are always evaluated first, and the algorithm
    // only continues when all P1 points are evaluated / or when
    // a success is found.
    bool _P1;

public:
    QueuePoint(double x, double y, double bestEval)
      : _x(x),
        _y(y),
        _eval(0),
        _bestEval(bestEval),
        _P1(false)
    {}

    // Get/Set
    double getX() const { return _x; }
    double getY() const { return _y; }
    double getEval() const { return _eval; }
    void setEval(const double eval) { _eval = eval; }
    double getBestEval() const { return _bestEval; }
    void setP1(const bool p1) { _P1 = p1; }
    bool getP1() const { return _P1; }

};

typedef std::shared_ptr<QueuePoint> QueuePointPtr;

std::ostream& operator<<(std::ostream& out, const QueuePoint& point);


class LowerPriority {
private:
    std::function<bool(QueuePointPtr &p1, QueuePointPtr &p2)> _comp;

public:
    // Constructor
    LowerPriority(const std::function<bool(QueuePointPtr &p1, QueuePointPtr &p2)> comp = std::function<bool(QueuePointPtr &p1, QueuePointPtr &p2)>(DefaultComp))
      : _comp(comp)
    {}


    // Comparison operator for sorting queue points (at insertion).
    // Points P1 are always prioritary.
    bool operator()(QueuePointPtr& p1, QueuePointPtr& p2)
    {
        bool hasLowerPriority = false;
        
        if (p1->getP1() != p2->getP1())
        {
            hasLowerPriority = (p1->getP1() < p2->getP1());
        }
        else
        {
            hasLowerPriority = _comp(p1, p2);
        }

        return hasLowerPriority;
    }

    // Priority is lower if evaluation is higher.
    static bool DefaultComp(QueuePointPtr& p1, QueuePointPtr& p2)
    {
        return (p1->getBestEval() > p2->getBestEval());
    }
};


class Queue {
private:
    // The Queue of Points
    std::priority_queue<QueuePointPtr, 
                        std::vector<QueuePointPtr>,
                        LowerPriority> _queue;
    bool _doneWithEval;         // All evaluations done. Queue can be destroyed.
    //static int _sLocksForAdd;   // When we add, we do not want to eval.
    omp_lock_t _lockForAdd;     // When we add, we do not want to eval.


public:
    // Constructor
    explicit Queue(LowerPriority lowerPriority)
      : _queue(lowerPriority),
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

    // get the top Point from the queue and pop it.
    // Return true if it worked, false otherwise.
    bool popPoint(QueuePointPtr &point);

    // Display all points in the queue, in order of priority.
    // Clear it at the same time.
    void displayAndClear();


    void clearQueue();


};

#endif // __QUEUE_HPP__
