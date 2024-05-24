#include <iostream>
#include "Level.h"

using namespace std;

struct Room
{
    int mTop;
    int mLeft;
    int mWidth;
    int mHeight;
};

Level::~Level()
{
    delete mPlayer;


    delete rooms;

    for (vector<Monster*>::iterator it = mMonsters.begin();  //iterate through mMonsters vector
        it != mMonsters.end(); it++)
    {
        delete *it; //call delete on each remaining monster in vector
    }
    for (vector<GameObject*>::iterator it = mObjects.begin();
        it != mObjects.end(); it++)
    {
        delete* it; //call delete on each remaining gameobject in vector
    }

    delete mLog;
}



bool Level::doesOverlap(int x, int y, int length, int width)
{
    for (int i = y; i < y + length; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            if (gameRoom[i][j] == ' ') //if the room encounters an empty 
                return true;           //space that means there will be overlap
        }
    }
    return false;
}

void Level::generateRoom()
{
    Room newRoom;

    //do while loop to check if the room is going to overlap
    do
    {
        newRoom.mTop = randInt(1, height - 6 - 1);
        newRoom.mLeft = randInt(1, width - 10 - 1);
        newRoom.mWidth = randInt(8, 10);
        newRoom.mHeight = randInt(3, 6);

    } while (doesOverlap(newRoom.mLeft, newRoom.mTop, newRoom.mHeight, newRoom.mWidth));

    for (int i = newRoom.mTop; i < newRoom.mTop + newRoom.mHeight; i++)
    {
        for (int j = newRoom.mLeft; j < newRoom.mLeft + newRoom.mWidth; j++)
        {
            gameRoom[i][j] = ' ';
        }
    }
    rooms->push_back(newRoom); //add the newRoom to the rooms vector
}


void Level::generateCorridor(int room1Index, int room2Index)
{
    Room room1 = (*rooms)[room1Index];
    Room room2 = (*rooms)[room2Index];

    int x1 = room1.mLeft + room1.mWidth / 2; //x coord of center of room1
    int y1 = room1.mTop + room1.mHeight / 2; //y coord of center of room1

    int x2 = room2.mLeft + room2.mWidth / 2; //x coord of center of room2
    int y2 = room2.mTop + room2.mHeight / 2; //y coord of center of room2

    // Draw a horizontal line from room1 to room2
    while (x1 != x2)
    {
        if (x1 < x2)
            gameRoom[y1][x1++] = ' ';
        else if (x1 > x2)
            gameRoom[y1][x1--] = ' ';
    }

    // Draw a vertical line from the end of the horizontal line to room2
    while (y1 != y2)
    {
        if (y1 < y2)
            gameRoom[y1++][x1] = ' ';
        else if (y1 > y2)
            gameRoom[y1--][x1] = ' ';
    }
}


void Level::generateLevel()
{
    for (int i = 1; i < randInt(4, 6); i++)
    {
        generateRoom();
    }

    Coord newLocation = generateValidCoord();

    mPlayer->setLocation(newLocation);


    for (int i = 0; i < rooms->size() - 1; i++)
    {
        generateCorridor(i, i + 1);
    }

    //spawn monsters based on what level of the temple we are on
    for (int j = 0; j < randInt(2, 5 * (levelNum + 1) + 1); j++)
    {
        addMonster();
    }
    
    for (int j = 0; j < randInt(2, 3); j++)
    {
        addObject();
    }

    if (levelNum == 4) // golden idol spawns on the last level
    {
        addGoldenIdol();
    }

    addStairs();

}


