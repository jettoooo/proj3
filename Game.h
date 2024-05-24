// Game.h

#ifndef GAME_INCLUDED
#define GAME_INCLUDED

// You may add data members and other member functions to this class.

#include "Level.h"
#include "GameLog.h"


class Game
{
public:
    Game(int goblinSmellDistance);
    ~Game(); //game destructor
    void play(); 
    void printLog(); //prints game log messages
    void displayPlayerStats(Player* player); //prints player stats
    void handleInput(char input, Player* player, bool& inventoryOn); //takes player input
    void handleInventory(char input, Player* player, bool& inventoryOn); //handles all the inventory displays
    void handleMonsterActions(); //iterates all monster actions
    void handleDeadMonsters(); //removes dead monsters from game

private:
    Level* mLevel;
};

#endif // GAME_INCLUDED
