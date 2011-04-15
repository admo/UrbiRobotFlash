/* 
 * File:   URobotFlash.cpp
 * Author: aoleksy
 * 
 * Created on 15 kwiecień 2011, 11:09
 */

#include "urobotflash.h"
#include <unistd.h>
#include <player-2.0/libplayerc++/playerclient.h>
#include <list>

using namespace PlayerCc;
using namespace std;

URobotFlash::URobotFlash(const std::string& hostname, uint port) :
        mRobot(new PlayerClient(hostname, port)),
        mPosition(new Position2dProxy(mRobot.get()))
        //mPlanner(new PlannerProxy(mRobot.get())) {
{
    mPosition->GoTo(10, 0, 0);
    usleep(5000000);
}

URobotFlash::~URobotFlash() {
}

