#include <iostream>
#include "GameObject.h"
#include "Level.h"
#include "Actor.h"




GameObject::GameObject(Level* level) 
	: mLocation(Coord()), mCurrLvl(level) {}

GameObject::~GameObject()
{
}

void GameObject::setLocation(Coord newLocation)
{
	mLocation = newLocation;
}

//SCROLL METHODS

void TeleportScroll::effect(Player* player)
{
	Level* level = getLevel();
	//generate a random coord to teleport to
	Coord teleportDestination = level->generateValidCoord();

	//change player's location to teleport destination
	player->setLocation(teleportDestination);

	string message = "You teleported!";
	level->getGameLog()->record(message);
}

void ArmorScroll::effect(Player* player)
{
	Level* level = getLevel();

	int armorIncrease = randInt(1, 3); //generate random number of armor point increase

	player->setArmor(player->getArmor() + armorIncrease); //adjust armor increase
	string message = "Your armor stat increased by: " + to_string(armorIncrease) + "!";
	level->getGameLog()->record(message);
}

void StrengthScroll::effect(Player* player)
{
	Level* level = getLevel();

	int strengthIncrease = randInt(1, 3); //generate random number of strength point increase

	player->setStrength(player->getStrength() + strengthIncrease); //adjust strength increase
	string message = "Your strength stat increased by: " + to_string(strengthIncrease) + "!";
	level->getGameLog()->record(message);
}

void HealthScroll::effect(Player* player)
{
	Level* level = getLevel();

	int maxHealthIncrease = randInt(3, 8); //generate random number of max health increase

	player->setMaxHealth(player->getMaxHealth() + maxHealthIncrease); //adjust max health increase
	string message = "Your maximum health increased by: " + to_string(maxHealthIncrease) + "!";
	level->getGameLog()->record(message);
}

void DexterityScroll::effect(Player* player)
{
	Level* level = getLevel();

	int dexterityIncrease = 1; //generate random number of dex point increase

	player->setDexterity(player->getDexterity() + dexterityIncrease); //adjust dex increase
	string message = "Your dexterity stat increased by: " + to_string(dexterityIncrease) + "!";
	level->getGameLog()->record(message);
}


