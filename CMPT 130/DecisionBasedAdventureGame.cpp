#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <string>
#include <cctype>
#include <limits>

using namespace std;

const int STARTING_MAX_DAMAGE = 50;
const int MIN_DAMAGE_USER = 10;

const int MONSTER_ONE_HEALTH = 50;
const int MONSTER_TWO_HEALTH = 75;
const int MONSTER_THREE_HEALTH = 100;
const int MONSTER_FOUR_HEALTH = 150;
const int M1POINTS = 1;
const int M2POINTS = 3;
const int M3POINTS = 5;
const int M4POINTS = 10;
const int M1_DMG = 15;
const int M2_DMG = 20;
const int M3_DMG = 30;
const int M4_DMG = 50;
const int BOSS_DMG = 100;
const int BOSS_HEALTH = 500;
const double DMG_DECREASE = 1.2;

// The const index value of each room, so that they may be called upon in the program:
const int HOME = 0;
const int FIELD = 1;
const int WOODS = 2;
const int TRADER1 = 3;
const int MEADOWS = 4;
const int MATH1 = 5;
const int TUNNEL = 6;
const int PATH = 7;
const int SWAMP = 8;
const int TRADER2 = 9;
const int QUARRY = 10;
const int LAVA_SWAMP = 11;
const int MATH2 = 12;
const int TOWNHOUSE = 13;
const int TRADER3 = 14;
const int BOSS = 15;
const int END = 16;


struct transition_t{
    string direction;
    int roomNum;
    char correctLetter;
    char correctLetterCap;
};

struct Boss_t{
    string name;
    string intro;
    string halfHealth;
    string defeat;
    int health;
    int maxDamage;
};

struct UserData_t{
    string name;
    int userClass;
    int maxHealth;
    int userHealth;
    int MaxDamage;
    int MinDamage;
    int userPoints;
};

struct EnemyData_t{
    string type;
    int health;
    int points;
    int weakness;
    int maxDamage;
};

struct Trader_t{
    string name;
    string tradeOptions;
    int tradeOnePValue;
    int tradeOne;
    int tradeTwoPValue;
    int tradeThreePValue;
    int tradeThree;
};

struct Mathtest_t{
    string name;
    string challenge;
    int answer;
    int points;
};

struct Area_t{
    string areaName;
    string areaInfo;
    vector<transition_t> transitions;
    int moves;
    bool start;
    bool hasEnemy;
    bool hasMath;
    bool hasTrader;
    bool hasBoss;
    bool isEnd;
    bool complete;
};

void title()
{
    cout << "         " << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
    cout << "         " << "***** *     ***   ***** ***     ***   * **   *  ***  " << endl;
    cout << "         " << "*     *     *   * *     *   *   *   * * * *  * *     " << endl;
    cout << "         " << "***** *     *   * ***** ***     ***   * *  * * *  ** " << endl;
    cout << "         " << "*     *     *   * *     *  *    *  *  * *   ** *   * " << endl;
    cout << "         " << "***** ***** ***   ***** *   *   *   * * *    *  ***  " << endl << endl;
    cout << "         " << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl; 
    return;
}

void howToPlay()
{
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HOW TO PLAY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "You are visiting your aunt in the neighbouring city and decide to take a " << endl;
    cout << "                 shortcut through the enchanted forest                   " << endl;
    cout << " You must make your way through the woods and arrive at your Aunt's home " << endl;
    cout << "  You will gain points from defeating enemies, and solving math problems " << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "                         Troll   = 1  Point " << endl;
    cout << "                         Thief   = 3  Points " << endl;
    cout << "                         Cyclops = 5  Points " << endl;
    cout << "                         Dragon  = 10 Points " << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "     These points will upgrade your character, making you stronger.      " << endl;
    cout << "There are three traders around the map who will let you upgrade one thing." << endl;
    cout << "    You can only make one trade per trader each game, so pick wisely.    " << endl;
    cout << "     You will pick a class, which some enemies might be weak against.    " << endl;
    cout << "                      There are two possible endings                     " << endl;
}

