#include <iostream>
#include <string>
#include <algorithm>
#include "GameObject.h"
#include "Level.h"
#include "Actor.h"

using namespace std;

Actor::~Actor()
{
	
}

void Actor::setLocation(Coord newLocation)
{
	mLocation = newLocation;
}

void Actor::setHealth(int newHealth)
{
	if (newHealth > mMaxHealth) //check in case new health goes over the maximum health
	{
		mHealth = mMaxHealth;
	}
	else //else set value as is
	{
		mHealth = newHealth;
	}
}


void Actor::move(int dir)
{
	Coord oldLocation = getLocation();
	
	int newRow = oldLocation.r();
	int newCol = oldLocation.c();

	if (mCurrLvl->determineNewPosition(newRow, newCol, dir)) //update newRow and newCol if true
	{
		if (mCurrLvl->isMonsterPosition(newRow, newCol)) //returns true if a monster is at this position
		{
			Monster* monster = mCurrLvl->getMonsterAt(newRow, newCol);
			attack(monster);
		}

		else if (mCurrLvl->isPlayerPosition(newRow, newCol))
		{
			Player* player = mCurrLvl->getPlayer();
			attack(player);
		}

		//if there is empty space in the new position
		else if (mCurrLvl->isValidPosition(newRow, newCol))
		{
			Coord newLocation = Coord(newRow, newCol); //calculated new location
			setLocation(newLocation);
		}
	}
}

void Actor::heal()
{
	double p = 0.1; //10% chance of heal
	if (trueWithProbability(p))
	{
		setHealth(getHealth() + 1);
	}
}

int Actor::squaredDistBetweenCoords(Coord c1, Coord c2)
{
	int rowDiff = c1.r() - c2.r();
	int colDiff = c1.c() - c2.c();

	return rowDiff * rowDiff + colDiff * colDiff;
}


string Actor::convertSymbolToName(char actorSymbol) //converts actor symbol into string name
{
	switch (actorSymbol)
	{
	case '@':
		return "Player";
	case 'B':
		return "the Bogeyman";
	case 'S':
		return "the Snakewoman";
	case 'G':
		return "the Goblin";
	case 'D':
		return "the dragon";
	default:
		return "INVALID SYMBOL";
	}
}

//PLAYER METHODS

Player::Player(Level* level)  
	: Actor(20, Coord(), 2, 2, 2, level),
	  mCurrWeapon(new ShortSword(level)), mMaxHealth(20), mHasGoldenIdol(false)
{
	mInventory.push_back(mCurrWeapon); //initialize mInventory with the starter sword
};

Player::~Player()
{
	for (GameObject* obj : mInventory)
	{
		delete obj;
	}
}


void Player::attack(Actor* attackedActor)
{
	// check that attackedActor is not null
	if (attackedActor == nullptr)
	{
		return;
	}

	//calculate whether attack misses or not
	int attackerPoints = getDexterity() + mCurrWeapon->dexterityBonus();
	int defenderPoints = attackedActor->getDexterity();

	//setting up names to print in message
	string attacker = convertSymbolToName(actorType());
	string defender = convertSymbolToName(attackedActor->actorType()); //identify monster type
	GameLog* gameLog = getLevel()->getGameLog();

	string message = attacker + mCurrWeapon->actionVerb() + mCurrWeapon->name() + " at " + defender;

	//attack misses
	if (randInt(1, attackerPoints) <= randInt(1, defenderPoints))
	{
		message += " and misses";
	}
	else //attack hits!
	{
		//calculate damage based on actor's strength and weapon
		int damagePoints = randInt(0, getStrength() + mCurrWeapon->damageBonus() - 1);

		//subtract damage from health
		attackedActor->setHealth(attackedActor->getHealth() - damagePoints);


		if (attackedActor->getHealth() <= 0)
		{
			attackedActor->die();
			message += " dealing a final blow";
		}
        else if (trueWithProbability(0.2)) // chances of putting monster to sleep
        {
            mCurrWeapon->putDefenderToSleep(attackedActor);
            if (attackedActor->getSleepTime() > 0)
            {
                message += " and hits,";
                message += " putting ";
                message += defender + " to sleep.";
            }
            else
            {
                message += " and hits";
            }
        }
	}
	gameLog->record(message);
}