void Level::printRoom() const //print the game room onto display
{
    // Create a local copy of gameRoom
    vector<vector<char>> localGameRoom(height, vector<char>(width));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            localGameRoom[i][j] = gameRoom[i][j];
        }
    }

    // Update the player's position on the local copy
    if (mPlayer != nullptr)
    {
        char& gridChar = localGameRoom[mPlayer->getLocation().r()][mPlayer->getLocation().c()];
        if (gridChar == ' ')
            gridChar = '@';
        else
            gridChar = '*';
    }

    //print monsters
    for (int k = 0; k < mMonsters.size(); k++)
    {
        Monster* mp = mMonsters[k];
        char& gridChar = localGameRoom[mp->getLocation().r()][mp->getLocation().c()];

        if (gridChar == ' ')
        {
            gridChar = mp->actorType();
        }
    }

    //print game objects

    for (int k = 0; k < mObjects.size(); k++)
    {
        GameObject* op = mObjects[k];
        char& gridChar = localGameRoom[op->getLocation().r()][op->getLocation().c()];

        if (gridChar == ' ')
        {
            gridChar = op->objectType();
        }
    }


    // Print the local copy
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cout << localGameRoom[i][j];
        }
        cout << '\n';
    }
}

bool Level::determineNewPosition(int& r, int& c, int dir) const
{
    switch (dir)
    {
    case UP:     if (r <= 1)      return false; else r--; break;
    case DOWN:   if (r >= height) return false; else r++; break;
    case LEFT:   if (c <= 1)      return false; else c--; break;
    case RIGHT:  if (c >= width)  return false; else c++; break;
    default:     return false;
    }
    return true;
}

bool Level::isValidPosition(int r, int c) const //returns true if the position is empty
{
    if (gameRoom[r][c] != ' ')
    {
        return false;
    }

    for (int i = 0; i < mMonsters.size(); i++)
    {
        if (mMonsters[i]->getLocation().r() == r && mMonsters[i]->getLocation().c() == c)
        {
            return false;
        }
    }

    if (mPlayer->getLocation().r() == r && mPlayer->getLocation().c() == c)
    {
        return false;
    }


    return true;
}


bool Level::isMonsterPosition(int r, int c) const //returns true if a monster is at this position
{
    for (int i = 0; i < mMonsters.size(); i++)
    {
        if (mMonsters[i]->getLocation().r() == r && mMonsters[i]->getLocation().c() == c)
        {
            return true;
        }
    }
    return false;
}

bool Level::isPlayerPosition(int r, int c) const
{
    if (mPlayer->getLocation().r() == r && mPlayer->getLocation().c() == c)
    {
        return true;
    }
    return false;
}


void Level::addPlayer(Player* player)
{
    mPlayer = player;
}

void Level::addMonster()
{
    Coord location = generateValidCoord();

    //create the new monster at chosen location
    int randomNumber = 0;
    if (levelNum < 2) //levels 0 and 1
    {
        randomNumber = randInt(2, 2);
    }
    else if (levelNum == 2) //level 2
    {
        randomNumber = randInt(2, 3); 
    }
    else //include dragon in spawn chances
    {
        randomNumber = randInt(2, 4); //levels 3 and 4
    }


    Monster* newMonster;

    switch (randomNumber)
    {
    case 1: //goblin
        newMonster = new Goblin(this);
        newMonster->setLocation(location);
        mMonsters.push_back(newMonster);
        numMonsters++;
        break;
    case 3: //bogeyman
        newMonster = new Bogeyman(this);
        newMonster->setLocation(location);
        mMonsters.push_back(newMonster);
        numMonsters++;
        break;
    case 2: //snakewoman
        newMonster = new Snakewoman(this);
        newMonster->setLocation(location);
        mMonsters.push_back(newMonster);
        numMonsters++;
        break;
    case 4: //dragon
        newMonster = new Dragon(this);
        newMonster->setLocation(location);
        mMonsters.push_back(newMonster);
        numMonsters++;
        break;
    }
}
void Level::addStairs()
{
    Coord location = generateValidCoord();
    GameObject* newStairs = new Stairs(this);
    newStairs->setLocation(location);
    mObjects.push_back(newStairs);
    numObjects++;
}
void Level::addObject()
{
    Coord location = generateValidCoord();

    GameObject* newObject;

    int randomNumber = randInt(1, 8);
    switch (randomNumber)
    {
    case 1: //short sword
        newObject = new ShortSword(this);
        break;
    case 2: //long sword
        newObject = new LongSword(this);
        break;
    case 3: //mace
        newObject = new Mace(this);
        break;
    case 4: //teleport scroll
        newObject = new TeleportScroll(this);
        break;
    case 5: //armor scroll
        newObject = new ArmorScroll(this);
        break;
    case 6: //strength scroll
        newObject = new StrengthScroll(this);
        break;
    case 7: //health scroll
        newObject = new HealthScroll(this);
        break;
    case 8: //dex scroll
        newObject = new DexterityScroll(this);
        break;
    default: //placeholder
        newObject = new ShortSword(this);
        break;
    }
    
    newObject->setLocation(location);
    mObjects.push_back(newObject);
    numObjects++;
}

