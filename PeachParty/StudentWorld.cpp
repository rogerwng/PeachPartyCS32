#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_bank = 0;
    
    //p1 = nullptr;
    //p2 = nullptr;
}

StudentWorld::~StudentWorld() {
    //delete p1;
    //cleanUp();
    
}

// adds actor to vector
void StudentWorld::addActor(Actor* newActor)
{
    m_actors.push_back(newActor);
}

// loads board by, takes string of board file name
void StudentWorld::loadBoard(int board)
{
    ostringstream oss;
    oss << "board0" << board << ".txt";
    
    string board_file = assetPath() + oss.str();
    Board::LoadResult result = bd.loadBoard(board_file);
    
    if (result == Board::load_fail_file_not_found)
        cerr << "Board loaded: "  << board_file << "\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Board not found: " << board_file << "\n";
    else if (result == Board::load_success)
        cerr << "Board loaded: "  << board_file << "\n";
}

// initializes actors after board loaded
void StudentWorld::initActors()
{
    // loop through 16 x 16 board to initialize actors
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            Board::GridEntry ge = bd.getContentsOf(x, y);
            switch(ge)
            {
                case Board::empty:
                    break;
                case Board::boo:
                    m_actors.push_back(new Boo(IID_BOO,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    m_actors.push_back(new BlueCoin(IID_BLUE_COIN_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
                case Board::bowser:
                    m_actors.push_back(new Bowser(IID_BOWSER,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    m_actors.push_back(new BlueCoin(IID_BLUE_COIN_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
                case Board::player:{
                    //PlayerAvatar *temp = new PlayerAvatar(IID_PEACH,x * SPRITE_WIDTH,y * SPRITE_HEIGHT,1,this);
                    p1 = new PlayerAvatar(IID_PEACH,x * SPRITE_WIDTH,y * SPRITE_HEIGHT,1,this);
                    p2 = new PlayerAvatar(IID_YOSHI,x * SPRITE_WIDTH,y * SPRITE_HEIGHT,2,this);
                    m_actors.push_back(new BlueCoin(IID_BLUE_COIN_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;}
                case Board::blue_coin_square:
                    m_actors.push_back(new BlueCoin(IID_BLUE_COIN_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
                case Board::red_coin_square:
                    m_actors.push_back(new RedCoin(IID_RED_COIN_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
                case Board::up_dir_square:
                    m_actors.push_back(new Directional(IID_DIR_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this,90));
                    break;
                case Board::down_dir_square:
                    m_actors.push_back(new Directional(IID_DIR_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this,270));
                    break;
                case Board::left_dir_square:
                    m_actors.push_back(new Directional(IID_DIR_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this,180));
                    break;
                case Board::right_dir_square:
                    m_actors.push_back(new Directional(IID_DIR_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this,0));
                    break;
                case Board::event_square:
                    m_actors.push_back(new Event(IID_EVENT_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
                case Board::bank_square:
                    m_actors.push_back(new Bank(IID_BANK_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
                case Board::star_square:
                    m_actors.push_back(new Star(IID_STAR_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
                    break;
            }
        }
    }
}

PlayerAvatar* StudentWorld::findWinner()
{
    // first check stars
    if (p1->getStars() > p2->getStars())
        return p1;
    else if (p2->getStars() > p1->getStars())
        return p2;
    
    // then coins
    if (p1->getCoins() > p2->getCoins())
        return p1;
    else if (p2->getCoins() > p1->getCoins())
        return p2;
    
    //randomly pick
    if (randInt(1,2) == 1)
        return p1;
    else
        return p2;
}

int StudentWorld::init()
{
    startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    
    loadBoard(getBoardNumber());
    initActors();
    
    return GWSTATUS_CONTINUE_GAME;
}

string StudentWorld::createStatLine()
{
    ostringstream oss;
    
    oss << "P1 Roll: " << p1->getRoll() << " Stars: " << p1->getStars() << " $$: " << p1->getCoins() << " ";
    if (p1->hasVortex())
        oss << "VOR ";
    
    oss << "| Time: " << timeRemaining() << " | Bank: " << m_bank << " | ";
    
    oss << "P2 Roll: " << p2->getRoll() << " Stars: " << p2->getStars() << " $$: " << p2->getCoins() << " ";
    if (p2->hasVortex())
        oss << "VOR ";
    
    return oss.str();
}

int StudentWorld::move()
{
    // check if game is over
    if (timeRemaining() <= 0)
    {
        playSound(SOUND_GAME_FINISHED);
        
        PlayerAvatar* winner = findWinner();
        setFinalScore(winner->getStars(), winner->getCoins());
        if (winner == p1)
            return GWSTATUS_PEACH_WON;
        else if (winner == p2)
            return GWSTATUS_YOSHI_WON;
    }
    
    // ask all actors to do smt
    if (p1->isActive())
        p1->doSomething();
    if (p2->isActive())
        p2->doSomething();
    
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (m_actors[k]->isActive())
            m_actors[k]->doSomething();
    }
    
    // delete dead actors
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (!(m_actors[k]->isActive()))
        {
            delete m_actors.at(k);
            m_actors.erase(m_actors.begin() + k);
        }
    }

    // update status text
    string stats = createStatLine();
    setGameStatText(stats);
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete p1;
    delete p2;
    for (size_t k = 0; k < m_actors.size(); k++)
        delete m_actors[k];
    m_actors.clear();
}

void StudentWorld::doSomething() const
{
    for (size_t i = 0; i < m_actors.size(); i++)
    {
        m_actors[i]->doSomething();
    }
}

void StudentWorld::checkPlayer(bool& p1_check, bool& p2_check, int x, int y) const
{
    p1_check = false;
    p2_check = false;
    
    x /= 16;
    y /= 16;
    
    int xP = p1->getX();
    int yP = p1->getY();
    xP /= 16;
    yP /= 16;
    
    if (x == xP && y == yP)
        p1_check = true;
    
    xP = p2->getX();
    yP = p2->getY();
    xP /= 16;
    yP /= 16;
    
    if (x == xP && y == yP)
        p2_check = true;
}

void StudentWorld::giveCoins(int player, int amt) const
{
    if (player == 1)
        p1->addCoins(amt);
    if (player == 2)
        p2->addCoins(amt);
}

void StudentWorld::changePlayerDirection(int player, int dir)
{
    if (player == 1)
        p1->setWalkingDir(dir);
    if (player == 2)
        p2->setWalkingDir(dir);
}

void StudentWorld::giveStar(int player, int amt) const
{
    if (player == 1)
        p1->addStar(amt);
    if (player == 2)
        p2->addStar(amt);
}

void StudentWorld::addBank(int player)
{
    if (player == 1)
    {
        if (p1->getCoins() < 5)
        {
            m_bank += p1->getCoins();
            p1->addCoins(-1 * p1->getCoins());
        }
        else
        {
            m_bank += 5;
            p1->addCoins(-5);
        }
    }
    
    if (player == 2)
    {
        if (p2->getCoins() < 5)
        {
            m_bank += p2->getCoins();
            p2->addCoins(-1 * p2->getCoins());
        }
        else
        {
            m_bank += 5;
            p2->addCoins(-5);
        }
    }
}

void StudentWorld::clearBank(int player)
{
    if (player == 1)
    {
        giveCoins(1, m_bank);
        m_bank = 0;
    }
    if (player == 2)
    {
        giveCoins(2, m_bank);
        m_bank = 0;
    }
}

void StudentWorld::giveVortex(int player)
{
    if (player == 1)
    {
        if(!(p1->hasVortex()))
        {
            p1->addVortex();
            playSound(SOUND_GIVE_VORTEX);
        }
    }
    
    if (player == 2)
    {
        if(!(p2->hasVortex()))
        {
            p2->addVortex();
            playSound(SOUND_GIVE_VORTEX);
        }
    }
}

void StudentWorld::teleportPlayer(int player)
{
    if (player == 1)
        p1->teleportMe();
    
    if (player == 2)
        p2->teleportMe();
}

void StudentWorld::swapPlayers()
{
    int tempX = p1->getX();
    int tempY = p1->getY();
    
    p1->moveTo(p2->getX(), p2->getY());
    p2->moveTo(tempX, tempY);
    p1->swapPosMov(*p2);
}

void StudentWorld::bowserActivated(int player)
{
    if (player == 1)
    {
        p1->addStar(-1 * p1->getStars());
        p1->addCoins(-1 * p1->getCoins()/2);
    }
    
    if (player == 2)
    {
        p2->addStar(-1 * p2->getStars());
        p2->addCoins(-1 * p2->getCoins()/2);
    }
    
    playSound(SOUND_BOWSER_ACTIVATE);
}

void StudentWorld::bowserDrop(int x, int y)
{
    x /= 16;
    y /= 16;
    size_t i = 0;
    
    for (i = 0; i < m_actors.size(); i++)
    {
        int X = m_actors[i]->getX() / 16;
        int Y = m_actors[i]->getY() / 16;
        
        if (X == x && Y == y && m_actors[i]->droppable())
            break;
    }
    
    m_actors[i]->setDead();
    m_actors.push_back(new Drop(IID_DROPPING_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT,this));
    playSound(SOUND_DROPPING_SQUARE_CREATED);
}

bool StudentWorld::checkOverlapAt(int x, int y)
{
    x /= 16;
    y /= 16;
    
    for (size_t i = 0; i < m_actors.size(); i++)
    {
        int X = m_actors[i]->getX() / 16;
        int Y = m_actors[i]->getY() / 16;
        
        if (X == x && Y == y && m_actors[i]->isImpactable())
            return true;
    }
    
    return false;
}

void StudentWorld::impactAt(int x, int y)
{
    x /= 16;
    y /= 16;
    size_t i = 0;
    
    for (i = 0; i < m_actors.size(); i++)
    {
        int X = m_actors[i]->getX() / 16;
        int Y = m_actors[i]->getY() / 16;
        
        if (X == x && Y == y && m_actors[i]->isImpactable())
            break;
    }
    
    m_actors[i]->impact();
}