void Player::pickUp()
{
	//get game object that is at player's coord
	GameObject* objOnFloor = getLevel()->getGameObjectAt(getLocation().r(), getLocation().c());
	
	Stairs* sp = dynamic_cast<Stairs*>(objOnFloor); //try to convert scrollFromInventory
	if (sp != nullptr)
	{
		return;
	}

	GoldenIdol* gp = dynamic_cast<GoldenIdol*> (objOnFloor); 
	if (gp != nullptr)
	{
		mHasGoldenIdol = true;
		string message = "You found the Golden Idol!";
		GameLog* gameLog = getLevel()->getGameLog();
		gameLog->record(message);
		return;
	}


	if (objOnFloor != nullptr) 
	{
		//push object into player's inventory
		mInventory.push_back(objOnFloor);

		//remove the object from the level
		getLevel()->removeGameObjectAt(getLocation().r(), getLocation().c());

		//record game message
		string message = "You pick up " + objOnFloor->name();
		getLevel()->getGameLog()->record(message);
	}
}


void Player::cheat()
{
	setMaxHealth(50);
	setHealth(50);
	setStrength(9);
}

void Player::readScroll(char inventoryChar)
{
	//get game object from player's inventory
	if (inventoryChar - 'a' >= mInventory.size())
	{
		return;
	}

	GameObject* scrollFromInventory = mInventory[inventoryChar - 'a']; //converting ascii character to corresponding index

	Scroll* sp = dynamic_cast<Scroll*>(scrollFromInventory); //try to convert scrollFromInventory
	if (sp != nullptr) // if result is not null, scrollFromInventory points to a Scroll
	{
		sp->effect(this);
		delete sp; //delete the scroll
		mInventory.erase(mInventory.begin() + (inventoryChar - 'a')); //remove scroll pointer from inventory
	}
	else
	{
		return;
	}
}

void Player::wield(char inventoryChar)
{
	//get game object from player's inventory
	if (inventoryChar - 'a' >= mInventory.size())
	{
		return; //invalid index in inventory
	}

	GameObject* weaponFromInventory = mInventory[inventoryChar - 'a'];
	Weapon* wp = dynamic_cast<Weapon*>(weaponFromInventory); //try to convert weaponFromInventory
	if (wp != nullptr)
	{
		setCurrentWeapon(wp); //set the curr weapon pointer to a different weapon in inventory
        GameLog* gameLog = getLevel()->getGameLog();
        string message = "You are wielding " + wp->name();
        gameLog->record(message);
	}
}

bool Player::hasGoldenIdol()
{
	return mHasGoldenIdol;
}


void Player::displayInventory()
{
	cout << "Inventory: " << endl;
	char inventoryIndex = 'a';
	string period = ". ";
	for (vector<GameObject*>::iterator it = mInventory.begin(); it != mInventory.end(); it++, inventoryIndex++)
	{
		cout << inventoryIndex + period + (*it)->name() << endl;
	}
}


//MONSTER METHODS

Monster::~Monster()
{
	delete mCurrWeapon;
}

void Monster::attack(Actor* attackedActor)
{

	// check that attackedActor is not null
	if (attackedActor == nullptr)
	{
		return;
	}

	//get the player from monster's level
	Actor* player = attackedActor;

	if (player == nullptr)
	{
		return;
	}

	//calculate whether attack misses or not
	int attackerPoints = getDexterity() + mCurrWeapon->dexterityBonus();
	int defenderPoints = player->getDexterity();

	//setting up names to print in message
	string attacker = convertSymbolToName(actorType());
	string defender = "Player";
	GameLog* gameLog = getLevel()->getGameLog();

	string message = attacker + mCurrWeapon->actionVerb() + mCurrWeapon->name() + " at " + defender;

	//attack misses
	if (randInt(1, attackerPoints) < randInt(1, defenderPoints))
	{
		message += " and misses";
	}
	else //attack hits!
	{
		//calculate damage based on actor's strength and weapon
		int damagePoints = randInt(0, getStrength() + mCurrWeapon->damageBonus() - 1);

		//subtract damage from player's health
		player->setHealth(player->getHealth() - damagePoints);
        
		if (player->getHealth() <= 0)
		{
			player->die();
			message += " dealing a final blow";
		}
        else if (trueWithProbability(0.2)) // chances of putting player to sleep
        {
            mCurrWeapon->putDefenderToSleep(player);
            message += " and hits,";
            message += " putting";
            message += " the player to sleep.";
        }
		else
		{
			message += " and hits ";
		}
	}
	gameLog->record(message);
}

void Monster::drop() 
{
	GameObject* droppedObj = mCurrWeapon;
	if (droppedObj != nullptr)
	{
		droppedObj->setLocation(getLocation());
	}

	getLevel()->addToObjects(droppedObj);
	mCurrWeapon = nullptr;
}


