//
//  Square.hpp
//  PeachParty
//
//  Created by Roger Wang on 2/22/23.
//  Copyright © 2023 UCLA Computer Science Department. All rights reserved.
//

#ifndef Square_hpp
#define Square_hpp

#include "Actor.h"

class Square : public Actor
{
public:
    Square(int imageID, int startX, int startY, StudentWorld* world);
    virtual ~Square();
    
    virtual void doSomething() = 0;
    
    // has the square been landed on
    bool landed();
private:
};

#endif /* Square_hpp */
