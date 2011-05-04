/* 
 * File:   URobotFlash.cpp
 * Author: aoleksy
 * 
 * Created on 15 kwiecień 2011, 11:09
 */

#include "urobotflash.h"

#include <iostream>

using namespace PlayerCc;
using namespace std;
using namespace urbi;
using namespace boost;


URobotFlash::URobotFlash(const std::string& s) :
        mRobot(NULL),
        mPosition(NULL),
        mPlanner(NULL),
        mIsConnected(false),
        mXSpeed(0.0), mYawSpeed(0.0),
        mCurrentControllerType(SpeedController),
        UObject(s)
{
    UBindFunction(URobotFlash, connect);
    UBindFunction(URobotFlash, disconnect);
    UBindFunction(URobotFlash, isConnected);
    
    UBindFunction(URobotFlash, setSpeed);
    UBindFunction(URobotFlash, setXSpeed);
    UBindFunction(URobotFlash, setYawSpeed);
    UBindFunction(URobotFlash, stopRobot);
    UBindFunction(URobotFlash, getActualXSpeed);
    UBindFunction(URobotFlash, getActualYawSpeed);
    
    UBindFunction(URobotFlash, setGoalPose);
    UBindFunction(URobotFlash, goToGoalPose);
    UBindFunction(URobotFlash, isGoalPoseReached);
    UBindFunction(URobotFlash, getActualXPos);
    UBindFunction(URobotFlash, getActualYPos);
    UBindFunction(URobotFlash, getActualAnglePos);
    UBindFunction(URobotFlash, getGoalXPos);
    UBindFunction(URobotFlash, getGoalYPos);
    UBindFunction(URobotFlash, getGoalAnglePos);
}

URobotFlash::~URobotFlash() {
    disconnect();
}

void URobotFlash::setGoalPose(double goalX, double goalY, double goalAngle) {
    if(!isConnected())
        return;
    
    switchController(NavigationController);
    mPlanner->SetGoalPose(goalX, goalY, goalAngle);
}

bool URobotFlash::goToGoalPose(double goalX, double goalY, double goalAngle) {
    
}

bool URobotFlash::connect(const std::string& hostname, uint port) {
    // Jeśli połączony, to wyłącz wątek i rozłącz
    if(isConnected())
        disconnect();
    // Sprubuj się połączyć
    try {
        // Połącz z serwerem
        mRobot.reset(new PlayerClient(hostname, port)); // Jeśli się nie powiedzie to catch;
        // Pobierz listę urządzeń
        mRobot->RequestDeviceList();
        DeviceInfoList deviceInfoList = mRobot->GetDeviceList();
        
        // Należy znaleźć p2os lub flash i wavefront
        DeviceInfoList::const_iterator dev;
        if ((dev = findDevice(deviceInfoList, "p2os")) == deviceInfoList.end()
                && (dev = findDevice(deviceInfoList, "flash")) == deviceInfoList.end())
            throw PlayerError();
        else
            mPosition.reset(new Position2dProxy(mRobot.get(), dev->addr.index));
        
        if ((dev = findDevice(deviceInfoList, "wavefront")) == deviceInfoList.end())
            throw PlayerError();
        else
            mPlanner.reset(new PlannerProxy(mRobot.get(), dev->addr.index));
        
        //Stworz watek - NA KONCU!
        mPlanner->SetEnable(false);
        mSpeedControlThread = thread(&URobotFlash::speedControlThread, this);
    } catch (...) {
        // Nie udało się, rozłącz wszystko
        mPosition.reset(NULL);
        mPlanner.reset(NULL);
        mRobot.reset(NULL);
        return (mIsConnected = false);
    }
    return (mIsConnected = true);
}

void URobotFlash::disconnect() {
    if(isConnected()) {
        mSpeedControlThread.interrupt();
        mSpeedControlThread.join();
        mPosition.reset(NULL);
        mPlanner.reset(NULL);
        mRobot.reset(NULL);
        mIsConnected = false;
    }
}

void URobotFlash::speedControlThread() {
    posix_time::milliseconds workTime(10);
    while(true) {
        {
            mURobotFlashThreadMutex.lock();
            mPosition->SetSpeed(mXSpeed, mYawSpeed);
            mURobotFlashThreadMutex.unlock();
        }
        
        try {
            this_thread::sleep(workTime);
        } catch(thread_interrupted&) {
            lock_guard<mutex> lock(mURobotFlashThreadMutex);
            mPosition->SetSpeed(0.0, 0.0);
            return;
        }
    }
}

void URobotFlash::switchController(ControllerType controllerType) {
    if (mCurrentControllerType == NavigationController && controllerType == SpeedController) {
        mPlanner->SetEnable(false);
        mSpeedControlThread = thread(&URobotFlash::speedControlThread, this);
        mCurrentControllerType = SpeedController;
        return;
    }
    else if (mCurrentControllerType == SpeedController && controllerType == NavigationController) {
        mSpeedControlThread.interrupt();
        mSpeedControlThread.join();
        mPlanner->SetEnable(true);
        mCurrentControllerType = NavigationController;
        return;
    }
}

UStart(URobotFlash);