void Level::addGoldenIdol()
{
    //generate random spawn point for golden idol
    Coord location = generateValidCoord();

    //create new golden idol object
    GoldenIdol* goldenIdol = new GoldenIdol(this);
    goldenIdol->setLocation(location);

    //push golden idol object into level's object vector
    mObjects.push_back(goldenIdol);

    //increment number of objects
    numObjects++;

}


void Level::createNewLevel()
{
    //save pointer to player
    Player* player = getPlayer();

    // increment the level number
    levelNum++;

    rooms->clear();

    // clear the old monsters
    for (auto monster : mMonsters) 
    {
        delete monster;
    }
    mMonsters.clear();
    numMonsters = 0;

    //clear the old objects
    for (auto object : mObjects) 
    {
        delete object;
    }
    mObjects.clear();
    numObjects = 0;

    // genearte the new level
    generateLevel();

    //add the player to the new level
    addPlayer(player);

    //log the level change
    string message = "You descend to level " + to_string(levelNum) + ".";
    mLog->record(message);
}

Monster* Level::getMonster(int index) const
{
    if (index >= 0 && index < mMonsters.size())
    {
        return mMonsters[index];
    }
    else
    {
        return nullptr;
    }
}

Monster* Level::getMonsterAt(int r, int c) const
{
    for (int i = 0; i < mMonsters.size(); i++)
    {
        if (mMonsters[i]->getLocation().r() == r && mMonsters[i]->getLocation().c() == c)
        {
            return mMonsters[i];
        }
    }
    return nullptr; //null if no monster at location
}

Coord Level::generateValidCoord()
{
    int row, col = 0;

    do
    {
        //generate random coordinates within the game room
        row = randInt(1, height - 1);
        col = randInt(1, width - 1);
    } 
    while (!isValidPosition(row, col)); //repeat until a valid position is found

    Coord validCoord(row,col);

    return validCoord;
}

void Level::removeMonster(int index)
{
    if (index >= 0 && index < mMonsters.size())
    {
        mMonsters.erase(mMonsters.begin() + index);
    }
}

GameObject* Level::getGameObject(int index) const
{
    if (index >= 0 && index < mObjects.size())
    {
        return mObjects[index];
    }
    else
    {
        return nullptr;
    }
}

GameObject* Level::getGameObjectAt(int r, int c) const
{
    for (int i = 0; i < mObjects.size(); i++)
    {
        if (mObjects[i]->getLocation().r() == r && mObjects[i]->getLocation().c() == c)
        {
            return mObjects[i];
        }
    }
    return nullptr; //null if no monster at location
}

void Level::removeGameObjectAt(int r, int c)
{
    //iterate through all game objects on level
    for (int i = 0; i < numObjects; i++)
    {
        //get game objects location
        Coord objLocation = mObjects[i]->getLocation();

        //if game object is at target position
        if (objLocation.r() == r && objLocation.c() == c)
        {
            //remove game object from mObjects vector
            mObjects.erase(mObjects.begin() + i);

            numObjects--;

            break;
        }
    }
}