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

