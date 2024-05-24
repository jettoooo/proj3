// Game.cpp

#include "Game.h"
#include "utilities.h"
#include "Level.h"
#include <iostream>
using namespace std;

// Implement these and other Game member functions you may have added.

Game::Game(int goblinSmellDistance) : mLevel(new Level) {}

Game::~Game()
{
    delete mLevel;
}

void Game::play()
{

    Player* player = mLevel->getPlayer();
    if (player == nullptr)
        return;

    mLevel->printRoom();
    displayPlayerStats(player);

    bool inventoryOn = false;
    char input;
    do {        
        if (!inventoryOn)
        {
            printLog();
        }

        if (!player->isAlive() || player->hasGoldenIdol()) //leave loop if player dies
        {
            break;
        }

        input = getCharacter(); // Get user input
        handleInput(input, player, inventoryOn);

        handleMonsterActions();
        clearScreen();
        mLevel->printRoom();
        displayPlayerStats(player);


        handleDeadMonsters();


    } while (input != 'q'); // Exit game if 'q' is pressed

    cout << "Press q to exit game." << endl;
}

void Game::handleInput(char input, Player* player, bool& inventoryOn)
{
    // Handle user input

    GameObject* stairsUnderPlayer = nullptr;

    switch (input) {
    case ARROW_UP:
        // Move player up
        player->move(UP);
        
        break;
    case ARROW_DOWN:
        // Move player down
        player->move(DOWN);
        
        break;
    case ARROW_LEFT:
        // Move player left
        player->move(LEFT);
       
        break;
    case ARROW_RIGHT:
        // Move player right
        player->move(RIGHT);
        
        break;
    case 'c':
        player->cheat();
        break;
    case 'g':
        player->pickUp();
        break;
    case '>':
        stairsUnderPlayer = mLevel->getGameObjectAt(player->getLocation().r(), player->getLocation().c());
        if (stairsUnderPlayer != nullptr)
        {
            mLevel->createNewLevel();
           
        }
        break;
    case 'i':
    case 'r':
    case 'w':
        handleInventory(input, player, inventoryOn);
        return;
    }
    player->heal(); //player has a chance to heal
}

void Game::handleInventory(char input, Player* player, bool& inventoryOn)
{
    clearScreen();
    if (inventoryOn == false) //inventory is not displayed when entering pressing inventory
    {
        inventoryOn = true;
        player->displayInventory();
        if (input == 'r')
        {
            char scrollInput = getCharacter(); //get character if we opened inventory with 'r'
            if (scrollInput - 'a' < player->inventorySize()) //calculate correct input to pass to index
            {
                player->readScroll(scrollInput);
            }
            inventoryOn = false;
        }
        else if (input == 'w') //get character if we opened inventroy with 'w'
        {
            char weaponInput = getCharacter();
            if (weaponInput - 'a' < player->inventorySize())
            {
                player->wield(weaponInput);
            }
            inventoryOn = false;
        }
        else
        {
            getCharacter(); //if input was not 'w' or 'r' we take any key to leave inventory
            inventoryOn = false;
        }
        player->heal();
    }
    else
    {
        inventoryOn = false;
        mLevel->printRoom();
        displayPlayerStats(player);
        player->heal();
    }
}

void Game::handleMonsterActions()
{
    //handle monster action
    for (int i = 0; i < mLevel->getNumMonsters(); i++)
    {
        Monster* monster = mLevel->getMonster(i);
        if (monster != nullptr && monster ->isAlive())
        {
            monster->action();
        }
    }
}

void Game::handleDeadMonsters()
{
    //handle dead monsters
    for (int i = 0; i < mLevel->getNumMonsters(); i++)
    {
        Monster* monster = mLevel->getMonster(i);
        if (monster != nullptr && !monster->isAlive())
        {
            monster->drop();
            delete monster;
            mLevel->removeMonster(i);
            i++;
        }
    }
}


void Game::printLog()
{
    GameLog* gameLog = mLevel->getGameLog();
    gameLog->display();
}

void Game::displayPlayerStats(Player* player)
{
    cout << "Level: " << mLevel->getLevelNum() <<
        ", Hit points: " << player->getHealth() <<
        ", Armor: " << player->getArmor() <<
        ", Strength: " << player->getStrength() <<
        ", Dexterity: " << player->getDexterity() << endl << endl;
}


// You will presumably add to this project other .h/.cpp files for the various
// classes of objects you need to play the game:  player, monsters, weapons, 
// etc.  You might have a separate .h/.cpp pair for each class (e.g., Player.h,
// Bogeyman.h, etc.), or you might put the class declarations for all actors in
// Actor.h, all game objects in GameObject.h, etc.
