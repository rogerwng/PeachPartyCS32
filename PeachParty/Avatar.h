//
//  Avatar.hpp
//  PeachParty
//
//  Created by Roger Wang on 2/22/23.
//  Copyright © 2023 UCLA Computer Science Department. All rights reserved.
//

#ifndef Avatar_hpp
#define Avatar_hpp

#include "Actor.h"

class Avatar : public Actor
{
public:
    Avatar(int imageID, int startX, int startY, StudentWorld* world);
    virtual ~Avatar();
    
    virtual void doSomething() = 0;
    
    // mutators
    void setWalkingDir(int dir) { m_walkingDir = dir; };
    void setTicksToMove(int ticks) { m_ticks_to_move = ticks; };
    void setSquaresToMove(int squares) { m_squares_to_move = squares; };
    void setWalking() { m_state = STATE_WALKING; };
    void setWaiting() { m_state = STATE_WAITING; };
    void setState(int state) { m_state = state; }
    
    // accessors
    int getWalkingDir() const { return m_walkingDir; };
    int getTicksToMove() const { return m_ticks_to_move; };
    int getSquaresToMove() const { return m_squares_to_move; };
    int isWalking() const { return m_state == STATE_WALKING; };  // !isWalking() means state is waiting
    int getState() const { return m_state; };
    
    // helper func
    // checks if player can move forward, i.e. if there is valid square in front of player
    bool canMove(int dir) const;
    // turns player into best available direction
    void availableTurn();
    void availableUpDown();
    void availableRightLeft();
private:
    const int STATE_WALKING = 0;
    const int STATE_WAITING = 1;
    
    int m_walkingDir;
    int m_ticks_to_move;
    int m_squares_to_move;
    int m_state;
};

#endif /* Avatar_hpp */