bool Monster::smellPlayer(Player* player, int smellDistance)
{
	Coord playerLocation = player->getLocation();
	Coord monsterLocation = getLocation();

	if (squaredDistBetweenCoords(playerLocation, monsterLocation) <= smellDistance * smellDistance)
	{
		return true;
	}
	return false;
}


void Bogeyman::action()
{
	Player* player = getLevel()->getPlayer();
	Level* currentLevel = getLevel(); //get current level
	
	Coord playerLocation = player->getLocation();
	Coord bogeymanLocation = getLocation();

	//check if the bogeyman is right next to the player, if it is then don't move
	if (squaredDistBetweenCoords(playerLocation, bogeymanLocation) <= 1)
	{
		attack(player);
		return;
	}

	if (smellPlayer(player, 5))
	{
		int dir = UP;
		if (playerLocation.r() < bogeymanLocation.r()
			&& currentLevel->isValidPosition(bogeymanLocation.r() - 1, bogeymanLocation.c()))
			dir = UP;
		else if (playerLocation.r() > bogeymanLocation.r()
			&& currentLevel->isValidPosition(bogeymanLocation.r() + 1, bogeymanLocation.c()))
			dir = DOWN;
		else if (playerLocation.c() < bogeymanLocation.c()
			&& currentLevel->isValidPosition(bogeymanLocation.r(), bogeymanLocation.c() - 1))
			dir = LEFT;
		else if (playerLocation.c() > bogeymanLocation.c()
			&& currentLevel->isValidPosition(bogeymanLocation.r(), bogeymanLocation.c() + 1))
			dir = RIGHT;

		move(dir);
	}
}

void Bogeyman::drop()
{
	int randomNumber = randInt(1, 10);
	if (randomNumber==1)
	{
		GameObject* droppedObj = getCurrWeapon();
		if (droppedObj != nullptr)
		{
			droppedObj->setLocation(getLocation());
		}
		getLevel()->addToObjects(droppedObj);
		clearWeapon();
	}
}

void Snakewoman::action()
{
	Player* player = getLevel()->getPlayer();
	Level* currentLevel = getLevel(); //get current level

	Coord playerLocation = player->getLocation();
	Coord snakewomanLocation = getLocation();

	//check if the bogeyman is right next to the player, if it is then don't move
	if (squaredDistBetweenCoords(playerLocation, snakewomanLocation) <= 1)
	{
		attack(player);
		return;
	}

	if (smellPlayer(player, 3))
	{
		int dir = UP;
		if (playerLocation.r() < snakewomanLocation.r()
			&& currentLevel->isValidPosition(snakewomanLocation.r() - 1, snakewomanLocation.c()))
			dir = UP;
		else if (playerLocation.r() > snakewomanLocation.r()
			&& currentLevel->isValidPosition(snakewomanLocation.r() + 1, snakewomanLocation.c()))
			dir = DOWN;
		else if (playerLocation.c() < snakewomanLocation.c()
			&& currentLevel->isValidPosition(snakewomanLocation.r(), snakewomanLocation.c() - 1))
			dir = LEFT;
		else if (playerLocation.c() > snakewomanLocation.c()
			&& currentLevel->isValidPosition(snakewomanLocation.r(), snakewomanLocation.c() + 1))
			dir = RIGHT;

		move(dir);
	}
}

void Snakewoman::drop()
{
	int randomNumber = randInt(1, 3);
	if (randomNumber == 1)
	{
		GameObject* droppedObj = getCurrWeapon();
		if (droppedObj != nullptr)
		{
			droppedObj->setLocation(getLocation());
		}
		getLevel()->addToObjects(droppedObj);
		clearWeapon();
	}
}
/*
bool Goblin::smellPlayer(Player* player, int smellDistance) //GOBLIN RECURSION
{
	//base case
	if (smellDistance == 0)
	{
		return (getLocation()->r() == player->getLocation()->r()
			 && getLocation()->c() == player->getLocation()->c());
	}

	//recursive case
	int newRow = getLocation()->r();
	int newCol = getLocation()->c();

	if (getLevel()->determineNewPosition(newRow, newCol, UP) && smellPlayer(player, smellDistance - 1))
	{
		move(UP);
		return true;
	}
	if (getLevel()->determineNewPosition(newRow, newCol, LEFT) && smellPlayer(player, smellDistance - 1))
	{
		move(LEFT);
		return true;
	}
	if (getLevel()->determineNewPosition(newRow, newCol, RIGHT) && smellPlayer(player, smellDistance - 1))
	{
		move(RIGHT);
		return true;
	}
	if (getLevel()->determineNewPosition(newRow, newCol, DOWN) && smellPlayer(player, smellDistance - 1))
	{
		move(DOWN);
		return true;
	}
}
*/


