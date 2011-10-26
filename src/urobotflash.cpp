/* 
 * File:   URobotFlash.cpp
 * Author: aoleksy
 * 
 * Created on 15 kwiecień 2011, 11:09
 */

#include "urobotflash.h"

#include <cmath>
#include <cstring>
#include <iostream>

using namespace PlayerCc;
using namespace std;
using namespace urbi;
using namespace boost;


URobotFlash::URobotFlash(const std::string& s) :
        mRobotProxy(NULL),
        mSpeedControllerProxy(NULL),
        mPlannerProxy(NULL),
        mLocalizeProxy(NULL),
        mPositionProxy(NULL),
        mLaserProxy(NULL),
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
    UBindThreadedFunction(URobotFlash, goToGoalPose, LOCK_NONE);
    UBindFunction(URobotFlash, isGoalPoseReached);
    UBindFunction(URobotFlash, getActualXPos);
    UBindFunction(URobotFlash, getActualYPos);
    UBindFunction(URobotFlash, getActualAnglePos);
    UBindFunction(URobotFlash, getGoalXPos);
    UBindFunction(URobotFlash, getGoalYPos);
    UBindFunction(URobotFlash, getGoalAnglePos);
    
    UBindFunction(URobotFlash, setPose);
    
    UBindFunction(URobotFlash, getMinDist);
    UBindFunction(URobotFlash, getMaxDist);
    UBindFunction(URobotFlash, getDist);
}

URobotFlash::~URobotFlash() {
    disconnect();
}

void URobotFlash::setGoalPose(double goalX, double goalY, double goalAngle) {
    if(!isConnected())
        return;
    
    switchController(NavigationController);
    mPlannerProxy->SetGoalPose(goalX, goalY, goalAngle);
}

bool URobotFlash::goToGoalPose(double goalX, double goalY, double goalAngle) {
    if(!isConnected())
        return false;
    
    int waitIter = 10;
    
    stopRobot();
    this_thread::sleep(posix_time::milliseconds(200));
    
    setGoalPose(goalX, goalY, goalAngle);
    // Trzeba z piec razy czytac dane;
    for (int i = 0; i < 5; ++i) {
        volatile double tmp;
        tmp = mPlannerProxy->GetPathValid();
        tmp = mPlannerProxy->GetPathDone();
        this_thread::sleep(posix_time::milliseconds(100));
    }
    
    // Glowna petla oczekiwania
    while(true) {
	std:cerr << "URobotFlash::goToGoalPose" << std::endl;
        this_thread::sleep(posix_time::milliseconds(100));
        if(mPlannerProxy->GetPathValid() == 0 && (--waitIter) == 0)
            return false;
        else if (mPlannerProxy->GetPathDone() != 0)
            return true;
        else if (mCurrentControllerType != NavigationController)
            return false;
    }
}

bool URobotFlash::connect(const std::string& hostname, uint port) {
    // Jeśli połączony, to wyłącz wątek i rozłącz
    if(isConnected())
        disconnect();
    // Sprubuj się połączyć
    try {
        // Połącz z serwerem
        mRobotProxy.reset(new PlayerClient(hostname, port)); // Jeśli się nie powiedzie to catch;
        // Pobierz listę urządzeń
        mRobotProxy->RequestDeviceList();
        DeviceInfoList deviceInfoList = mRobotProxy->GetDeviceList();
        
        // Należy znaleźć p2os lub flash i wavefront
        DeviceInfoList::const_iterator dev;
        if ((dev = findDevice(deviceInfoList, "p2os", PLAYER_POSITION2D_CODE)) == deviceInfoList.end()
                && (dev = findDevice(deviceInfoList, "flash", PLAYER_POSITION2D_CODE)) == deviceInfoList.end())
            throw PlayerError();
        else
            mSpeedControllerProxy.reset(new Position2dProxy(mRobotProxy.get(), dev->addr.index));
        
        if ((dev = findDevice(deviceInfoList, "wavefront", PLAYER_PLANNER_CODE)) == deviceInfoList.end())
            throw PlayerError();
        else
            mPlannerProxy.reset(new PlannerProxy(mRobotProxy.get(), dev->addr.index));
        
        if ((dev = findDevice(deviceInfoList, "amcl", PLAYER_LOCALIZE_CODE)) == deviceInfoList.end())
            throw PlayerError();
        else
            mLocalizeProxy.reset(new LocalizeProxy(mRobotProxy.get(), dev->addr.index));
        
        if ((dev = findDevice(deviceInfoList, "amcl", PLAYER_POSITION2D_CODE)) == deviceInfoList.end())
            throw PlayerError();
        else
            mPositionProxy.reset(new Position2dProxy(mRobotProxy.get(), dev->addr.index));
        
        if ((dev = findDevice(deviceInfoList, "urg", PLAYER_LASER_CODE)) == deviceInfoList.end()
                && (dev = findDevice(deviceInfoList, "sicklms200", PLAYER_LASER_CODE)) == deviceInfoList.end())
            throw PlayerError();
        else
            mLaserProxy.reset(new LaserProxy(mRobotProxy.get(), dev->addr.index));
        
        //Stworz watek - NA KONCU!
        mPlannerProxy->SetEnable(false);
        mPositionProxy->RequestGeom();
        mSpeedControlThread = thread(&URobotFlash::speedControlThread, this);
        mRobotProxy->Read();
        //mRobotProxy->StartThread();
    } catch (...) {
        // Nie udało się, rozłącz wszystko
        mSpeedControllerProxy.reset(NULL);
        mPlannerProxy.reset(NULL);
        mPositionProxy.reset(NULL);
        mLocalizeProxy.reset(NULL);
        mLaserProxy.reset(NULL);
        mRobotProxy.reset(NULL);
        return (mIsConnected = false);
    }
    return (mIsConnected = true);
}

