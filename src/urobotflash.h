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
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/bind.hpp>
#include <urbi/uobject.hh>

#include <cstring>
#include <string>
#include <list>
#include <algorithm>

class URobotFlash : private boost::noncopyable, public urbi::UObject {
public:
    URobotFlash(const std::string& s);
    ~URobotFlash();
    
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
    
    // Funkcje sterujące lokalizatorem
    void setPose(double x, double y, double yaw);
    
    // Funkcje sterujące laserem
    double getMinDist() const;
    double getMaxDist() const;
    std::vector<double> getDist() const;
    
private:
    typedef std::list<playerc_device_info_t> DeviceInfoList;
    
    boost::scoped_ptr<PlayerCc::PlayerClient> mRobotProxy;
    boost::scoped_ptr<PlayerCc::Position2dProxy> mSpeedControllerProxy;
    boost::scoped_ptr<PlayerCc::PlannerProxy> mPlannerProxy;  
    boost::scoped_ptr<PlayerCc::LocalizeProxy> mLocalizeProxy;
    boost::scoped_ptr<PlayerCc::Position2dProxy> mPositionProxy;
    boost::scoped_ptr<PlayerCc::LaserProxy> mLaserProxy;
    boost::thread mSpeedControlThread;
    boost::mutex mURobotFlashThreadMutex;
    
    void speedControlThread();
    
    enum ControllerType {SpeedController, NavigationController};
    ControllerType mCurrentControllerType;
    void switchController(ControllerType controllerType);
        
    // Status połączenia
    bool mIsConnected;
    
    // Dane sterujące robotem
    double mXSpeed, mYawSpeed;
    
    // Preykat do wyszukiwania odpowiednich urządzen po nazwie
    bool deviceNamePred(const playerc_device_info_t&, const char*, uint16_t deviceType) const;
    DeviceInfoList::const_iterator findDevice(const std::list<playerc_device_info_t>&, const char*, uint16_t deviceType) const;
};

inline bool URobotFlash::isConnected() const {
    return mIsConnected;
}

inline void URobotFlash::setSpeed(double xSpeed, double yawSpeed) {
    setXSpeed(xSpeed);
    setYawSpeed(yawSpeed);
}

inline void URobotFlash::setXSpeed(double xSpeed) {
    if(!isConnected())
        return;
    switchController(SpeedController);
    mXSpeed = xSpeed;
}

inline void URobotFlash::setYawSpeed(double yawSpeed) {
    if(!isConnected())
        return;
    
    switchController(SpeedController);
    mYawSpeed = yawSpeed;
}

inline void URobotFlash::stopRobot() {
    setSpeed(0.0, 0.0);
}

inline double URobotFlash::getActualXSpeed() const {
    return isConnected() ? mSpeedControllerProxy->GetXSpeed() : 0.0;
}

inline double URobotFlash::getActualYawSpeed() const {
    return isConnected() ? mSpeedControllerProxy->GetYawSpeed() : 0.0;
}

inline double URobotFlash::getGoalXPos() const {
    return isConnected() ? mPlannerProxy->GetGoal().px : 0.0;
}

inline double URobotFlash::getGoalYPos() const {
    return isConnected() ? mPlannerProxy->GetGoal().py : 0.0;
}

inline double URobotFlash::getGoalAnglePos() const {
    return isConnected() ? mPlannerProxy->GetGoal().pa : 0.0;
}

inline bool URobotFlash::deviceNamePred(const playerc_device_info_t& deviceInfo, const char* deviceName, uint16_t deviceType) const {
    return strcmp(deviceInfo.drivername, deviceName) == 0 && deviceInfo.addr.interf == deviceType;
}

inline void URobotFlash::setPose(double x, double y, double yaw) {
    if(isConnected()) {
        double pos[3] = {x, y, yaw};
        double cov[3] = {0.5, 0.5, 0.5};
        mLocalizeProxy->SetPose(pos, cov);
    }
}

inline URobotFlash::DeviceInfoList::const_iterator
URobotFlash::findDevice(const std::list<playerc_device_info_t>& deviceList, const char* deviceName, uint16_t deviceType) const {
    return std::find_if(deviceList.begin(), deviceList.end(),
            boost::bind(&URobotFlash::deviceNamePred, this, _1, deviceName, deviceType));
}

#endif	/* UROBOTFLASH_H */