// two boolean functions to easily check user input at any point
bool isValidUserNumber(int inputNum){
    return inputNum == 1 || inputNum == 2 || inputNum == 3;
}

// specifically to check letter input including capitals for when two letters are options
bool isValidLetterTwo(char a, char b, char c, char d, char input)
{
    return (input == a || input == b || input == c || input == d);
}

// the user's decisions for the game
UserData_t userInput()
{
    UserData_t userData;
    // get name
    cout << "What is your name? ";
    getline(cin, userData.name);
    cout << endl;

    // choose class
    cout << "Are you a Swordsman, Archer, or Mage? " << endl;
    cout << "Choose 1, 2 or 3: ";
    cin >> userData.userClass;
        while(!isValidUserNumber(userData.userClass)){
            cout << "Choose 1, 2, or 3: ";
            cin >> userData.userClass;
            }

    // display their choice
    if (userData.userClass == 1){
        cout << "A swordsman by the name of " << userData.name << "! " << endl;
    }else if (userData.userClass == 2){
        cout << "An Archer by the name of " << userData.name << "! " << endl;
    } else if (userData.userClass == 3){
        cout << "A Mage by the name of " << userData.name << "! " << endl;
    }
    
    // User chooses how much health they have
    cout << "How much health will you begin with? (max 500): ";
    cin >> userData.userHealth;
        while(userData.userHealth > 500 || userData.userHealth <= 0){
            cout << "Your health must be between 1 and 500. ";
            cin >> userData.userHealth;
        }
    // set the max health
    userData.maxHealth = userData.userHealth;

    // Tell the user how much damage they deal
    cout << "You currently deal a maximum damage of 50 Hit Points." << endl;
    userData.MaxDamage = 50;
    // Tell the user their minimum damage
    cout << "You deal a minimum damage of " << MIN_DAMAGE_USER << " Hit Points." << endl;
    userData.MinDamage = MIN_DAMAGE_USER;

    // Initialize how many points the user has
    userData.userPoints = 0;

    return userData;
}

// vector of enemies
vector<EnemyData_t> makeEnemies(){
    return
    {
        {
            .type = "Troll",
            .health = MONSTER_ONE_HEALTH,
            .points = M1POINTS,
            .weakness = rand() % 3 + 1,
            .maxDamage = M1_DMG
        },
        {
            .type = "Thief",
            .health = MONSTER_TWO_HEALTH,
            .points = M2POINTS,
            .weakness = rand() % 3 + 1,
            .maxDamage = M2_DMG
        },
        {
            .type = "Cyclops",
            .health = MONSTER_THREE_HEALTH,
            .points = M3POINTS,
            .weakness = rand() % 3 + 1,
            .maxDamage = M3_DMG,
        },
        {
            .type = "Dragon",
            .health = MONSTER_FOUR_HEALTH,
            .points = M4POINTS,
            .weakness = rand() % 3 + 1,
            .maxDamage = M4_DMG,
        }
    };
}

// Vector of traders
vector<Trader_t> makeTraders(){
    return 
    {
        // first trader
        {
            .name = "Nobus",
            .tradeOptions = "This trader offers: +10 Damage = 2 pts  |  Full Heal = 1 pt |  + 50 Max Health = 2 pts",
            .tradeOnePValue = 2,
            .tradeOne = 10,
            .tradeTwoPValue = 1,
            .tradeThreePValue = 2,
            .tradeThree = 50,
        },
        // second trader
        {
            .name = "Skillbus",
            .tradeOptions = "This trader offers: +20 Damage = 3 pts | Full Heal = 1 pt | + 75 Max Health = 4 pts",
            .tradeOnePValue = 3,
            .tradeOne = 20,
            .tradeTwoPValue = 1,
            .tradeThreePValue = 4,
            .tradeThree = 75,
        },
        // thrid trader
        {
            .name = "Talentbus",
            .tradeOptions = "This trader offers: +100 Damage = 15 pts | Full Heal = 1 pt | +500 Max health = 20 pts",
            .tradeOnePValue = 15,
            .tradeOne = 100,
            .tradeTwoPValue = 1,
            .tradeThreePValue = 20,
            .tradeThree = 500,
        }
    };
}

