/* 
 * File:   URobotFlash.cpp
 * Author: aoleksy
 * 
 * Created on 15 kwiecień 2011, 11:09
 */

#include "urobotflash.h"
#include <player-2.0/libplayerc++/playerclient.h>
#include <boost/thread/detail/thread.hpp>

#include <algorithm>

using namespace PlayerCc;
using namespace std;
using namespace urbi;


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

void URobotFlash::setGoalPose(double goalX, double goalY, double goalAngle) {
    
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
        list<playerc_device_info_t> deviceList = mRobot->GetDeviceList();
        // Należy znaleźć p2os lub flash i wavefront
    } catch (PlayerError&) {
        // Nie udało się, rozłącz wszystko
        mPosition.reset(NULL);
        mPlanner.reset(NULL);
        mRobot.reset(NULL);
        return false;
    }
    return true;
}

void URobotFlash::disconnect() {
    if(isConnected()) {
        mURobotFlashThread->interrupt();
        mPosition.reset(NULL);
        mPlanner.reset(NULL);
        mRobot.reset(NULL);
        mIsConnected = false;
    }
}

UStart(URobotFlash);
