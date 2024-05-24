#ifndef GAMEROOM_H
#define GAMEROOM_H

#include <iostream>
#include <list>
#include <vector>
#include "Actor.h"
#include "utilities.h"
#include "GameLog.h"
#include "GameObject.h"

using namespace std;


class Level
{
public:
    Level() : gameRoom(18, vector<char>(70, '#')), rooms(new vector<Room>),
        height(18), width(70), mPlayer(new Player(this)), mMonsters(0), numMonsters(0), 
        mLog(new GameLog), mObjects(0), numObjects(0), levelNum(0)
    {
        generateLevel();
    };

    ~Level(); //destructor

    //room generation
    bool doesOverlap(int x, int y, int length, int width);
    void generateCorridor(int room1Index, int room2Index);
    void generateRoom(); //generate one level room
    void generateLevel(); //generate level, takes level number as argument
    void printRoom() const; //print the game room onto display
    

    Player* getPlayer() const {return mPlayer;} 
    void addPlayer(Player* player); //spawn player into the game
    void addMonster(); //choose room to spawn monsters into game
    void addObject(); //choose empty place to spawn objects
    void addGoldenIdol(); //add the golden idol into the final level
    void addStairs(); //add stairs object into game, only for levels 0-3
    bool determineNewPosition(int& r, int& c, int dir) const; //checks if actor can go in specified direction without walking off, if it can then will update newRow and newCol
    bool isValidPosition(int r, int c) const; //returns true if the position is empty
    bool isMonsterPosition(int r, int c) const; //returns true if a monster is at this position
    bool isPlayerPosition(int r, int c) const; //returns true if a player is at this position
    int getNumMonsters() const { return numMonsters; }
    Monster* getMonster(int index) const; //get monster based on index
    Monster* getMonsterAt(int r, int c) const; //get monster at specified position
    GameLog* getGameLog() const { return mLog; }
    Coord generateValidCoord(); //generates a random coordinate that is empty
    void removeMonster(int index); //remove monster from vector at specified index
    GameObject* getGameObject(int index) const; //get object based on index
    GameObject* getGameObjectAt(int r, int c) const; //get game object at specified position
    void removeGameObjectAt(int r, int c); //remove the game object from level at specified location
    void createNewLevel(); //intended to be called by game when player moves to the next level
    int getLevelNum() const { return levelNum; } // get level number
    void addToObjects(GameObject* droppedObject) { mObjects.push_back(droppedObject); numObjects++; } //push dropped object to level's object vector


private:
    struct Room
    {
        int mTop;
        int mLeft;
        int mWidth;
        int mHeight;
    };

    vector<vector<char>> gameRoom;
    vector<Room>* rooms;
    int height;
    int width;
    Player* mPlayer;
    vector<Monster*> mMonsters;
    int numMonsters;
    GameLog* mLog;
    vector<GameObject*> mObjects;
    int numObjects;
    int levelNum;
};
#endif