Boss_t boss()
{
    return{
            .name = "The Developer",
            .intro = "I appreciate you playing my game. Unfortunatly, you were never supposed to make it this far. \n Now you must die! \n",
            .halfHealth = "Ouch! You're stronger than you look! ",
            .defeat = "As The Developer blinks out of existance, he leaves behind a tear in the fabric of your reality. \n There are two directions in front of you: the path to your Aunt's home, or the mystery of another adventure...\n",
            .health = BOSS_HEALTH,
            .maxDamage = BOSS_DMG,
    };
}

// Vector of math challenges
vector<Mathtest_t> makeMathTests(){
    return
    {
        {
            .name = "Quandavious the Knowledgable",
            .challenge = "He says: What is six multiplied by seven?",
            .answer = 42,
            .points = 5,
        },
        {
            .name = "Skibidi Dibbidy",
            .challenge = "He asks: If Kai Cenat watches 13 episodes of Skibidi Toilet a week, how many episodes has we watched in one month (four weeks)?",
            .answer = 52,
            .points = 10,
        }
    };
}

// THE MAP VECTOR:
vector<Area_t> makeAreas(){
    return 
        {   
            // Starting Area - only forward
            {
                .areaName = "your Home",
                .areaInfo = "You must go see your aunt, the only way is forward...",
                .transitions = {{"(U)p", 1, 'u', 'U'}},
                .moves = 1,
                .start = true,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Second area - forward or backwards
            {
                .areaName = "A Field",
                .areaInfo = "You encounter a Troll named Bogus!",
                .transitions = {{"(D)own", 0, 'd', 'D'}, {"(U)p", 2, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Third row - forward or backwards
            {
                .areaName = "The Peaceful Woods",
                .areaInfo = "You encounter the cousin of Bogus, Booger!",
                .transitions = {{"(D)own", 1, 'd', 'D'}, {"(U)p", 3, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // fourth row - there are three options to move through and you start at the trader's stand
            {
                .areaName = "Trader's Stand #1",
                .areaInfo = "You may choose one option, or come back and get something later.",
                .transitions = {{"(D)own", 2, 'd', 'D'}, {"(L)eft", 5, 'l', 'L'}, {"(R)ight", 4, 'r', 'R'}, {"(U)p", 7, 'u', 'U'}},
                .moves = 4,
                .start = false,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = true,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            {
                .areaName = "The Misty Meadows",
                .areaInfo = "You find a third troll mourning the loss of his two brothers, and he's ANGY >:(",
                .transitions = {{"(L)eft", 3, 'l', 'L'}, {"(U)p", 8, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            {
                .areaName = "A mysterious shadow",
                .areaInfo = "You see a man lurking in the depths of the shadow, his name is ",
                .transitions = {{"(R)ight", 3, 'r', 'R'}, {"(U)p", 6, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = false,
                .hasMath = true,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Fifth row - there are three options to move through
            {
                .areaName = "A Cavernous Tunnel",
                .areaInfo = "You are confronted by a theif! ",
                .transitions = {{"(D)own", 5, 'd', 'D'}, {"(R)ight", 7, 'r', 'R'}},
                .moves = 2,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            {
                .areaName = "A simple path...",
                .areaInfo = "It splits in four directions: down, left, right, and up.",
                .transitions = {{"(D)own", 3, 'd', 'D'}, {"(L)eft", 6, 'l', 'L'}, {"(R)ight", 8, 'r', 'R'}, {"(U)p", 9, 'u', 'U'}},
                .moves = 4,
                .start = false,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            {
                .areaName = "The Spooky Swamp",
                .areaInfo = "You spot a large cyclops coming your way!", 
                .transitions = {{"(D)own", 4, 'd', 'D'}, {"(L)eft", 7, 'l', 'L'}},
                .moves = 2,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Sixth Row - the only way to access this row is by the path
            {
                .areaName = "Trader's stand #2.",
                .areaInfo = "You may choose one option, or come back and get something later.",
                .transitions = {{"(D)own", 7, 'd', 'D'}, {"(U)p", 10, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = true,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Seventh Row - There are two options in the second and third row
            {
                .areaName = "An abandoned rock quarry.",
                .areaInfo = "You encounter a Theif!",
                .transitions = {{"(D)own", 9, 'd', 'D'}, {"(L)eft", 11, 'l', 'L'}, {"(U)p", 12, 'u', 'U'}},
                .moves = 3,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            {
                .areaName = "A boiling hot Lava Swamp!",
                .areaInfo = "You encounter another cyclops! This one is expecially agitated...",
                .transitions = {{"(R)ight", 10, 'r', 'R'}},
                .moves = 1,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Eigth row - only a math challenge resides here and you can only access it by the quarry
            {
                .areaName = "The brainrot mist",
                .areaInfo = "You must answer the question of the brain rotten: ",
                .transitions = {{"(D)own", 10, 'd', 'D'}, {"(U)p", 13, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = false,
                .hasMath = true,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Ninth row - the only way to access here is by the math challenge
            {
                .areaName = "A well-lit townhouse in the middle of the woods...",
                .areaInfo = "Unfortunatly, this is the home of a dragon and she is intent upon eating you.",
                .transitions = {{"(D)own", 12, 'd', 'D'}, {"(L)eft", 14, 'l', 'L'}, {"(U)p", 15, 'u', 'U'}},
                .moves = 3,
                .start = false,
                .hasEnemy = true,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            {
                .areaName = "Trader Stand #3.",
                .areaInfo = "This trader offers: +100 Damage = 15 pts | Full Heal = 1 pt | +500 Max health = 20 pts",
                .transitions = {{"(R)ight", 13, 'r', 'R'}, {"(U)p", 15, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = true,
                .hasBoss = false,
                .isEnd = false,
                .complete = false,
            },
            // Tenth row - the boss takes up this entire row
            {
                .areaName = "A massive ominous arena...",
                .areaInfo = "A man stands in the center and beckons you to approach... do you? (Y)es (N)o",
                .transitions = {{"(D)own", 14, 'd', 'D'}, {"(U)p", 16, 'u', 'U'}},
                .moves = 2,
                .start = false,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = true,
                .isEnd = false,
                .complete = false,
            },
            // Final row (11) - this is the end of the game - display some text and then end the program
            {
                .areaName = "Your Aunt's home.",
                .areaInfo = "Wait, this is the wrong address!",
                .transitions = {{"(D)own", 15, 'd', 'D'}},
                .moves = 1,
                .start = false,
                .hasEnemy = false,
                .hasMath = false,
                .hasTrader = false,
                .hasBoss = false,
                .isEnd = true,
                .complete = false, 
            }
    };
}

// Display Area info
void displayCurrentArea(const Area_t &area)
{
    cout << "You enter " << area.areaName << endl;
    if(area.complete){
        cout << "You have completed this area." << endl;
    }
    if(area.hasEnemy || area.hasMath || area.hasTrader || area.hasBoss || area.isEnd || area.start){
        cout << area.areaInfo << endl;
    }
    return;
}

// function for every fight
void doBattle(Area_t &currentArea, UserData_t &userData, EnemyData_t currentEnemy)
{
    int maxEnemyDmg = currentEnemy.maxDamage;
    int enemyHealth = currentEnemy.health;

    char userEscape;
    cout << "Would you like to run away? (Y)es (N)o" << endl << "> ";
    cin >> userEscape;
    while (true){ 
        if(isValidLetterTwo('y', 'Y', 'n', 'N', userEscape)){ 
            break; 
        } 
        cout << "Please choose a valid input: " << endl << ">"; 
        cin >> userEscape; 
        // Check if the input is valid using the function 
        // Clear input buffer if the input is invalid 
        cin.clear(); 
        // Clear any error flags 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        // Discard invalid input 
        } 
        // Process the valid input 
        if (userEscape == 'y' || userEscape == 'Y'){
        return;
        }

    cout << "You engage in battle with a " << currentEnemy.type << endl;
    cout << "Your Maximum Damage: " << userData.MaxDamage << " dmg" << endl << endl;

    // see if enemy is weak against you
    currentEnemy.weakness = (rand() % 3) + 1;
        if(userData.userClass == currentEnemy.weakness){
            maxEnemyDmg /= DMG_DECREASE;
            cout << "This enemy is currently weak against you! ";
            cout << "They deal 20% less damage against you!" << endl;
        }else{
            cout << "This enemy is not weak against you right now." << endl;
        }

    // variable to keep track of the turn
    int i = 0;
    while (userData.userHealth > 0 && enemyHealth > 0){
        // Go back and forth with the enemy
        if(i % 2 == 0){
            int roundDamage = userData.MinDamage + (rand() % (userData.MaxDamage - userData.MinDamage + 1));
            enemyHealth -= roundDamage;
            cout << "You deal " << roundDamage << " dmg" << " to the " << currentEnemy.type << endl;
            cout << "Enemy Health: " << enemyHealth << endl;
        }else{
            // enemy attack - it is possible for them to deal no damage
            int enemyDamage = rand() % currentEnemy.maxDamage;
            userData.userHealth -= enemyDamage;
            cout << "Enemy's turn. They deal " << enemyDamage  << " dmg" << endl;
            if (enemyDamage == 0){
                cout  << "They miss!" << endl;
            }
            cout << "Your Health: " << userData.userHealth << endl;
            // If user dies, end the game
            if (userData.userHealth <= 0){
                cout << "You have died a glorious death, may you have more luck in the next life..." << endl;
                exit(0);
            }

            // allow user the ability to run away
            char yesOrNo2;
            cout << "Would you like to run away? (Y)es (N)o: " << endl << "> ";
            cin >> yesOrNo2;
            while(!isValidLetterTwo('y', 'Y', 'n', 'N', yesOrNo2)){
                cout << "Y or N: ";
                cin >> yesOrNo2;
            }
            if(yesOrNo2 == 'Y' || yesOrNo2 == 'y'){
                return;
            }
        }
        i++;
    }
    // if you win, update the important information
    if (enemyHealth <= 0){
        currentArea.complete = true;
        currentArea.hasEnemy = false;
        userData.userPoints += currentEnemy.points;
        cout << "You defeat the enemy and clear the stage! " << endl;
        cout << "You gain " << currentEnemy.points << " point(s)" << endl << endl << endl;
    }
    return;
}

// function for every trade
void doTrade(Area_t &currentArea, UserData_t &userData, Trader_t currentTrader)
{
    char userChoice;
    // check to see if room is done
    if(currentArea.complete){
        cout << currentTrader.name << " says hello." << endl;
        return;
    }
    // Display options for trade
    cout << "This Trader's name is " << currentTrader.name << endl;
    cout << currentTrader.tradeOptions << endl;
    // ask if they want to trade
    cout << "Would you like to make a trade? (Y)es or (N)o?" << endl << "> ";
    cin >> userChoice;
    while(!isValidLetterTwo('y', 'n', 'Y', 'N', userChoice)){
        cout << "Invalid Input: Re-enter: ";
        cin >> userChoice;
    }
    if(userChoice == 'n' || userChoice == 'N'){
        return;
    }
    cout << "You have: " << userData.userPoints << " points" << endl;
    int userNumChoice = 0;
    cout << "Trade #1: " << currentTrader.tradeOnePValue << endl;
    cout << "Trade #2: " << currentTrader.tradeTwoPValue << endl;
    cout << "Trade #3: " << currentTrader.tradeThreePValue << endl;

    bool validTrade = false;
    while(!validTrade){
        cout << "1, 2, or 3: " << endl << "> ";
        cin >> userNumChoice;
    
        if(userNumChoice == 1 || userNumChoice == 2 || userNumChoice == 3){
            // if they pick #1
            if(userNumChoice == 1){
                // see that they have enough points
                if(userData.userPoints < currentTrader.tradeOnePValue){
                    cout << "You don't have enough points for this trade." << endl;
                }else{
                // if they have enough, make trades
                    userData.MaxDamage += currentTrader.tradeOne;
                    userData.MinDamage += currentTrader.tradeOne;
                    userData.userPoints -= currentTrader.tradeOnePValue;
                    cout << " Your maximum and minimum damage has increased by: " << currentTrader.tradeOne << endl;
                    validTrade = true;
                }
            // if they pick #2
            }else if(userNumChoice == 2){
                // see that they have enough points
                if(userData.userPoints < currentTrader.tradeTwoPValue){
                    cout << "You don't have enough points for this trade." << endl;
                }else{
                    // if they have enough, make trades
                    userData.userHealth = userData.maxHealth;
                    userData.userPoints -= currentTrader.tradeTwoPValue;
                    cout << "Your health has been refilled!" << endl;
                    validTrade = true;
                }
            // if they pick #3
            }else if(userNumChoice == 3){
                // see that they have enough points
                if(userData.userPoints < currentTrader.tradeThreePValue){
                    cout << "You don't have enough points for this trade." << endl;
                }else{
                    // if they have enough, make trades
                    userData.maxHealth += currentTrader.tradeThree;
                    userData.userHealth = userData.maxHealth;
                    userData.userPoints -= currentTrader.tradeThreePValue;
                    cout << "Your max health has been increased by " << currentTrader.tradeThree << ", and your health has been refilled!" << endl;
                    validTrade = true;
                }
            }
        }else{
            cout << "Invalid choice. Please choose 1, 2, or 3." << endl; 
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    currentArea.hasTrader = false;
    currentArea.complete = true;
    return;
}

// function for every math challenge
void doMath(Area_t &currentArea, UserData_t &userData, Mathtest_t math)
{
    int userAnswer = 0;
    // Display the math challenge
    cout << math.name << endl;

    // ask the user if they want to run away
    char userEscape;
    cout << "Would you like to run away? (Y)es (N)o" << endl << "> ";
    cin >> userEscape;
    while(!isValidLetterTwo('y', 'Y', 'n', 'N', userEscape)){
        cout << "Please choose a valid input: " << endl << ">";
        cin >> userEscape;
    }
    if(userEscape == 'y' || userEscape == 'Y'){
        return;
    }

    cout << math.challenge << endl;
    cout << "You answer: " << endl << "> ";
    cin >> userAnswer;
    // if they get it right, hooray! If not, bad news
    if(userAnswer == math.answer){
        cout << math.name << " is impressed with your quick thinking! He rewards you with " << math.points << " points as a reward!" << endl;
        userData.userPoints += math.points;
    }else{
        cout << math.name << " finds your lack of math skills disturbing and ends the game so you can go study." << endl;
        exit(0);
    }
    // update the room
    currentArea.hasMath = false;
    currentArea.complete = true;
    return;
}

// function for boss battle
void bossBattle(Area_t &currentArea, UserData_t &userData, Boss_t boss)
{
    int initialBossHealth = boss.health;
    bool halfHealthMessage = false;
    int maxBossDmg = boss.maxDamage;

    // allow the user one chance to run away:
    char fightBoss;
        cin >> fightBoss;
        while(!isValidLetterTwo('y', 'Y', 'n', 'N', fightBoss)){
            cout << "Y or N: ";
            cin >> fightBoss;
        }
        if(fightBoss == 'n' || fightBoss == 'N'){
            return;
        }

    // tell the user their stats:
    cout << "Your Health: " << userData.userHealth << " hp" << endl;
    cout << "Your Maximum Damage: " << userData.MaxDamage << " dmg" << endl << endl;


    cout << "The man is shrouded in mystery... he introduces himself as: " << boss.name << "." << endl;
    cout << boss.intro << endl;
    // variable to keep track of the turn
    int i = 0;
    while (boss.health > 0 && userData.userHealth > 0){
        // print a special message if the user decreases the boss health to less than half
        // only print once
        if (boss.health < (initialBossHealth / 2) && !halfHealthMessage){
            cout << boss.halfHealth << endl;
            halfHealthMessage = true;
        }
        // Go back and forth with the enemy
        if(i % 2 == 0){
            cout << "Your turn. ";
            int roundDamage = userData.MinDamage + (rand() % (userData.MaxDamage - userData.MinDamage + 1));
            boss.health -= roundDamage;
            cout << "You deal " << roundDamage << " to " << boss.name << endl;
            cout << "Boss Health: " << boss.health << endl;
        }else{
            // enemy attack - it is possible for them to deal no damage
            int bossDamage = rand() % maxBossDmg;
            userData.userHealth -= bossDamage;
            cout << "The Developer's turn. They deal " << bossDamage << endl;
            if (bossDamage == 0){
                cout  << "They miss!" << endl;
            }
            cout << "Your Health: " << userData.userHealth << endl;
        // if the user dies show some text and end the game
        if(userData.userHealth <= 0){
            cout << "The Developer stairs down upon your motionless body. As your life fades you hear him whisper: \"Subscribe to PewDiePie...\"" << endl;
            exit(0);
        }

            // The user may escape after each turn
            char userEscape;
            cout << "Would you like to run away? (Y)es (N)o" << endl << "> ";
            cin >> userEscape;
            while (true){ 
                if(isValidLetterTwo('y', 'Y', 'n', 'N', userEscape)){ 
                    break; 
                } 
                cout << "Please choose a valid input: " << endl << ">"; 
                cin >> userEscape; 
                // Check if the input is valid using the function 
                // Clear input buffer if the input is invalid 
                cin.clear(); 
                // Clear any error flags 
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                // Discard invalid input 
                } 
                // Process the valid input 
                if (userEscape == 'y' || userEscape == 'Y'){
                return;
                }
            }
        i++;
    }
    char userChoice;
    // if the boss dies
    if(boss.health <= 0){
        // update area info
        currentArea.complete = true;
        currentArea.hasBoss = false;
        cout << boss.defeat << endl;
        // show the two different ending options
        cout << "(A)unt's home or (U)nknown " << endl << "> ";
        cin >> userChoice;
        while(!isValidLetterTwo('a', 'A', 'u', 'U', userChoice)){
            cout << "You're literally at the end, just pick a valid input: " << endl << "> ";
            cin >> userChoice;
        }
        // the two conditions for ending the game
        if(userChoice == 'u' || userChoice == 'U'){
            cout << "You step into the abyss, you don't know where your'e going - but there's bound to be an adventure! " << endl;
            cout << "Ending #2 found!" << endl;
            exit(0);
        // if they want to continue, do not end the game and push them towards the next stage
        }else if(userChoice == 'a' || userChoice == 'A'){
            cout << "You continue along the destined path." << endl;
            return;
        }
    }
    return;
}

vector<char> rightLetters(const Area_t &area)
{
    // build a vector that only holds the letters from the struct for the room
    // the letters are the correct inputs for movement (see area vector)
    vector<char> possibleMoves;
    for(int i = 0; i < area.moves; i++){
        // extract the struct of transition info from the room and one index point (increases each loop)
        transition_t currentTransition = area.transitions.at(i);
        // take out the two letters (lower and uppercase)
        char letter = currentTransition.correctLetter;
        char upperLetter = currentTransition.correctLetterCap;
        // push the two letters into the vector
        possibleMoves.push_back(upperLetter);
        possibleMoves.push_back(letter);
    }
    return possibleMoves;
}

// using the possible places to move, and the user's input, return the value of the next room they're moving to
int findRoomNumber(const vector<transition_t> &rooms, char &userInput)
{
    // go through each room struct
    for(transition_t transitions : rooms){
        // if the user input (letter) exists in a struct, return the number for that move
        if((userInput == transitions.correctLetter)||(userInput == transitions.correctLetterCap)){
            return transitions.roomNum;
        }
    }
    // else return 0 (hopefully this will never happen)
    return 0;
}

int movement(const Area_t &area)
{
    //vector of valid letter inputs
    vector<char> letterMoves = rightLetters(area);

    // the vector of possible movements pulled from the current room
    vector<transition_t> possiblePlaces = area.transitions;

    // display the movement options from the area struct
    char userMove;
    cout << "You may move: ";
    for(unsigned int i = 0; i < possiblePlaces.size(); i++){
        cout << possiblePlaces.at(i).direction << " ";
    }
    cout << endl << "> ";
    // check to see if input is valid by comparing it to the relative correct inputs from the area struct
    bool isFound = false;
    while(!isFound){
        cin >> userMove;

        for(char a : letterMoves){
            if(a == userMove){
                isFound = true;
                break;
            }
        }
        if(!isFound){
                cout << "A thick wall of thorns blocks your path, please choose another direction: " << endl << "> ";
                cin >> userMove;
            }
        }

    int nextRoom = findRoomNumber(possiblePlaces, userMove);
    return nextRoom;
}


int main()
{
    // Title screen
    title();
    howToPlay();
    
    // seed the random generator
    srand(time(nullptr));

    // the map and all that is contained
    vector<Area_t> rooms = makeAreas();
    vector<EnemyData_t> enemies = makeEnemies();
    vector<Trader_t> traders = makeTraders();
    vector<Mathtest_t> mathtest = makeMathTests();
    Boss_t theBoss = boss();

    // user info
    UserData_t userData = userInput();

    // The room loop
    unsigned int currentRoom = 0;
    unsigned int prevRoom = 0;
    while(currentRoom < rooms.size()){
        // a variable to represent the user's current area
        Area_t &room = rooms.at(currentRoom);
        // show the room information
        displayCurrentArea(room);

        // set the trader for the current room
        Trader_t trader;
        if(room.hasTrader){
            if(currentRoom == TRADER1){
                trader = traders.at(0);
            }else if(currentRoom == TRADER2){
                trader = traders.at(1);
            }else if(currentRoom == TRADER3){
                trader = traders.at(2);
            }
        }
        // set the enemy for the current room
        EnemyData_t enemy;
        if(room.hasEnemy){
            if(currentRoom == FIELD){
                enemy = enemies.at(0);
            }else if(currentRoom == WOODS){
                enemy = enemies.at(0);
            }else if(currentRoom == MEADOWS){
                enemy = enemies.at(0);
            }else if(currentRoom == TUNNEL){
                enemy = enemies.at(1);
            }else if(currentRoom == SWAMP){
                enemy = enemies.at(2);
            }else if(currentRoom == QUARRY){
                enemy = enemies.at(1);
            }else if(currentRoom == LAVA_SWAMP){
                enemy = enemies.at(2);
            }else if(currentRoom == TOWNHOUSE){
                enemy = enemies.at(3);
            }
        }
        // set the math test for the current room
        Mathtest_t math;
        if(room.hasMath){
            if(currentRoom == MATH1){
                math = mathtest.at(0);
            }else if(currentRoom == MATH2){
                math = mathtest.at(1);
            }
        }

        // preform actions within the room based on the struct
        if(room.hasEnemy){
            doBattle(room, userData, enemy);
            // if the room still has an enemy, i.e the user ran, send them to the previous room
            if(room.hasEnemy){
                currentRoom = prevRoom;
                continue;
            }
            prevRoom = currentRoom;
            currentRoom = movement(room);
        }else if(room.hasMath){
            doMath(room, userData, math);
            // if the room still has a math test, i.e. the user ran, make the current room equal to the previous room index
            if(room.hasMath){
                currentRoom = prevRoom;
                continue;
            }
                prevRoom = currentRoom;
                currentRoom = movement(room);
        }else if(room.hasTrader){
            doTrade(room, userData, trader);
            prevRoom = currentRoom;
            currentRoom = movement(room);
            // since it does not matter if there is or isn't a trader for movement, do not alter this parameter
        }else if(room.hasBoss){
            bossBattle(room, userData, theBoss);
            // if there is still a boss but the function ends, send them to the last room
            if(room.hasBoss){
                currentRoom = prevRoom;
                continue;
            }
            prevRoom = currentRoom;
            currentRoom = movement(room);
        }else{
            cout << "There is no one around." << endl;
            prevRoom = currentRoom;
            currentRoom = movement(room);
        }
        // if the person makes it to their aunt's home, end the program
        if(currentRoom == END){
            displayCurrentArea(rooms.at(END));
            exit(0);
        }
    }
}
