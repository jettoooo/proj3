#ifndef ACTOR_H
#define ACTOR_H

#include <iostream>
#include <string>
#include "utilities.h"
#include "Coord.h"
#include "GameObject.h"

using namespace std;

class Level; //forward declaration
class GameObject;
class Weapon;

enum Direction  //enumeration for directional movements
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE       //for goblin recursion to indicate we are at the player
};



class Actor
{
public:
    Actor(int health, Coord location, int armor,
        int strength, int dexterity, Level* level)
        : mHealth(health), mLocation(location), mArmor(armor),
        mStrength(strength), mDexterity(dexterity), 
        mSleepTime(0), mCurrLvl(level), mIsAlive(true), mMaxHealth(health)
    {};

    virtual ~Actor(); //base destructor
    
    virtual char actorType() = 0;


    //GETTERS
    virtual Coord getLocation() const { return mLocation; }//returns location of actor
    virtual int getHealth() const { return mHealth; }//returns health of actor
    virtual int getMaxHealth() const { return mMaxHealth; } //return max health
    virtual int getArmor() const { return mArmor; } //return armor
    virtual int getStrength() const { return mStrength; } //return strength
    virtual int getDexterity() const { return mDexterity; } //return dexterity
    Level* getLevel() const { return mCurrLvl; } //return pointer to level actor is on
    bool isAlive() const { return mIsAlive; } //indicator for whether actor is still alive

    //SETTERS
    virtual void setLocation(Coord newLocation); //updates location of actor
    virtual void setMaxHealth(int newMaxHealth) { mMaxHealth = newMaxHealth; } //update the maximum health of actors
    virtual void setHealth(int newHealth); //updates health value
    virtual void setArmor(int newArmor) { mArmor = newArmor; } //updates armor value
    virtual void setStrength(int newStrength) { mStrength = newStrength; } //updates strength value
    virtual void setDexterity(int newDexterity) { mDexterity = newDexterity; } //updates dex value

    virtual void move(int dir); //move actor
    virtual void die() { mIsAlive = false; } //actor is no longer living
    virtual void attack(Actor* attackedActor) = 0;
    virtual void heal(); //heal for player and dragon

    void setLevel(Level* level) {mCurrLvl = level;} //set the current level of the actor
    int squaredDistBetweenCoords(Coord playerLocation, Coord monsterLocation); //finds distance between two coords squared
    string convertSymbolToName(char actorSymbol); //converts actor symbol into string name

private:
	int mHealth;
	Coord mLocation;
    // Weapon mCurrWeapon;
    int mArmor;
    int mStrength;
    int mDexterity;
    int mSleepTime;
    Level* mCurrLvl; //current level the actor is on
    bool mIsAlive;
    int mMaxHealth;
};

class GameObject; //forward declaration

class Player :public Actor
{
public:
    Player(Level* level);
    virtual ~Player();

    virtual char actorType() { return '@'; }
    virtual void attack(Actor* attackedActor); //attack an actor
    void pickUp(); //pick up an item
    void readScroll(char inventoryChar); //read scroll
    void wield(char inventoryChar); //wield weapon from inventory
    void setCurrentWeapon(Weapon* newWeapon) { mCurrWeapon = newWeapon; } //set the curr weapon pointer to a different weapon in inventory
    void cheat(); //cheats so that we can test the game easily

    void displayInventory(); //show player's inventory
    int inventorySize() { return mInventory.size(); } //return size of player's inventory
    bool hasGoldenIdol(); //returns true if player has golden idol, sending signal to end the game

private:
    Weapon* mCurrWeapon; 
    vector<GameObject*> mInventory; //inventory is a vector of gameobject pointers
    int mMaxHealth;
    bool mHasGoldenIdol;

};

class Monster :public Actor
{
public:
    Monster(int health, Coord location, int armor,
            int strength, int dexterity, Level* level, Weapon* weapon) 
            : Actor(health, location, armor, strength, dexterity, level), mCurrWeapon(weapon) {};
    ~Monster();
    virtual void attack(Actor* attackedActor);
    virtual void drop(); //drop weapon or scroll
    virtual void action() = 0;
    virtual bool smellPlayer(Player* playerLocation, int smellDistance);
    virtual void clearWeapon(){ mCurrWeapon = nullptr; } //set currweapon to null

    Weapon* getCurrWeapon() const { return mCurrWeapon; } //get current weapon

    void setCurrWeapon(Weapon* weapon) { mCurrWeapon = weapon; } //set current weapon

private:
    Weapon* mCurrWeapon; //every monster has a weapon
};

class Bogeyman :public Monster
{
public:
    Bogeyman(Level* level)  //bogeyman constructor
        : Monster(randInt(5, 10), Coord(5, 5), 2, 
                  randInt(2, 3), randInt(2, 3), level, 
                  new MagicAxe(level)) {}; 
    virtual void action();
    virtual char actorType() { return 'B'; }
    virtual void drop(); //1 in 10 chance to drop magic axe

};

class Snakewoman :public Monster
{
public: 
    Snakewoman(Level* level) //snakewoman constructor
        : Monster(randInt(3, 6), Coord(), 3,
            2, 3, level, new MagicFangs(level)){};
    virtual void action();
    virtual char actorType() { return 'S'; }
    virtual void drop(); //1 in 3 chance to drop sharp fangs
};

class Goblin :public Monster
{
public:
    Goblin(Level* level) //goblin constructor
        : Monster(randInt(15, 20), Coord(), 1, 3, 1, level, new ShortSword(level)) {};
    virtual void action();
    virtual char actorType() { return 'G'; }
    bool smellPlayer(Player* playerLocation, int smellDistance);
    Direction smellPlayerDFS(Coord goblinLocation, Coord playerLocation, int smellDistance, vector<vector<bool>>& visited);

};

class Dragon :public Monster
{
public:
    Dragon(Level* level) //dragon constructor
        :Monster(randInt(20, 25), Coord(), 4, 4, 4, level, new LongSword(level)) {};
    virtual void action();
    virtual char actorType() { return 'D'; }
    virtual void drop(); //drop random scroll
};



#endif
