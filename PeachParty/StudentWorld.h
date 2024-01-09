#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>


class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
    ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
    
// accessors
    Board* getBoard() { return &bd; }
    
// mutators
    void addActor(Actor* newActor);

    // call doSomething for each actor
    void doSomething() const;
    // check if player is on top of given square
    void checkPlayer(bool& p1_check, bool& p2_check, int x, int y) const;
    void changePlayerDirection(int player, int dir);
    bool P1Landed() const { return p1->justLanded(); }
    bool P2Landed() const { return p2->justLanded(); }
    
    bool isP1Walk() const { return p1->isWalking(); }
    bool isP2Walk() const { return p2->isWalking(); }
    bool P1TickZero() const
    {
        int ticks = p1->getTicksWaiting();
        if (ticks == 0)
            return true;
        return false;
    }
    bool P2TickZero() const
    {
        int ticks = p2->getTicksWaiting();
        if (ticks == 0)
            return true;
        return false;
    }
    
    void giveCoins(int player, int amt) const;
    void giveStar(int player, int amt) const;
    void giveVortex(int player);
    void swapStars() { p1->swapCoins(*p2); };
    void swapCoins() { p1->swapCoins(*p2); };
    
    void bowserActivated(int player);
    void bowserDrop(int x, int y);
    
    void teleportPlayer(int player);
    void swapPlayers();
    
    bool checkOverlapAt(int x, int y);
    void impactAt(int x, int y);
    
    void addBank(int player);
    void clearBank(int player);
private:
    // loads board, returns 0 for loaded board, 1 for board not found, 2 for bad board
    void loadBoard(int board);
    // initializes actors after board loaded
    void initActors();
    
    PlayerAvatar* findWinner();
    std::string createStatLine();
    
    // vector data structure holds all non-player actors in the world
    std::vector<Actor*> m_actors;
    // p1 -> peach
    PlayerAvatar* p1;
    // p2 -> yoshi
    PlayerAvatar* p2;
    
    Board bd;
    int m_bank;
};

#endif // STUDENTWORLD_H_
