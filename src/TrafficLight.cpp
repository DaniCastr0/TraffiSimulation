#include <iostream>
#include <random>
#include "TrafficLight.h"
#include<future>
#include<queue>
#include<memory>


/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(_mutex);
    _queue.emplace_back(msg);
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto msg=Lightmsg->receive();
        if (msg==green) break;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    std::cout<<"interseccion2"<<std::endl;
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
    std::cout<<"interseccion3"<<std::endl;
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    std::chrono::time_point<std::chrono::system_clock> start,final;
    std::chrono::duration<double> diff;

    Lightmsg=std::make_shared<MessageQueue<TrafficLightPhase>>();
    while(1)
    {
        //taken from https://www.fluentcpp.com/2019/05/24/how-to-fill-a-cpp-collection-with-random-values/
        std::random_device random_device;
        std::mt19937 random_engine(random_device());
        std::uniform_real_distribution<double> distribution(4, 6);
 
        double randomNumber = (distribution(random_engine));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        start=std::chrono::system_clock::now();
        do
        {
            final=std::chrono::system_clock::now();
            diff=final-start;
        }while(diff.count()<randomNumber);
        if (_currentPhase==red)
        {
            _currentPhase=green;
        }
        else
        {
            _currentPhase=red;
        }
        auto message=_currentPhase;
        std::future<void> ftr=std::async(&MessageQueue<TrafficLightPhase>::send,Lightmsg,std::move(message));
        ftr.wait();
        

    }
}

