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
#include <urbi/uobject.hh>

#include <string>

class URobotFlash : private boost::noncopyable, public urbi::UObject {
public:
    URobotFlash(const std::string& hostname = PlayerCc::PLAYER_HOSTNAME,
            uint port = PlayerCc::PLAYER_PORTNUM);
    
    // Funkcje sterujące połączeniem
    bool connect(const std::string& hostname = PlayerCc::PLAYER_HOSTNAME,
            uint port = PlayerCc::PLAYER_PORTNUM);
    bool isConnected() const { return mIsConnected; }
    void disconnect();
    
    // Funkcje sterujące bezpośrednio robotem
    void setSpeed(double xSpeed, double yawSpeed) { setXSpeed(xSpeed); setYawSpeed(yawSpeed); }
    void setXSpeed(double xSpeed) { mXSpeed = xSpeed; }
    void setYawSpeed(double yawSpeed) { mYawSpeed = yawSpeed; }
    void stopRobot() { setSpeed(0.0, 0.0); }
    double getActualXSpeed() const { return isConnected() ? mPosition->GetXSpeed() : 0.0; }
    double getActualYawSpeed() const { return isConnected() ? mPosition->GetYawSpeed() : 0.0; }
    
    // Funkcje sterujące planerem
    void setGoalPose(double goalX, double goalY, double goalAngle);
    bool goToGoalPose(double goalX, double goalY, double goalAngle);
    bool isGoalPoseReached() const;
    double getActualXPos() const { return isConnected() ? mPlanner->GetPx() : 0; }
    double getActualYPos() const { return isConnected() ? mPlanner->GetPy() : 0; }
    double getActualAnglePos() const { return isConnected() ? mPlanner->GetPa() : 0; }
    double getGoalXPos() const { return isConnected() ? mPlanner->GetGx() : 0; }
    double getGoalYPos() const { return isConnected() ? mPlanner->GetGy() : 0; }
    double getGoalAnglePos() const { return isConnected() ? mPlanner->GetPa() : 0; }
    
private:
    boost::scoped_ptr<PlayerCc::PlayerClient> mRobot;
    boost::scoped_ptr<PlayerCc::Position2dProxy> mPosition;
    boost::scoped_ptr<PlayerCc::PlannerProxy> mPlanner;    
    boost::scoped_ptr<boost::thread> mURobotFlashThread;
    
    // Status połączenia
    bool mIsConnected;
    
    // Dane sterujące robotem
    double mXSpeed, mYawSpeed;
};

#endif	/* UROBOTFLASH_H */

