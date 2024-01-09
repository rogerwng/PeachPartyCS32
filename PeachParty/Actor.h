#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// base class Actor
class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world);
    virtual ~Actor() {};
    
    // doSomething specific to each type of actor
    virtual void doSomething() = 0;
    
    virtual bool isImpactable() { return false; }
    virtual void impact() {}
    
    bool getP1Act() { return p1Act; };
    void setP1Act(bool value) { p1Act = value; };
    bool getP2Act() { return p2Act; };
    void setP2Act(bool value) { p2Act = value; };
    void checkAct();
    
    virtual bool droppable() { return true; }
    
    void teleportMe();
    
    // mutators
    void setDead() { m_active = false; };
    void setActive() { m_active = true; };
    
    // accessors
    bool isActive() const { return m_active; }
    StudentWorld* getWorld() const { return m_world; }
private:
    bool m_active;
    StudentWorld* m_world;  // all actors belong to a world
    
    bool p1Act;
    bool p2Act;
};


// base class Avatar: any actors that represents characters like Player, Bowser, Boo
class Avatar : public Actor
{
public:
    Avatar(int imageID, int startX, int startY, StudentWorld* world);
    virtual ~Avatar();
    
    virtual void doSomething() = 0;
    
    // mutators
    void setWalkingDir(int dir);
    void setTicksToMove(int ticks) { m_ticks_to_move = ticks; };
    void setSquaresToMove(int squares) { m_squares_to_move = squares; };
    void setWalking() { m_state = STATE_WALKING; };
    void setWaiting() { m_state = STATE_WAITING; };
    void setState(int state) { m_state = state; }
    
    virtual bool droppable() { return false; }
    
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
    
    bool atFork() const;
    virtual void chooseDir() = 0;
private:
    // helper funcs for availableTurn()
    void availableUpDown();
    void availableRightLeft();
    
    const int STATE_WALKING = 0;
    const int STATE_WAITING = 1;
    
    int m_walkingDir;
    int m_ticks_to_move;
    int m_squares_to_move;
    int m_state;
};


// PlayerAvatar class that represents player 1 and 2
class PlayerAvatar : public Avatar
{
public:
    PlayerAvatar(int imageID, int startX, int startY, int playerNum, StudentWorld* world);
    ~PlayerAvatar();
    
    // accessors
    bool isPlayer1() const { return m_player; }
    bool hasVortex() const { return m_vortex; }
    int getCoins() const { return m_coins; }
    int getStars() const { return m_stars; }
    int getRoll() const { return m_roll; }
    int getTicksWaiting() const { return ticks_waiting; }
    
    bool justLanded() const { return m_justLanded; }
    
    // swap mutators
    void swapCoins(PlayerAvatar& p0);
    void swapStars(PlayerAvatar& p0);
    void swapPosMov(PlayerAvatar& p0);
    
    void addCoins(int amt);
    void addStar(int amt);
    void addVortex() { m_vortex = true; }
    
    virtual void doSomething();
    virtual void chooseDir();
private:
    int m_player;
    bool m_justLanded;
    int m_roll;
    int ticks_waiting;

    // 'inventory'
    int m_coins;
    int m_stars;
    bool m_vortex;
};


// Coin square
class CoinSquare : public Actor
{
public:
    CoinSquare(int imageID, int startX, int startY, StudentWorld* world, int sign);
    virtual ~CoinSquare();
    
    virtual void doSomething();
    
    // has the square been landed on
    bool landed();
private:
    int m_sign;
};


// BlueCoin class (bc classes can't have IID in them??? so can't check if IID == Blue in coinsquare, so new class??
class BlueCoin : public CoinSquare
{
public:
    BlueCoin(int imageID, int startX, int startY, StudentWorld* world);
    ~BlueCoin();
};

// RedCoin class
class RedCoin : public CoinSquare
{
public:
    RedCoin(int imageID, int startX, int startY, StudentWorld* world);
    ~RedCoin();
};

// Directional class
class Directional : public Actor
{
public:
    Directional(int imageID, int startX, int startY, StudentWorld* world, int dir);
    ~Directional();
    
    void doSomething();
private:
    int forceDir;
};

// Vortex class
class Vortex : public Actor
{
public:
    Vortex(int imageID, int startX, int startY, int dir, StudentWorld* world);
    ~Vortex();
    
    void doSomething();
private:
    bool checkPos() const;
    bool checkOverlap() const;
    virtual void impact() {}
    
    int m_dir;
};

// Star Square
class Star : public Actor
{
public:
    Star(int imageID, int startX, int startY, StudentWorld* world);
    ~Star();
    
    void doSomething();
};

// Bank Square
class Bank : public Actor
{
public:
    Bank(int imageID, int startX, int startY, StudentWorld* world);
    ~Bank();
    
    void doSomething();
};

// Event square
class Event : public Actor
{
public:
    Event(int imageID, int startX, int startY, StudentWorld* world);
    ~Event();
    
    void doSomething();
private:
    void teleportPlayer(int player) const;
    void swapPlayers() const;
    void giveVortex(int player) const;
};

// Dropping square
class Drop : public Actor
{
public:
    Drop(int imageID, int startX, int startY, StudentWorld* world);
    ~Drop();
    
    void doSomething();
};

// Baddies base class for boo and bowser
class Baddies : public Avatar
{
public:
    Baddies(int imageID, int startX, int startY, StudentWorld* world);
    ~Baddies();
    
    void chooseDir();
    virtual bool isImpactable() { return true; }
    virtual void impact();
    
    int getPause() const { return m_pauseCount; };
    void resetPause() { m_pauseCount = 180; };
    void addPause(int amt) { m_pauseCount += amt; };
    
    void decrementPauseCounter();
    void moveBaddie();
private:
    int m_pauseCount;
};

// Bowser
class Bowser : public Baddies
{
public:
    Bowser(int imageID, int startX, int startY, StudentWorld* world);
    ~Bowser();
    
    void doSomething();
private:
};

// Boo
class Boo : public Baddies
{
public:
    Boo(int imageID, int startX, int startY, StudentWorld* world);
    ~Boo();
    
    void doSomething();
};
#endif // ACTOR_H_
