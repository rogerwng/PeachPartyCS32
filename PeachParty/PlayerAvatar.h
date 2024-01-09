//
//  PlayerAvatar.hpp
//  PeachParty
//
//  Created by Roger Wang on 2/22/23.
//  Copyright © 2023 UCLA Computer Science Department. All rights reserved.
//
#include "Avatar.h"

#ifndef PlayerAvatar_hpp
#define PlayerAvatar_hpp

class PlayerAvatar : public Avatar
{
public:
    PlayerAvatar(int imageID, int startX, int startY, int playerNum, StudentWorld* world);
    virtual ~PlayerAvatar();
    
    // accessors
    bool isPlayer1() const { return m_player; }
    bool hasVortex() const { return m_vortex; }
    
    // swap mutators
    void swapCoins(PlayerAvatar& p0);
    void swapStars(PlayerAvatar& p0);
    void swapPosMov(PlayerAvatar& p0);
    
    virtual void doSomething();
private:
    int m_player;

    // 'inventory'
    int m_coins;
    int m_stars;
    bool m_vortex;
};

#endif /* PlayerAvatar_hpp */
