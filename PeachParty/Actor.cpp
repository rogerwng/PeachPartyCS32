#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//
// IMPLEMENTING ACTOR
//

// constructor
// takes same arguments as GraphObject constructor and uses these to initialize the base GraphObject
Actor::Actor(int imageID, int startX, int startY, StudentWorld* world) : GraphObject(imageID, startX, startY)
{
    // actors are alive when first constructed
    m_active = true;
    m_world = world;
    p1Act = false;
    p2Act = false;
}

void Actor::checkAct()
{
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    // check if player been activated and then left square
    if (!p1_check && getP1Act())
        setP1Act(false);
    if (!p2_check && getP2Act())
        setP2Act(false);
}

void Actor::teleportMe()
{
    int newX = randInt(0,15);
    int newY = randInt(0,15);
    
    Board::GridEntry ge = getWorld()->getBoard()->getContentsOf(newX,newY);
    
    while (ge == Board::empty)
    {
        newX = randInt(0,15);
        newY = randInt(0,15);
        
        ge = getWorld()->getBoard()->getContentsOf(newX,newY);
    }
    
    moveTo(newX * 16, newY * 16);
}

//
// IMPLEMENTING AVATAR
//
Avatar::Avatar(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, world)
{
    m_walkingDir = right;
    m_ticks_to_move = 0;
    m_squares_to_move = 0;
    m_state = STATE_WAITING;
}

Avatar::~Avatar() {}

// checks if the pixel that the avatar is moving to is part of a valid square
bool Avatar::canMove(int dir) const
{
    // coord of pixel that avatar moves to
    int x_new, y_new;
    getPositionInThisDirection(dir, 2, x_new, y_new);
    
    // convert coord to board axis
    x_new /= 16;
    y_new /= 16;
    
    int x = getX();
    int y = getY();
    
    switch (dir)
    {
        case up:
            if (y == VIEW_HEIGHT)
                return false;
            y += 16;
            break;
        case down:
            if (y == 0)
                return false;
            y -= 2;
            break;
        case left:
            if (x == 0)
                return false;
            x -= 2;
            break;
        case right:
            if (x == VIEW_WIDTH)
                return false;
            x += 16;
            break;
    }
    
    x /= SPRITE_WIDTH;
    y /= SPRITE_HEIGHT;
    
    Board::GridEntry ge = getWorld()->getBoard()->getContentsOf(x,y);
    
    if (ge == Board::empty)
        return false;
    
    return true;
}

void Avatar::setWalkingDir(int dir)
{
    dir %= 360;
    m_walkingDir = dir;
    
    if (dir == left)
        setDirection(left);
    else
        setDirection(right);
}

void Avatar::availableUpDown()
{
    if (canMove(up))           // prioritizes up over down
        setWalkingDir(up);
    else if (canMove(down))
        setWalkingDir(down);
}

