//
//  Avatar.cpp
//  PeachParty
//
//  Created by Roger Wang on 2/22/23.
//  Copyright © 2023 UCLA Computer Science Department. All rights reserved.
//

#include "Avatar.h"

Avatar::Avatar(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, world)
{
    m_walkingDir = right;
    m_ticks_to_move = 0;
    m_squares_to_move = 0;
    m_state = STATE_WAITING;
}

Avatar::~Avatar() {}

bool Avatar::canMove(int dir) const
{
    return false;
}

void Avatar::availableUpDown()
{
    if (canMove(up))           // prioritizes up over down
        m_walkingDir = up;
    else if (canMove(down))
        m_walkingDir = down;
    else
        if (m_walkingDir == right)
            m_walkingDir = left;
        else
            m_walkingDir = right;
}

void Avatar::availableRightLeft()
{
    if (canMove(right))           // prioritizes right over left
        m_walkingDir = right;
    else if (canMove(left))
        m_walkingDir = left;
    else
        if (m_walkingDir == up)
            m_walkingDir = down;
        else
            m_walkingDir = up;
}

void Avatar::availableTurn()
{
    switch(m_walkingDir)
    {
        case right:
            availableUpDown();
            break;
        case left:
            availableUpDown();
            break;
        case up:
            availableRightLeft();
            break;
        case down:
            availableRightLeft();
            break;
    }
}
