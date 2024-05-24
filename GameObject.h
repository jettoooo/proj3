#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <iostream>
#include <string>

#include "Coord.h"

using namespace std;

class Actor;
class Player;
class Coord;


using namespace std;
class Level;
class Actor;
class Player;

class GameObject
{
public:
	GameObject(Level* level);
	virtual ~GameObject(); //destructor
	virtual char objectType() = 0; //returns either '?' for scroll, ')' for weapon
	virtual string name() = 0; //returns name of object in string form
	virtual void setLocation(Coord newLocation); //set location of object
	Coord getLocation() { return mLocation; } //returns location coord
	Level* getLevel() { return mCurrLvl; }
	
private:
	Coord mLocation;
	Level* mCurrLvl;
};

class Weapon :public GameObject
{
public:
	Weapon(Level* level) : GameObject(level) {} //constructor
	virtual ~Weapon() {} //destroy abstract class
	virtual char objectType() { return ')'; }
	virtual int dexterityBonus() = 0; //weapons give dexterity bonus
	virtual int damageBonus() = 0; //weapons give damage bonus
	virtual string actionVerb() = 0;
};


class Mace :public Weapon
{
public:
	Mace(Level* level) : Weapon(level) {}
	virtual int dexterityBonus() { return 0; }
	virtual int damageBonus() { return 2; }
	virtual void use() {}
	virtual string name() { return "mace"; }
	virtual string actionVerb() { return " swings "; }
};

class ShortSword :public Weapon
{
public:
	ShortSword(Level* level) : Weapon(level) {}
	virtual int dexterityBonus() { return 0; }
	virtual int damageBonus() { return 2; }
	virtual string name() { return "short sword"; }
	virtual string actionVerb() { return " slashes "; }
};

class LongSword :public Weapon
{
public:
	LongSword(Level* level) : Weapon(level) {}
	virtual int dexterityBonus() { return 2; }
	virtual int damageBonus() { return 4; }
	virtual string name() { return "long sword"; }
	virtual string actionVerb() { return " slashes "; }
};

class MagicAxe :public Weapon
{
public:
	MagicAxe(Level* level) : Weapon(level) {}
	virtual int dexterityBonus() { return 5; }
	virtual int damageBonus() { return 5; }
	virtual string name() { return "magic axe"; }
	virtual string actionVerb() { return " swings "; }
};

class MagicFangs :public Weapon
{
public:
	MagicFangs(Level* level) : Weapon(level) {}
	virtual int dexterityBonus() { return 3; }
	virtual int damageBonus() { return 2; }
	virtual string name() { return "magic fangs"; }
	virtual string actionVerb() { return " strikes "; }
	void putDefenderToSleep(); //has the ability to put defenders to sleep
};

class Scroll :public GameObject
{
public:
	Scroll(Level* level) : GameObject(level) {}
	virtual ~Scroll() {}
	virtual void effect(Player* player) = 0;
	virtual string name() = 0;
	virtual char objectType() { return '?'; }
};

class TeleportScroll :public Scroll
{
public:
	TeleportScroll(Level* level) : Scroll(level) {}
	virtual void effect(Player* player); //teleports player to random location
	virtual string name() { return "scroll of teleportation"; }
};

class ArmorScroll :public Scroll
{
public:
	ArmorScroll(Level* level) : Scroll(level) {}
	virtual void effect(Player* player); //improves armor stat of player
	virtual string name() { return "scroll of improve armor"; }
};

class StrengthScroll :public Scroll
{
public:
	StrengthScroll(Level* level) : Scroll(level) {}
	virtual void effect(Player* player); //improves strength stat of player
	virtual string name() { return "scroll of raise strength"; }
};

class HealthScroll :public Scroll
{
public:
	HealthScroll(Level* level) : Scroll(level) {}
	virtual void effect(Player* player); //enhances max health
	virtual string name() { return "scroll of enhance health"; }
};

class DexterityScroll :public Scroll
{
public:
	DexterityScroll(Level* level) : Scroll(level) {}
	virtual void effect(Player* player); //improves dexterity stat of player
	virtual string name() { return "scroll of enhance dexterity"; }
};

class Stairs :public GameObject
{
public:
	virtual ~Stairs() {} //destructor
	Stairs(Level* level) :GameObject(level) {}
	virtual char objectType() { return '>'; } //to help game print
	virtual string name() { return "stairs"; } //returns name of object in string form
};

class GoldenIdol :public GameObject
{
public:
	virtual ~GoldenIdol() {} //destructor
	GoldenIdol(Level* level) :GameObject(level) {}
	virtual char objectType() { return '&'; }
	virtual string name() { return "the golden idol"; }
};
#endif
