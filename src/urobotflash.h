/* 
 * File:   URobotFlash.h
 * Author: aoleksy
 *
 * Created on 15 kwiecień 2011, 11:09
 */

#ifndef UROBOTFLASH_H
#define	UROBOTFLASH_H

#include <libplayerc++/playerc++.h>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <urbi/uobject.hh>

#include <cstring>
#include <string>
#include <list>
#include <algorithm>

class URobotFlash : private boost::noncopyable, public urbi::UObject {
public:
    URobotFlash(const std::string& s);
    
    // Funkcje sterujące połączeniem
    bool connect(const std::string& hostname = PlayerCc::PLAYER_HOSTNAME,
            uint port = PlayerCc::PLAYER_PORTNUM);
    bool isConnected() const;
    void disconnect();
    
    // Funkcje sterujące bezpośrednio robotem
    void setSpeed(double xSpeed, double yawSpeed);
    void setXSpeed(double xSpeed);
    void setYawSpeed(double yawSpeed);
    void stopRobot();
    double getActualXSpeed() const;
    double getActualYawSpeed() const;
    
    // Funkcje sterujące planerem
    void setGoalPose(double goalX, double goalY, double goalAngle);
    bool goToGoalPose(double goalX, double goalY, double goalAngle);
    bool isGoalPoseReached() const;
    double getActualXPos() const;
    double getActualYPos() const;
    double getActualAnglePos() const;
    double getGoalXPos() const;
    double getGoalYPos() const;
    double getGoalAnglePos() const;
    
private:
    typedef std::list<playerc_device_info_t> DeviceInfoList;
    
    boost::scoped_ptr<PlayerCc::PlayerClient> mRobot;
    boost::scoped_ptr<PlayerCc::Position2dProxy> mPosition;
    boost::scoped_ptr<PlayerCc::PlannerProxy> mPlanner;    
    boost::scoped_ptr<boost::thread> mURobotFlashThread;
    
    // Status połączenia
    bool mIsConnected;
    
    // Dane sterujące robotem
    double mXSpeed, mYawSpeed;
    
    // Preykat do wyszukiwania odpowiednich urządzen po nazwie
    bool deviceNamePred(const playerc_device_info_t&, const char*) const;
    DeviceInfoList::const_iterator findDevice(const std::list<playerc_device_info_t>&, const char*) const;
};

inline bool URobotFlash::isConnected() const {
    return mIsConnected;
}

inline void URobotFlash::setSpeed(double xSpeed, double yawSpeed) {
    setXSpeed(xSpeed);
    setYawSpeed(yawSpeed);
}

inline void URobotFlash::setXSpeed(double xSpeed) {
    mXSpeed = xSpeed;
}

inline void URobotFlash::setYawSpeed(double yawSpeed) {
    mYawSpeed = yawSpeed;
}

inline void URobotFlash::stopRobot() {
    setSpeed(0.0, 0.0);
}

inline double URobotFlash::getActualXSpeed() const {
    return isConnected() ? mPosition->GetXSpeed() : 0.0;
}

inline double URobotFlash::getActualYawSpeed() const {
    return isConnected() ? mPosition->GetYawSpeed() : 0.0;
}

inline bool URobotFlash::isGoalPoseReached() const {
    return isConnected() ? mPlanner->GetPathDone() : false;
}

inline double URobotFlash::getActualXPos() const {
    return isConnected() ? mPlanner->GetPx() : 0;
}

inline double URobotFlash::getActualYPos() const {
    return isConnected() ? mPlanner->GetPy() : 0;
}

inline double URobotFlash::getActualAnglePos() const {
    return isConnected() ? mPlanner->GetPa() : 0;
}

inline double URobotFlash::getGoalXPos() const {
    return isConnected() ? mPlanner->GetGx() : 0;
}

inline double URobotFlash::getGoalYPos() const {
    return isConnected() ? mPlanner->GetGy() : 0;
}

inline double URobotFlash::getGoalAnglePos() const {
    return isConnected() ? mPlanner->GetPa() : 0;
}

inline bool URobotFlash::deviceNamePred(const playerc_device_info_t& deviceInfo, const char* deviceName) const {
    return strcmp(deviceInfo.drivername, deviceName) == 0;
}

inline URobotFlash::DeviceInfoList::const_iterator
URobotFlash::findDevice(const std::list<playerc_device_info_t>& deviceList, const char* deviceName) const {
    return std::find_if(deviceList.begin(), deviceList.end(),
            boost::bind(&URobotFlash::deviceNamePred, this, _1, deviceName));
}

#endif	/* UROBOTFLASH_H */

