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
        mURobotFlashThread(NULL),
        mIsConnected(false),
        mXSpeed(0.0), mYawSpeed(0.0),
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
    
    if(!mIsNavEnabled) {
        mIsNavEnabled = true;
        mPlanner->SetEnable(true);
        mBlockNavPos.lock();
    }
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
        mURobotFlashThread.reset(new thread(&URobotFlash::threadMain, this));
    } catch (...) {
        // Nie udało się, rozłącz wszystko
        mPosition.reset(NULL);
        mPlanner.reset(NULL);
        mRobot.reset(NULL);
        mURobotFlashThread.reset(NULL);
        return (mIsConnected = false);
    }
    return (mIsConnected = true);
}

void URobotFlash::disconnect() {
    if(isConnected()) {
        mURobotFlashThread->interrupt();
        mURobotFlashThread->join();
        mURobotFlashThread.reset(NULL);
        mPosition.reset(NULL);
        mPlanner.reset(NULL);
        mRobot.reset(NULL);
        mIsConnected = false;
    }
}

void URobotFlash::threadMain() {
    posix_time::milliseconds workTime(10);
    while(true) {
        {
            mBlockNavPos.lock(); mURobotFlashThreadMutex.lock();
            mPosition->SetSpeed(mXSpeed, mYawSpeed);
            mBlockNavPos.unlock(); mURobotFlashThreadMutex.unlock();
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

UStart(URobotFlash);
