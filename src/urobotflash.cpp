/* 
 * File:   URobotFlash.cpp
 * Author: aoleksy
 * 
 * Created on 15 kwiecień 2011, 11:09
 */

#include "urobotflash.h"
#include <player-2.0/libplayerc++/playerclient.h>

using namespace PlayerCc;
using namespace std;
using namespace urbi;

URobotFlash::URobotFlash(const std::string& hostname, uint port) :
        mRobot(NULL),
        mPosition(NULL),
        mPlanner(NULL),
        mURobotFlashThread(NULL),
        mIsConnected(false),
        mXSpeed(0.0), mYawSpeed(0.0)
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
    
}

void URobotFlash::disconnect() {
    
}
