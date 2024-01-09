//
//  PlayerAvatar.cpp
//  PeachParty
//
//  Created by Roger Wang on 2/22/23.
//  Copyright © 2023 UCLA Computer Science Department. All rights reserved.
//

#include "PlayerAvatar.h"

// constructor
PlayerAvatar::PlayerAvatar(int imageID, int startX, int startY, int playerNum, StudentWorld* world) : Avatar(imageID, startX, startY, world)
{
    // initialize data members
    m_player = playerNum;
    m_coins = 0;
    m_stars = 0;
    m_vortex = false;
}

//destructor
PlayerAvatar::~PlayerAvatar()
{
    
}

void PlayerAvatar::swapCoins(PlayerAvatar& p0)
{
    int temp = this->m_coins;
    this->m_coins = p0.m_coins;
    p0.m_coins = temp;
}

void PlayerAvatar::swapStars(PlayerAvatar& p0)
{
    int temp = this->m_stars;
    this->m_stars = p0.m_stars;
    p0.m_stars = temp;
}

void PlayerAvatar::swapPosMov(PlayerAvatar& p0)
{
    // swap walking dir
    int temp = this->getWalkingDir();
    this->setWalkingDir(p0.getWalkingDir());
    p0.setWalkingDir(temp);
    
    // swap ticks to move
    temp = this->getTicksToMove();
    this->setTicksToMove(p0.getTicksToMove());
    p0.setTicksToMove(temp);
    
    // swap states
    temp = this->getState();
    this->setState(p0.getState());
    p0.setState(temp);
    
    // swap sprite direction: uses GraphObject func
    temp = this->getDirection();
    this->setDirection(p0.getDirection());
    p0.setDirection(temp);
}

// doSomething
void PlayerAvatar::doSomething()
{
    if (!this->isWalking())
    {
        int action = this->getWorld()->getAction(m_player);
        
        switch(action) {
            case ACTION_ROLL:
                this->setTicksToMove(randInt(1,10) * 8);
                this->setWalking();
                break;
            default:
                return;
        }
    }
    
    if (this->isWalking())
    {
        if (!canMove(this->getWalkingDir()))
        {
            this->availableTurn();
        }
        
        this->moveForward(2);
        this->setTicksToMove(this->getTicksToMove() - 1);
        if (this->getTicksToMove() == 0)
            this->setWaiting();
    }
}