void URobotFlash::disconnect() {
    if(isConnected()) {
        stopRobot();
        mSpeedControlThread.interrupt();
        mSpeedControlThread.join();
        mLocalizeProxy.reset(NULL);
        mSpeedControllerProxy.reset(NULL);
        mPlannerProxy.reset(NULL);
        mPositionProxy.reset(NULL);
        mLaserProxy.reset(NULL);
        //mRobotProxy->StopThread();
        mRobotProxy.reset(NULL);
        mIsConnected = false;
    }
}

void URobotFlash::speedControlThread() {
    while(true) {
        {
            mURobotFlashThreadMutex.lock();
            mSpeedControllerProxy->SetSpeed(mXSpeed, mYawSpeed);
            mURobotFlashThreadMutex.unlock();
        }
        
        try {
            this_thread::sleep(posix_time::milliseconds(10));
        } catch(thread_interrupted&) {
            lock_guard<mutex> lock(mURobotFlashThreadMutex);
            mSpeedControllerProxy->SetSpeed(0.0, 0.0);
            return;
        }
    }
}

void URobotFlash::switchController(ControllerType controllerType) {
    if (mCurrentControllerType == NavigationController && controllerType == SpeedController) {
        mPlannerProxy->SetEnable(false);
        mSpeedControlThread = thread(&URobotFlash::speedControlThread, this);
        mCurrentControllerType = SpeedController;
        return;
    }
    else if (mCurrentControllerType == SpeedController && controllerType == NavigationController) {
        mSpeedControlThread.interrupt();
        mSpeedControlThread.join();
        mPlannerProxy->SetEnable(true);
        mCurrentControllerType = NavigationController;
        return;
    }
}

inline bool URobotFlash::isGoalPoseReached() const {
    return isConnected() ? mPlannerProxy->GetPathDone() : false;
}

inline double URobotFlash::getActualXPos() const {
    return isConnected() ? mPositionProxy->GetXPos() : 0.0;
}

inline double URobotFlash::getActualYPos() const {
    return isConnected() ? mPositionProxy->GetYPos() : 0.0;
}

inline double URobotFlash::getActualAnglePos() const {
    return isConnected() ? mPositionProxy->GetYaw() : 0.0;
}

double URobotFlash::getMinDist() const {
    if(!isConnected())
        return 0.0;
    
    vector<double> dist = getDist();
    return *min_element(dist.begin(), dist.end());
}

inline double URobotFlash::getMaxDist() const {
    if(!isConnected())
        return 0.0;
    
    vector<double> dist = getDist();
    return *max_element(dist.begin(), dist.end());
}

vector<double> URobotFlash::getDist() const {
    vector<double> dist(mLaserProxy->GetCount());
    
    for (int i = 0; i < dist.size(); ++i) {
        dist[i] = mLaserProxy->operator [](i);
    }
    
    return dist;
}

UStart(URobotFlash);
