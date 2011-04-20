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
#include <urbi/uobject.hh>

#include <string>

class URobotFlash : private boost::noncopyable, public urbi::UObject {
public:
    URobotFlash(const std::string& hostname = PlayerCc::PLAYER_HOSTNAME,
            uint port = PlayerCc::PLAYER_PORTNUM);
private:
    boost::scoped_ptr<PlayerCc::PlayerClient> mRobot;
    boost::scoped_ptr<PlayerCc::Position2dProxy> mPosition;
    boost::scoped_ptr<PlayerCc::PlannerProxy> mPlanner;
};

#endif	/* UROBOTFLASH_H */

