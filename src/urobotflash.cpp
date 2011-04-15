/* 
 * File:   URobotFlash.cpp
 * Author: aoleksy
 * 
 * Created on 15 kwiecień 2011, 11:09
 */

#include "urobotflash.h"

using namespace PlayerCc;

URobotFlash::URobotFlash(const std::string& hostname, uint port) :
        mRobot(new PlayerClient(hostname, port)),
        mPosition(new Position2dProxy(mRobot.get())),
        mPlanner(new PlannerProxy(mRobot.get())) {

}

URobotFlash::URobotFlash(const URobotFlash& orig) {
}

URobotFlash::~URobotFlash() {
}

