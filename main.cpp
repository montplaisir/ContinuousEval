
#include "Queue.hpp"

// Static class...
class OrderByDirection
{
private:
    static double _dirX;
    static double _dirY;

public:
    // Get/Set
    static void setDirX(double dirX) { _dirX = dirX; }
    static void setDirY(double dirY) { _dirY = dirY; }
    static double getDirX() { return _dirX; }
    static double getDirY() { return _dirY; }

    // Simple implementation of sorting points according
    // to a direction. Points that match this direction
    // the most have a higher priority.
    static bool comp(QueuePointPtr& p1, QueuePointPtr& p2)
    {
        bool lowerPriority = false;

        double d1x = (p1->getX() - _dirX);
        double d1y = (p1->getY() - _dirY);
        double val1 = (d1x * d1x) + (d1y * d1y);    // square norm

        double d2x = (p2->getX() - _dirX);
        double d2y = (p2->getY() - _dirY);
        double val2 = (d2x * d2x) + (d2y * d2y);

        // The point farthest from _dir gets lower priority.
        if (val1 > val2)
        {
            lowerPriority = true;
        }

        return lowerPriority;
    }
};


// Init statics
double OrderByDirection::_dirX = 0.0;
double OrderByDirection::_dirY = 0.0;


// Calling argument: Number of threads to use.
int main(int argc , char **argv)
{
    int nbThreads = omp_get_max_threads();
    int nbMainThreads = nbThreads / 3 + 1;
    if (argc > 1)
    {
        nbThreads = std::atoi(argv[1]);
        nbMainThreads = nbThreads / 3 + 1;
        if (0 == nbThreads)
        {
            nbThreads = omp_get_max_threads();
        }
        if (argc > 2)
        {
            nbMainThreads = std::atoi(argv[2]);
            if (0 == nbMainThreads)
            {
                nbMainThreads = 1;
            }
        }
    }
    if (nbThreads < nbMainThreads)
    {
        std::cerr << "Error: number of main threads (" << nbMainThreads << ") should be less or equal to number of threads (" << nbThreads << ")." << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Working with " << nbMainThreads << " main thread" << (nbMainThreads > 1 ? "s" : "") << " on a total of " << nbThreads << " thread" << (nbThreads > 1 ? "s" : "") << "." << std::endl;
    }

    // Create queue for all threads
    LowerPriority orderByDirection(OrderByDirection::comp);
    OrderByDirection::setDirX(6);
    OrderByDirection::setDirY(-2);
    // June 2020: Queue is now a vector, instead of using a priority_queue.
    // Sorting is done when stopAdding() is called.
    Queue queue(orderByDirection);
    queue.start();
    std::cout << "Start main" << std::endl;

    // Init points here... for testing purposes.
    auto pP1 = QueuePointPtr(new QueuePoint(5, -2, 59)); // x, y, eval
    auto pP2 = QueuePointPtr(new QueuePoint(4, -2, 58));
    auto pP3 = QueuePointPtr(new QueuePoint(5, -1, 40));
    auto pP4 = QueuePointPtr(new QueuePoint(4, -1, 76));
    auto pP5 = QueuePointPtr(new QueuePoint(5, -3, 9));
    auto pP6 = QueuePointPtr(new QueuePoint(4, -3, 38));
    auto pP7 = QueuePointPtr(new QueuePoint(6, -2, 23));
    auto pP8 = QueuePointPtr(new QueuePoint(6, -2, 31));
    auto pP9 = QueuePointPtr(new QueuePoint(6, -1, 57));
    auto pP10 = QueuePointPtr(new QueuePoint(6, -1, 11));
    auto pP11 = QueuePointPtr(new QueuePoint(6, -3, 85));
    auto pP12 = QueuePointPtr(new QueuePoint(6, -3, 22));
    auto pP13 = QueuePointPtr(new QueuePoint(7, -2, 49));
    auto pP14 = QueuePointPtr(new QueuePoint(7, -1, 66));
    auto pP15 = QueuePointPtr(new QueuePoint(7, -3, 91));
    auto pP16 = QueuePointPtr(new QueuePoint(4, -4, 28));
    auto pP17 = QueuePointPtr(new QueuePoint(6, -4, 13));
    auto pP18 = QueuePointPtr(new QueuePoint(6, -5, 21));
    auto pP19 = QueuePointPtr(new QueuePoint(6, -5, 47));
    auto pP20 = QueuePointPtr(new QueuePoint(7, -5, 1));
    auto pP21 = QueuePointPtr(new QueuePoint(7, -5, 75));
    auto pP22 = QueuePointPtr(new QueuePoint(8, -3, 12));
    auto pP23 = QueuePointPtr(new QueuePoint(8, -2, 48));
    auto pP24 = QueuePointPtr(new QueuePoint(8, -1, 30));
    auto pP25 = QueuePointPtr(new QueuePoint(7.1, -3.1, 48));
    auto pP26 = QueuePointPtr(new QueuePoint(4.1, -4.1, 43));
    auto pP27 = QueuePointPtr(new QueuePoint(6.1, -4.1, 60));
    auto pP28 = QueuePointPtr(new QueuePoint(6.1, -5.1, 63));
    auto pP29 = QueuePointPtr(new QueuePoint(6.1, -5.2, 24));
    auto pP30 = QueuePointPtr(new QueuePoint(7.1, -5.2, 49));
    auto pP31 = QueuePointPtr(new QueuePoint(7.1, -5.3, 63));
    auto pP32 = QueuePointPtr(new QueuePoint(8.1, -3.3, 25));
    auto pP33 = QueuePointPtr(new QueuePoint(8.1, -2.4, 53));
    auto pP34 = QueuePointPtr(new QueuePoint(8.1, -1.4, 17));
    auto pP35 = QueuePointPtr(new QueuePoint(7.1, -3.5, 5));
    auto pP36 = QueuePointPtr(new QueuePoint(4.1, -4.5, 53));
    auto pP37 = QueuePointPtr(new QueuePoint(6.1, -4.6, 47));
    auto pP38 = QueuePointPtr(new QueuePoint(6.1, -5.6, 59));
    auto pP39 = QueuePointPtr(new QueuePoint(6.1, -5.7, 47));
    auto pP40 = QueuePointPtr(new QueuePoint(7.1, -5.7, 95));
    auto pP41 = QueuePointPtr(new QueuePoint(7.1, -5.8, 93));
    auto pP42 = QueuePointPtr(new QueuePoint(8.1, -3.8, 85));
    auto pP43 = QueuePointPtr(new QueuePoint(8.1, -2.9, 59));
    auto pP44 = QueuePointPtr(new QueuePoint(8.1, -1.11, 5));
    auto pP45 = QueuePointPtr(new QueuePoint(7.1, -3.11, 46));
    auto pP46 = QueuePointPtr(new QueuePoint(4.1, -4.11, 83));
    auto pP47 = QueuePointPtr(new QueuePoint(6.1, -4.8, 54));
    auto pP48 = QueuePointPtr(new QueuePoint(6.1, -5.9, 38));
    auto pP49 = QueuePointPtr(new QueuePoint(6.1, -5.11, 18));

    // Set P1 for these points
    pP6->setP1(true);
    pP7->setP1(true);
    pP8->setP1(true);
    pP13->setP1(true);
    pP14->setP1(true);
    pP15->setP1(true);
    pP16->setP1(true);
    pP17->setP1(true);
    pP23->setP1(true);

    // Start all processes
    #pragma omp parallel num_threads(nbThreads) default(shared)
    {
        // June 2020: Modify code so we have nbMainThreads main threads, instead of one.
        // Other threads are secundary threads, for evaluations only.
        //
        // Create points in main threads only
        // The first nbMainThreads threads that reach this point are considered main threads.
        // The master thread (number 0) has to be in that set.
        int threadNum = omp_get_thread_num();
        #pragma omp critical(printInfo)
        {
            if (queue.getNbMainThreads() < nbMainThreads)
            {
                queue.addMainThread(threadNum);
            }
        }

        #pragma omp barrier

        #pragma omp single
        {
            std::cout << "Main threads are:";
            for (int thnum : queue.getMainThreads())
            {
                std::cout << " " << thnum;
            }
            std::cout << std::endl;
        }


        // Each main thread will generate some points and add them to the queue.
        if (queue.isMainThread(threadNum))
        {
            #pragma omp single nowait
            {
                //std::cout << "Start adding points for main thread " << threadNum << std::endl;
                queue.startAdding();
                for (QueuePointPtr pp : { pP1, pP2, pP3, pP4, pP5, pP6, pP7, pP8 })
                {
                    queue.addToQueue(pp);
                }
                queue.stopAdding();
                //std::cout << "Done adding points for main thread " << threadNum << std::endl;
            }
            #pragma omp single nowait
            {
                //std::cout << "Start adding points for main thread " << threadNum << std::endl;
                queue.startAdding();
                for (QueuePointPtr pp : { pP9, pP10, pP11, pP12, pP13, pP14, pP15, pP16 })
                {
                    queue.addToQueue(pp);
                }
                queue.stopAdding();
                //std::cout << "Done adding points for main thread " << threadNum << std::endl;
            }
            #pragma omp single nowait
            {
                //std::cout << "Start adding points for main thread " << threadNum << std::endl;
                queue.startAdding();
                for (QueuePointPtr pp : { pP17, pP18, pP19, pP20, pP21, pP22, pP23, pP24 })
                {
                    queue.addToQueue(pp);
                }
                queue.stopAdding();
                //std::cout << "Done adding points for main thread " << threadNum << std::endl;
            }

        }   // End of adding points, main threads only

        // Adding a barrier here is not ideal. TODO see if we can get rid of it.
        //#pragma omp barrier

        // Launch evaluation on all threads, including master.
        #pragma omp critical(printInfo)
        {
            std::cout << "Launch run for thread " << omp_get_thread_num() << std::endl;
        }
        queue.run();
        #pragma omp critical(printInfo)
        {
            std::cout << "Done running for thread " << omp_get_thread_num() << std::endl;
        }

        // From here, only main threads are out of queue.run(). The other
        // threads are waiting for evaluation.
        // So we work on main threads only.
        // Warning: pragma omp single cannot be used here, because it would wait endlessly
        // for non-main threads.
        // "pragma omp single nowait" may be used, but with caution, as the next steps
        // might start before the current step is done.
        if (queue.isMainThread(threadNum))
        {
            #pragma omp single nowait
            {
                queue.setAllP1ToFalse();
            }

            #pragma omp single nowait
            {
                #pragma omp critical(printInfo)
                {
                    std::cout << std::endl << "Adding new points via main thread " << threadNum << "..." << std::endl;
                }
                queue.startAdding();
                for (QueuePointPtr pp : { pP1, pP2, pP3, pP4, pP5, pP6, pP13, pP14, \
                                          pP15, pP16, pP17, pP18, pP19, pP20, pP21, \
                                        })
                {
                    queue.addToQueue(pp);
                }
                queue.stopAdding();
            }
            #pragma omp single nowait
            {
                #pragma omp critical(printInfo)
                {
                    std::cout << std::endl << "Adding new points via main thread " << threadNum << "..." << std::endl;
                }
                queue.startAdding();
                for (QueuePointPtr pp : { pP22, pP23, pP24, pP25, pP26, pP27, pP28, \
                                          pP29, pP30, pP31, pP32, pP33, pP34, pP35, \
                                        })
                {
                    queue.addToQueue(pp);
                }
                queue.stopAdding();
            }
            #pragma omp single nowait
            {
                #pragma omp critical(printInfo)
                {
                    std::cout << std::endl << "Adding new points via main thread " << threadNum << "..." << std::endl;
                }
                queue.startAdding();
                for (QueuePointPtr pp : { pP36, pP37, pP38, pP39, pP40, pP41, pP42, \
                                          pP43, pP44, pP45, pP46, pP47, pP48, pP49
                                        })
                {
                    queue.addToQueue(pp);
                }
                queue.stopAdding();
            }

            // All the threads other than main are still available for evaluation.
            // Re-launch run for main threads only.
            #pragma omp critical(printInfo)
            {
                std::cout << "ReLaunch run for thread " << omp_get_thread_num() << std::endl;
            }
            queue.run();

            #pragma omp critical(printInfo)
            {
                std::cout << "Ready to stop for main thread " << omp_get_thread_num() << std::endl;
            }
            // Stop queue for this main thread
            queue.stop();
        }   // End main thread
    }   // End parallel region

    return 0;
}