bool Goblin::smellPlayer(Player* playerLocation, int smellDistance)
{
	// Get the current location of the Goblin
	Coord goblinLocation = getLocation();

	// Get the location of the Player
	Coord playerLoc = playerLocation->getLocation();

	// Create a visited matrix
	vector<vector<bool>> visited(18, vector<bool>(70, false));

	// Call the recursive function
	return smellPlayerDFS(goblinLocation, playerLoc, smellDistance, visited);
}

Direction Goblin::smellPlayerDFS(Coord goblinLocation, Coord playerLocation, int smellDistance, vector<vector<bool>>& visited)
{
	// Base case: If the Goblin is at the player's location
	if (goblinLocation.r() == playerLocation.r() && goblinLocation.c() == playerLocation.c())
	{
		return NONE;  // Return NONE to indicate that the Goblin is already at the player's location
	}

	// If smellDistance is 0, return NONE because the Goblin can't smell the player
	if (smellDistance == 0)
	{
		return NONE;
	}

	// Mark the current cell as visited
	visited[goblinLocation.r()][goblinLocation.c()] = true;

	// Create a vector of possible moves (North, East, South, West)
	vector<Direction> directions = { UP, RIGHT, DOWN, LEFT };

	// Try all possible moves
	for (int i = 0; i < 4; i++)
	{
		int newRow = goblinLocation.r() + (directions[i] == UP ? -1 : directions[i] == DOWN ? 1 : 0);
		int newCol = goblinLocation.c() + (directions[i] == LEFT ? -1 : directions[i] == RIGHT ? 1 : 0);

		// Check if the new position is valid and not visited
		if (getLevel()->isValidPosition(newRow, newCol) && !visited[newRow][newCol])
		{
			Coord newLocation(newRow, newCol);

			// Recur for the new position
			Direction result = smellPlayerDFS(newLocation, playerLocation, smellDistance - 1, visited);

			// If the result is not NONE, then a path to the player has been found
			if (result != NONE)
			{
				// If the result is NONE (the Goblin is at the player's location), return the current direction
				if (result == NONE)
				{
					return directions[i];
				}
				// Otherwise, return the result (the direction to move in)
				else
				{
					return result;
				}
			}
		}
	}

	// If no path is found, backtrack and unmark this cell
	visited[goblinLocation.r()][goblinLocation.c()] = false;

	// Return NONE to indicate that no path to the player was found within the given smell distance
	return NONE;
}


void Goblin::action()
{
	// Get the player's location and the smell distance
	Player* player = getLevel()->getPlayer();
	int smellDistance = 15;

	// Create a visited matrix
	vector<vector<bool>> visited(18, vector<bool>(70, false));

	// Call smellPlayerDFS to get the direction to move in
	Direction direction = smellPlayerDFS(getLocation(), player->getLocation(), smellDistance, visited);

	// Move the Goblin in the given direction if a valid direction was returned
	if (direction != NONE)
	{
		move(direction);
	}
}

void Dragon::action()
{
	heal(); //heal before taking turn
	Player* player = getLevel()->getPlayer();

	Coord playerLocation = player->getLocation();
	Coord dragonLocation = getLocation();

	//check if the dragon is right next to the player, if it is then don't move
	if (squaredDistBetweenCoords(playerLocation, dragonLocation) <= 1)
	{
		attack(player);
		return;
	}
}

void Dragon::drop()
{
	int randomNumber = randInt(1, 5);
	GameObject* droppedObj;
	switch (randomNumber)
	{
	case (1):
		droppedObj = new TeleportScroll(getLevel());
	case(2):
		droppedObj = new ArmorScroll(getLevel());
		break;
	case(3):
		droppedObj = new StrengthScroll(getLevel());
		break;
	case(4):
		droppedObj = new HealthScroll(getLevel());
		break;
	case(5):
		droppedObj = new DexterityScroll(getLevel());
		break;
	default: //default case, will never occur
		droppedObj = new DexterityScroll(getLevel());
	}

	if (droppedObj != nullptr)
	{
		droppedObj->setLocation(getLocation());
	}

	getLevel()->addToObjects(droppedObj);
	
}