void Avatar::availableRightLeft()
{
    if (canMove(right))           // prioritizes right over left
        m_walkingDir = right;
    else if (canMove(left))
        m_walkingDir = left;
   //else
     //   if (m_walkingDir == up)
       //     setWalkingDir(down);
        //else
          //  setWalkingDir(up);
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

bool Avatar::atFork() const
{
    if (! (getX() % 16 == 0 && getY() % 16 == 0))
        return false;
    int dirs = 0;
    int possible[4] = {up,down,left,right};
    for (int i = 0; i < 4; i++)
    {
        if (canMove(possible[i]))
            dirs++;
    }
    
    if (dirs > 2)
        return true;
    else
        return false;
}

//
//IMPLEMENTING PLAYER AVATAR
//

// constructor
PlayerAvatar::PlayerAvatar(int imageID, int startX, int startY, int playerNum, StudentWorld* world) : Avatar(imageID, startX, startY, world)
{
    // initialize data members
    m_player = playerNum;
    m_coins = 0;
    m_stars = 0;
    m_vortex = false;
    m_justLanded = true;
    m_roll = 0;
    ticks_waiting = 0;
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

void PlayerAvatar::chooseDir()
{
    int action = getWorld()->getAction(m_player);
    
    
    switch (action)
    {
        case ACTION_UP:
            if (getWalkingDir() != down && canMove(up))
                setWalkingDir(up);
            break;
        case ACTION_DOWN:
            if (getWalkingDir() != up && canMove(down))
                setWalkingDir(down);
            break;
        case ACTION_LEFT:
            if (getWalkingDir() != right && canMove(left))
                setWalkingDir(left);
            break;
        case ACTION_RIGHT:
            if (getWalkingDir() != left && canMove(right))
                setWalkingDir(right);
            break;
        case ACTION_NONE:
            return;
    }
}

// doSomething
void PlayerAvatar::doSomething()
{
    // Is avatar in invalid direction
    if (!canMove(getWalkingDir()))
    {
        availableTurn();
    }
    
    // Player in waiting state
    if (!isWalking())
    {
        int action = this->getWorld()->getAction(m_player);
        
        switch(action) {
            case ACTION_ROLL:
                m_roll = randInt(1,10);
                setTicksToMove(m_roll * 8);
                setWalking();
                break;
            case ACTION_FIRE:
                if (m_vortex)
                {
                    int x,y;
                    getPositionInThisDirection(getWalkingDir(), 16, x, y);
                    Vortex* newVortex = new Vortex(IID_VORTEX,x,y,getWalkingDir(),getWorld());
                    getWorld()->addActor(newVortex);
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_vortex = false;
                }
                break;
        }
        ticks_waiting++;
        if (m_justLanded)
            m_justLanded = false;
    }
    
    // Player in walking state
    if (isWalking())
    {
        // check if avatar at a fork
        if (atFork())
        {
            chooseDir();
        }
        
        if (!canMove(getWalkingDir()))
        {
            availableTurn();
        }
        
        int x_new, y_new;
        getPositionInThisDirection(getWalkingDir(), 2, x_new, y_new);
        moveTo(x_new, y_new);
        setTicksToMove(getTicksToMove() - 1);
        if (getTicksToMove() == 0)
        {
            setWaiting();
            ticks_waiting = 0;
            m_justLanded = true;
            m_roll = 0;
        }
    }
}

void PlayerAvatar::addCoins(int amt)
{
    m_coins += amt;
    
    if (m_coins < 0)
        m_coins = 0;
}

void PlayerAvatar::addStar(int amt)
{
    m_stars += amt;
    
    if (m_stars < 0)
        m_stars = 0;
}

//
// IMPLEMENTING COIN SQUARE
//

CoinSquare::CoinSquare(int imageID, int startX, int startY, StudentWorld* world, int sign) : Actor(imageID, startX, startY, world)
{
    // whether or not Coin square is blue or red is passed to CoinSquare when it is constructed
    m_sign = sign;
}

CoinSquare::~CoinSquare()
{
    
}

void CoinSquare::doSomething()
{
    if (!this->isActive())
    {
        return;
    }
    
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    if (p1_check && !getWorld()->isP1Walk() && getWorld()->P1TickZero())
    {
        getWorld()->giveCoins(1, m_sign * 3);
        if (m_sign > 0)
            getWorld()->playSound(SOUND_GIVE_COIN);
        else
            getWorld()->playSound(SOUND_TAKE_COIN);
    }
    if (p2_check && !getWorld()->isP2Walk() && getWorld()->P2TickZero())
    {
        getWorld()->giveCoins(2, m_sign * 3);
        if (m_sign > 0)
            getWorld()->playSound(SOUND_GIVE_COIN);
        else
            getWorld()->playSound(SOUND_TAKE_COIN);
    }
}

//
// IMPLEMENTING BLUE COIN
//

BlueCoin::BlueCoin(int imageID, int startX, int startY, StudentWorld* world) : CoinSquare(imageID, startX, startY, world, 1)
{}

BlueCoin::~BlueCoin() {}

//
// IMPLEMENTING RED COIN
//

RedCoin::RedCoin(int imageID, int startX, int startY, StudentWorld* world) : CoinSquare(imageID, startX, startY, world, -1)
{}

RedCoin::~RedCoin() {}

//
// IMPLEMENTING DIRECTIONALSQUARE
//

Directional::Directional(int imageID, int startX, int startY, StudentWorld* world, int dir) : Actor(imageID, startX, startY, world), forceDir(dir)
{
    setDirection(forceDir);
}

Directional::~Directional() {}

void Directional::doSomething()
{
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    checkAct();
    
    if (p1_check && !getP1Act())
    {
        getWorld()->changePlayerDirection(1, forceDir);
        setP1Act(true);
    }
    if (p2_check && !getP2Act())
    {
        getWorld()->changePlayerDirection(2, forceDir);
        setP2Act(true);
    }
}

//
// IMPLEMENTING VORTEX
//

Vortex::Vortex(int imageID, int startX, int startY, int dir, StudentWorld* world) : Actor(imageID, startX, startY, world), m_dir(dir)
{
    
}

Vortex::~Vortex() {}

bool Vortex::checkPos() const
{
    if (getX() < 0 || getX() >= VIEW_WIDTH)
        return false;
    if (getY() < 0 || getY() >= VIEW_HEIGHT)
        return false;
    
    return true;
}

bool Vortex::checkOverlap() const
{
    if (getWorld()->checkOverlapAt(getX(), getY()))
        return true;
    
    return false;
}

void Vortex::doSomething()
{
    if(!isActive())
    {
        return;
    }
    
    int x_new, y_new;
    getPositionInThisDirection(m_dir, 2, x_new, y_new);
    moveTo(x_new, y_new);
    
    // check if out of bounds
    if (!checkPos())
        setDead();
    
    if (checkOverlap())
    {
        getWorld()->impactAt(getX(), getY());
        setDead();
    }
}


//
// IMPLEMENTING STAR
//

Star::Star(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, world)
{}

Star::~Star() {}

void Star::doSomething()
{
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    checkAct();
    
    if (p1_check && !getWorld()->isP1Walk() && !getP1Act())
    {
        getWorld()->giveCoins(1, -20);
        getWorld()->giveStar(1,1);
        getWorld()->playSound(SOUND_GIVE_STAR);
        setP1Act(true);
    }
 
    if (p2_check && !getWorld()->isP2Walk() && !getP2Act())
    {
        getWorld()->giveCoins(2, -20);
        getWorld()->giveStar(2,1);
        getWorld()->playSound(SOUND_GIVE_STAR);
        setP2Act(true);
    }
}

//
// IMPLEMENTING BANK
//

Bank::Bank(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, world)
{}

Bank::~Bank() {}

void Bank::doSomething()
{
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    checkAct();
    
    if (p1_check && getWorld()->isP1Walk() && !getP1Act())
    {
        if (getWorld()->P1TickZero())
        {
            getWorld()->clearBank(1);
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
            setP1Act(true);
        }
        else
        {
            getWorld()->addBank(1);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            setP1Act(true);
        }
    }
 
    if (p2_check && getWorld()->isP2Walk() && !getP2Act())
    {
        if (getWorld()->P2TickZero())
        {
            getWorld()->clearBank(2);
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
            setP2Act(true);
        }
        else
        {
            getWorld()->addBank(2);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            setP2Act(true);
        }
    }
}

//
// IMPLEMENTING EVENT
//

Event::Event(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, world)
{}

Event::~Event() {}

void Event::teleportPlayer(int player) const
{
    if (player == 1)
    {
        getWorld()->teleportPlayer(1);
        getWorld()->playSound(SOUND_PLAYER_TELEPORT);
    }
    
    if (player == 2)
    {
        getWorld()->teleportPlayer(2);
        getWorld()->playSound(SOUND_PLAYER_TELEPORT);
    }
}

void Event::swapPlayers() const
{
    getWorld()->swapPlayers();
    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
}

void Event::giveVortex(int player) const
{
    if (player == 1)
    {
        getWorld()->giveVortex(1);
    }
    
    if (player == 2)
    {
        getWorld()->giveVortex(2);
    }
}

void Event::doSomething()
{
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    checkAct();
    
    int rand = randInt(1,3);
    if (p1_check && !getWorld()->isP1Walk( )&& !getP1Act())
    {
        switch (rand)
        {
            case 1:
                teleportPlayer(1);
                break;
            case 2:
                swapPlayers();
                setP1Act(true);
                break;
            case 3:
                giveVortex(1);
                break;
        }
        setP1Act(true);
    }
 
    if (p2_check && !getWorld()->isP2Walk() && !getP2Act())
    {
        switch (rand)
        {
            case 1:
                teleportPlayer(2);
                break;
            case 2:
                swapPlayers();
                setP1Act(true);
                break;
            case 3:
                giveVortex(2);
                break;
        }
        
        setP2Act(true);
    }
}


//
// IMPLEMENTING DROP
//

Drop::Drop(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, world)
{}

Drop::~Drop() {}

void Drop::doSomething()
{
    int rand = randInt(1, 2);
    
    bool p1_check = false;
    bool p2_check = false;
    
    getWorld()->checkPlayer(p1_check, p2_check, getX(), getY());
    
    checkAct();
    
    if (p1_check && !getWorld()->isP1Walk() && !getP1Act())
    {
        switch (rand)
        {
            case 1:
                getWorld()->giveCoins(1, -10);
                break;
            case 2:
                getWorld()->giveStar(1, -1);
                break;
        }
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        setP1Act(true);
    }
    if (p2_check && !getWorld()->isP2Walk() && !getP2Act())
    {
        switch (rand)
        {
            case 1:
                getWorld()->giveCoins(2, -10);
                break;
            case 2:
                getWorld()->giveStar(2, -1);
                break;
        }
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        setP2Act(true);
    }
}


//
// IMPLEMENTING BADDIES
//

Baddies::Baddies(int imageID, int startX, int startY, StudentWorld* world) : Avatar(imageID, startX, startY, world), m_pauseCount(180) {}

Baddies::~Baddies() {};

void Baddies::chooseDir()
{
    int directions[4] = {up,right,down,left};
    int dir;
    
    bool done = false;
    while (!done)
    {
        dir = directions[randInt(0, 3)];
        switch (dir)
        {
            case up:
                if (getWalkingDir() != down && canMove(up))
                {
                    setWalkingDir(up);
                    done = true;
                }
                break;
            case down:
                if (getWalkingDir() != up && canMove(down))
                {
                    setWalkingDir(down);
                    done = true;
                }
                break;
            case left:
                if (getWalkingDir() != right && canMove(left))
                {
                    setWalkingDir(left);
                    done = true;
                }
                break;
            case right:
                if (getWalkingDir() != left && canMove(right))
                {
                    setWalkingDir(right);
                    done = true;
                }
                break;
        }
    }
}

void Baddies::decrementPauseCounter()
{
    addPause(-1);
    if (getPause() == 0)
    {
        setTicksToMove(randInt(1,10) * 8);
        
        int directions[4] = {up,down,left,right};
        int rand = randInt(0, 3);
        
        while (!canMove(directions[rand]))
        {
            rand = randInt(0,3);
        }
        
        setWalkingDir(directions[rand]);
        setWalking();
        
        setP1Act(false);
        setP2Act(false);
    }
}

void Baddies::moveBaddie()
{
    int x_new, y_new;
    getPositionInThisDirection(getWalkingDir(), 2, x_new, y_new);
    moveTo(x_new, y_new);
    setTicksToMove(getTicksToMove() - 1);
}

void Baddies::impact()
{
    teleportMe();
    setWalkingDir(right);
    setWaiting();
    m_pauseCount = 180;
}

//
// IMPLEMENTING BOWSER
//

Bowser::Bowser(int imageID, int startX, int startY, StudentWorld* world) : Baddies(imageID, startX, startY, world)
{}

Bowser::~Bowser() {}

void Bowser::doSomething()
{
    if (!isWalking())
    {
        // check if on top of player
        bool p1, p2;
        getWorld()->checkPlayer(p1, p2, getX(), getY());
        
        // check if player been activated and then left square
        if (!p1 && getP1Act())
            setP1Act(false);
        if (!p2 && getP2Act())
            setP2Act(false);
        
        // bowser activate
        if (p1 && !(getWorld()->isP1Walk()) && !getP1Act())
        {
            int rand = randInt(1,2);
            if (rand == 1)
                getWorld()->bowserActivated(1);
            setP1Act(true);
        }
        
        if (p2 && !(getWorld()->isP2Walk()) && !getP2Act())
        {
            int rand = randInt(1,2);
            if (rand == 1)
                getWorld()->bowserActivated(2);
            setP2Act(true);
        }
        
        // decrement pause couter
        decrementPauseCounter();
    }
    
    if (isWalking())
    {
        if (atFork())
            chooseDir();
        
        if (!canMove(getWalkingDir()))
            availableTurn();
        
        
        moveBaddie();
        if (getTicksToMove() == 0)
        {
            setWaiting();
            resetPause();
            
            if (randInt(1, 4) == 1)
                getWorld()->bowserDrop(getX(), getY());
        }
    }
}


//
// IMPLEMENTING BOO
//

Boo::Boo(int imageID, int startX, int startY, StudentWorld* world) : Baddies(imageID, startX, startY, world) {}

Boo::~Boo() {}

void Boo::doSomething()
{
    if (!isWalking())
    {
        // check if on top of player
        bool p1, p2;
        getWorld()->checkPlayer(p1, p2, getX(), getY());
        
        // check if player been activated and then left square
        if (!p1 && getP1Act())
            setP1Act(false);
        if (!p2 && getP2Act())
            setP2Act(false);
        
        int rand = randInt(1, 2);
        if (p1 && !getP1Act())
        {
            setP1Act(true);
            if (rand == 1)
                getWorld()->swapCoins();
            else
                getWorld()->swapStars();
            
            getWorld()->playSound(SOUND_BOO_ACTIVATE);
        }
        if (p2 && !getP2Act())
        {
            setP2Act(true);
            if (rand == 1)
                getWorld()->swapCoins();
            else
                getWorld()->swapStars();
            
            getWorld()->playSound(SOUND_BOO_ACTIVATE);
        }
        
        //decrement pause counter
        decrementPauseCounter();
    }
    
    if(isWalking())
    {
        if (atFork())
            chooseDir();
        
        if (!canMove(getWalkingDir()))
            availableTurn();
        
        
        moveBaddie();
        if (getTicksToMove() == 0)
        {
            setWaiting();
            resetPause();
        }
    }
}
