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

URobotFlash::URobotFlash(const std::string& hostname, uint port) :
        mRobot(NULL),
        mPosition(NULL),
        mPlanner(NULL),
        mURobotFlashThread(NULL),
        mIsConnected(false),
        mXSpeed(0.0), mYawSpeed(0.0)
{}
