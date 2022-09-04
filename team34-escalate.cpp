// clang++ -std=c++20 -Wall -Werror -Wextra -Wpedantic -g3 -o team34-escalate team34-escalate.cpp
// run game with: "./team34-escalate 2> debugTapOutput.txt" instead of "./team34-escalate"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

using namespace std;

typedef vector<string> stringvector;

stringvector choiceList = {}; 
stringvector obstacleList = {};

map<int, string> goodChoice;
map<int, string> badChoice;

string gameMode;
string stageName;
string action;
string checkPlayAgain;

bool selectStage = true;
bool chooseAction = true;
bool didPopUp = false;
bool chooseActionObstacle = true;
bool continuePlaying = true;
bool loopToContinuePlayingGame = true;

int totalFloors;
int currentNumberOfTurns = 0;
int popUpIndex;

//constants were initialized for the purposes of indirection and to reduce the opportunity for typos
const string colourSuffix = "\033[0m";
const string darkRedPrefix = "\033[1;31m";
const string redPrefix = "\033[0;31m";
const string greenPrefix = "\033[0;32m";
const string darkGreenPrefix = "\033[1;32m";
const string yellowPrefix = "\033[0;33m";
const string boldYellowPrefix = "\033[1;33m";
const string darkBluePrefix = "\033[1;34m";

const string purpleBackground = "\033[0;45m";

struct Player //used a struct to initialize variables for Player A and Player B at the same time 
{
    int numberOfConsecutiveClimbs;
    int currentFloorNum;
    string name;
};

Player A { .numberOfConsecutiveClimbs = 0, .currentFloorNum = 0 };
Player B { .numberOfConsecutiveClimbs = 0, .currentFloorNum = 0 };

auto instructions() -> void {
    cout << endl;
    cout << purpleBackground << " ______     ______     ______     ______     __         ______     ______   ______    " << colourSuffix << endl;
    cout.flush();
    usleep(300000);
    cout << purpleBackground << "/\\  ___\\   /\\  ___\\   /\\  ___\\   /\\  __ \\   /\\ \\       /\\  __ \\   /\\__  _\\ /\\  ___\\   " << colourSuffix << endl;
    cout.flush();
    usleep(300000);
    cout << purpleBackground << "\\ \\  __\\   \\ \\___  \\  \\ \\ \\____  \\ \\  __ \\  \\ \\ \\____  \\ \\  __ \\  \\/_/\\ \\/ \\ \\  __\\   " << colourSuffix << endl;
    cout.flush();
    usleep(300000);
    cout << purpleBackground << " \\ \\_____\\  \\/\\_____\\  \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_____\\  \\ \\_\\ \\_\\    \\ \\_\\  \\ \\_____\\ " << colourSuffix << endl;
    cout.flush();
    usleep(300000);
    cout << purpleBackground << "  \\/_____/   \\/_____/   \\/_____/   \\/_/\\/_/   \\/_____/   \\/_/\\/_/     \\/_/   \\/_____/ " << colourSuffix << endl;
    cout.flush();
    usleep(300000);
    cout << purpleBackground << "                                                                                      " << colourSuffix << endl;
    cout.flush();
    sleep(1);
    cout << "\nWelcome to Escalate! \n\nIn this two-player game, you will compete against a partner to climb to the top of a building. \nBeware of your stamina levels! The more consecutive climbs you make, the lower your stamina gets. The higher your stamina, the more floors you can climb in one turn. Remember to rest when your stamina is low!\n" << endl;
    sleep(3);
}

auto chooseName( string player ) -> string { //to provide the user with a more personalized experience, the chooseName function allows the player to input their own name to be used while playing

    string playerName;

    cout << "Player " << player << ", please choose a name: ";
    cin >> playerName;

    return playerName;
}

auto gameIntro() -> void {
    while ( selectStage ) { //used a while loop so that the player has another opportunity to choose a game mode if their input was wrong
        try {
            cout << "There are two game modes: Easy or Hard.\nEnter [A] for The Statue of Liberty (EASY) or [B] for The Empire State Building (HARD): ";
            cin >> gameMode;
            if ( ( gameMode == "A" ) or ( gameMode == "a" ) ) {
                totalFloors = 7;
                stageName = "The Statue of Liberty (EASY)";
                selectStage = false;
            }
            else if ( ( gameMode == "B" ) or ( gameMode == "b" ) ) {
                totalFloors = 15;
                stageName = "The Empire State Building (HARD)";
                selectStage = false;
            }
            else {
                throw ( gameMode );
            }
        }
        catch ( ... ) { //returns error message if anything other than A, a, B, or b is inputted
            cout << "Not a valid stage!\n" << endl;
        }
    }
    cout << endl;

    //cerr << "Select a stage Input: " << gameMode << endl;
    //cerr << "Selected game mode: " << stageName << endl;
    //cerr << "Number of Floors in gamemode " << totalFloors << endl;
}

auto staminaBar( int numberOfConsecutiveClimbs ) -> void { //stamina bar is displayed in graphical mode and with different colours to create a more enjoyable user experience

    //cerr << "[StaminaBar Function] Current numberOfConsecutiveClimbs: " << numberOfConsecutiveClimbs << endl;

    string staminaBar;
    int currentStamina = 4 - numberOfConsecutiveClimbs; //there is an inverse relationship between stamina and the current number of consecutive climbs, so we subtracted the current number of consecutive climbs from 4 to ensure the stamina increases from least to greatest instead of greatest to least

    for ( int numberOfIterations = 0; numberOfIterations < currentStamina; numberOfIterations++ ) { //used an index-based for loop to increase the length of the stamina bar depending on the current stamina level for each player
        staminaBar.append( "ooooo" );
    }

    if ( numberOfConsecutiveClimbs == 0 ) {
        cout << darkGreenPrefix << staminaBar << " PERFECT STAMINA " << colourSuffix << endl;
    } 
    else if ( numberOfConsecutiveClimbs == 1 ) {
        cout << greenPrefix << staminaBar << " You're getting a bit tired there..." << colourSuffix << endl;
    } 
    else if ( numberOfConsecutiveClimbs == 2 ) {
        cout << yellowPrefix << staminaBar << " Be careful... " << colourSuffix << endl;
    } 
    else if ( numberOfConsecutiveClimbs == 3 ) {
        cout << redPrefix << staminaBar << " WARNING: Low Stamina " <<  colourSuffix << endl;
    } 
    else if ( numberOfConsecutiveClimbs >= 4 ) {
        cout << darkRedPrefix << "NO MORE STAMINA! SKIP YOUR TURN TO REST" << colourSuffix << endl;
    } 

    //explains to the user that perfect health is the highest they can go (if they press R when they have perfect health, nothing happens)

}

auto staminaLevel( int numberOfConsecutiveClimbs ) -> int { //based on number of consecutive climbs, the function returns how many floors you can climb
    //cerr << "[staminaLevel Function] Current numberOfConsecutiveClimbs: " << numberOfConsecutiveClimbs << endl;

    //map stamina;
    map < string, int > stamina; //used a map to store with string keys to ensure that the reader can easily understand how stamina levels work
    stamina ["Good"] = 3;
    stamina ["Ok"] = 2;
    stamina ["Bad"] = 1;
    stamina ["None"] = 0;

    if ( numberOfConsecutiveClimbs >= 4 ) {
        return stamina["None"];
    }
    if ( numberOfConsecutiveClimbs == 3 ) {
        return stamina["Bad"];
    }
    if ( numberOfConsecutiveClimbs == 2 ) {
        return stamina["Ok"];
    }
    if ( numberOfConsecutiveClimbs == 1 ) {
        return stamina["Good"];
    }
    return 0;
}

auto choicesAndObstacles() -> void { //groups together possible choices to make at certain obstacles, as well as obstacle choices. multiple options were included to improve the interactive experience

    choiceList = {
        "You find a half-empty Gatorade bottle on the ground! The liquid is an eerie yellow-green colour. Do you drink it?\n",
        "There's a note written on the door of this floor! Do you check it?\n",
        "You hear your ringtone go off - your mom is calling your cell phone, but it's all the way at the bottom of your bag! Do you answer it?\n",
        "You see an elderly lady on the stairs, and it looks like she's struggling to get to where she needs to be. Do you help her?\n"
    };

    obstacleList = {
        "Your shoelace is untied! Skip this turn to tie your shoes!",
        "You reaaallly have to use the bathroom... It's getting hard to climb - you're losing stamina!",
        "People are moving a couch! You have to go back down two floors and get out of the way!",
        "You're looking for your phone when you suddenly realize it's not in your pocket! Go down one floor to search for it - you must have dropped it!"
    };

    //created interesting stories to make the user experience more enjoyable

    goodChoice [0] = "The Gatorade was delicious! After that energy drink, you feel super pumped up and ready to go. You sprint up five stairs feeling like Superman!";
    goodChoice [1] = "The note is encouraging, and has a funny meme at the bottom! You feel pleasantly surprised. Go up three flights of stairs!";
    goodChoice [2] = "You find your phone, and when you pick up, your mom tells you that she baked you a cake! Now you're even more excited to get home! Your stamina recharges to max!";
    goodChoice [3] = "She thanks you profusely, and even gives you a five-dollar bill from her purse! Now that you've arrived at the destination, you find yourself two floors higher than before!";

    badChoice [0] = "Now that you've finished the bottle... You don't think it was actually Gatorade - oh goodness. You start to feel a little sick and take a seat on the stairs. Skip this turn to recover.";
    badChoice [1] = "The note is a bad joke. You don't laugh. You feel discouraged and very disappointed in society. You lose stamina."; 
    badChoice [2] = "You start searching for your phone, but by the time you find it, it's not ringing anymore. Oh man, your mom is gonna be super mad at you. Go down two floors out of shame.";
    badChoice [3] = "She wacks you with her purse and accuses you of stereotyping her, insisting that she's capable on her own. You feel defeated and go down a floor to get out of her way.";
}

auto popUp() -> void { 

    chooseActionObstacle = true;
    didPopUp = false; //checks if the pop up occurs or not 

    auto popUp = rand(); //used rand() as opposed to uniform distribution to prevent the game from returning the same integer in each iteration of the program
    //cerr << "[popUp Function] popUp Variable: " << popUp << endl;

    auto obstaclePopUp = rand();
    //cerr << "[popUp Function] obstaclePopUp Variable: " << obstaclePopUp << endl;

    auto goodOrBad = rand();
    //cerr << "[popUp Function] goodOrBad Variable: " << popUp << endl;

    popUpIndex = rand() % 4; // update this number when you add more choices to the choiceList
    //cerr << "[popUp Function] index Variable: " << popUpIndex << endl;

    string response;
    
    if ( popUp % 3 == 0 ) { // chances for getting a popup, implemented a 1 in 3 chance of getting a pop up to make sure that there is a good balance betweeen regular rounds and rounds with choices to make / obstacles to overcome

        if ( obstaclePopUp % 5 == 0 ) { // chances for getting an obstacle (instant repercussion), 20% chance of something bad happening to make the game more interesting while still keeping it a relatively low probability so the player is not frustrated
            cout << obstacleList[popUpIndex] << endl;
            if ( popUpIndex == 1 ) {
                if ( currentNumberOfTurns % 2 == 0 ) {
                    A.numberOfConsecutiveClimbs = A.numberOfConsecutiveClimbs + 1;
                }
                else {
                    B.numberOfConsecutiveClimbs = B.numberOfConsecutiveClimbs + 1;
                }
            }
            else if ( popUpIndex == 2 ) {
                if ( currentNumberOfTurns % 2 == 0 ) {
                    A.currentFloorNum = A.currentFloorNum - 2;
                }
                else {
                    B.currentFloorNum = B.currentFloorNum - 2;
                }
            }
            else if ( popUpIndex == 3 ) {
                if ( currentNumberOfTurns % 2 == 0 ) {
                    A.currentFloorNum = A.currentFloorNum - 1;
                }
                else {
                    B.currentFloorNum = B.currentFloorNum - 1;
                }
            }
            didPopUp = true;
            sleep(2);
        }

        else { // chances for getting something to choose
            cout << choiceList[popUpIndex];

            while ( chooseActionObstacle ) {
                try {
                    cout << "Enter [Y] for YES or [N] for NO: ";
                    cin >> response;
                    //cerr << "[popUp Function] response Variable: " << popUpIndex << endl;

                    if ( ( response == "y" ) or ( response == "Y" ) ) { // if they pick yes
                        if ( goodOrBad % 2 == 0 ) { // 50% chance of a choice going good or bad 
                            cout << goodChoice[popUpIndex] << endl;
                            sleep(2);
                            if ( popUpIndex == 0 ) { // checks the index number and updates the appropriate values based on which good repercussion happened
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.currentFloorNum = A.currentFloorNum + 5; // updates player A if it's player A's turn
                                }
                                else {
                                    B.currentFloorNum = B.currentFloorNum + 5;
                                }
                            }
                            else if ( popUpIndex == 1 ) {
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.currentFloorNum = A.currentFloorNum + 3;
                                }
                                else {
                                    B.currentFloorNum = B.currentFloorNum + 3;
                                }
                            }
                            else if ( popUpIndex == 2 ) {
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.numberOfConsecutiveClimbs = 0;
                                }
                                else {
                                    B.numberOfConsecutiveClimbs = 0;
                                }
                            }
                            else if ( popUpIndex == 3 ) {
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.currentFloorNum = A.currentFloorNum + 2;
                                }
                                else {
                                    B.currentFloorNum = B.currentFloorNum + 2;
                                }
                            }
                        }

                        else { // it didn't go well
                            cout << badChoice[popUpIndex] << endl;
                            sleep(2);
                            if ( popUpIndex == 1 ) {
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.numberOfConsecutiveClimbs = A.numberOfConsecutiveClimbs + 1;
                                }
                                else {
                                    B.numberOfConsecutiveClimbs = B.numberOfConsecutiveClimbs + 1;
                                }
                            }
                            else if ( popUpIndex == 2 ) {
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.currentFloorNum = A.currentFloorNum - 2;
                                }
                                else {
                                    B.currentFloorNum = B.currentFloorNum - 2;
                                }
                            }
                            else if ( popUpIndex == 3 ) {
                                if ( currentNumberOfTurns % 2 == 0 ) {
                                    A.currentFloorNum = A.currentFloorNum - 1;
                                }
                                else {
                                    B.currentFloorNum = B.currentFloorNum - 1;
                                }
                            }
                        }
                        chooseActionObstacle = false;
                        didPopUp = true;
                    }

                    else if ( ( response == "n" ) or ( response == "N" ) ) { // if they pick no and ignore the pop up, the game continues as normal
                        cout << "BO-RING! You chose not to bother." << endl;
                        chooseActionObstacle = false;
                        didPopUp = false;
                    }

                    else {
                        throw ( response );
                    }
                }
                catch ( ... ) { //if anything other than y, Y, n, or N is inputted, the try-catch statement outputs an error message
                    cout << "Not a valid choice!" << endl;
                }
            }
        }
    }
}

auto playerA() -> void { //for the next iteration of our game, we could combine playerA() and playerB() into one function. currently, two functions are used to increase code readability as it makes it easier to understand how the player statistics work
    cout << "--------------------------------------" << endl;
    cout << darkBluePrefix << A.name << "'s turn" << colourSuffix << endl;
    cout << "Stage: " << stageName << "\tCurrent floor: " << A.currentFloorNum << "\tFloors to go: " << ( totalFloors - A.currentFloorNum ) << endl;
}

auto playerB() -> void {
    cout << "--------------------------------------" << endl;
    cout << boldYellowPrefix << B.name << "'s turn" << colourSuffix << endl;
    cout << "Stage: " << stageName << "\tCurrent floor: " << B.currentFloorNum << "\tFloors to go: " << ( totalFloors - B.currentFloorNum ) << endl;
}

auto runGame() -> void {

    while ( continuePlaying ) {
        //cerr << "[runGame Function] turn: " << popUpIndex << endl;
        //cerr << "[runGame Function] Player A numberOfConsecutiveClimbs Variable: " << A.numberOfConsecutiveClimbs << endl;
        //cerr << "[runGame Function] Player B numberOfConsecutiveClimbs Variable: " << B.numberOfConsecutiveClimbs << endl;
        if ( currentNumberOfTurns % 2 == 0 ) { //asks player A
            
            if ( A.numberOfConsecutiveClimbs >= 4 ) { // if the current number of consecutive climbs is greater than or equal to four, the player is out of stamina and their turn is skipped (so they don't receive the option to climb or rest)
                playerA();
                staminaBar ( A.numberOfConsecutiveClimbs );
                A.numberOfConsecutiveClimbs = A.numberOfConsecutiveClimbs - 2;
                sleep(2);
            }
            else {
                playerA();
                cout << "Stamina: "  << endl; 
                staminaBar ( A.numberOfConsecutiveClimbs );
                
                if ( A.currentFloorNum >= 2 ) { //note: this must be out of the while loop so pop up cannot occur between wrong inputs
                        popUp(); // pop up can only occur if the current floor is greater than or equal to two to ensure that the floor count is not negative
                    }

                while ( chooseAction )
                {
                    if ( didPopUp == true ) {
                        chooseAction = false;
                    }
                    else {
                        try {
                            cout << "Do you want to climb or rest? Enter [C] to climb and [R] to rest: ";
                            cin >> action;
                            if ( ( action == "C" ) or ( action == "c" ) or ( action == "Climb" ) or ( action == "climb" ) ) { //accounts for multiple input possibilities to facilitate a better user experience 
                                A.numberOfConsecutiveClimbs = A.numberOfConsecutiveClimbs + 1; // inputs climb streak
                                A.currentFloorNum = A.currentFloorNum + staminaLevel(A.numberOfConsecutiveClimbs); // updates current floor
                                chooseAction = false;
                            }
                            else if ( ( action == "R" ) or ( action == "r" ) or ( action == "Rest" ) or ( action == "rest" ) ) {
                                if ( A.numberOfConsecutiveClimbs == 0 ) {
                                    cout << "Silly Goose! You already had perfect stamina!" << endl;
                                    sleep(2);
                                }
                                else if ( A.numberOfConsecutiveClimbs < 2 ) { //so stamina does not go into negative value after resting
                                    A.numberOfConsecutiveClimbs = 0;
                                }
                                else {
                                    A.numberOfConsecutiveClimbs = A.numberOfConsecutiveClimbs - 2;
                                }
                                chooseAction = false;
                            }
                            else {
                                throw ( action );
                            }
                        }
                        catch ( ... ) {
                            cout << "Not a valid input!" << endl;
                        }
                    }
                }
            }
            chooseAction = true;
            currentNumberOfTurns = currentNumberOfTurns + 1; // used +1 as opposed to ++ to use modern C++ practices 
        }
        
        else { //asks player B. in the next iteration, we would create one function for both players by passing parameters 
        
            if ( B.numberOfConsecutiveClimbs >= 4 ) {
                playerB();
                staminaBar ( B.numberOfConsecutiveClimbs );
                B.numberOfConsecutiveClimbs = B.numberOfConsecutiveClimbs - 2;
                sleep(2);
            }
            else {
                playerB();
                cout << "Stamina: "  << endl; 
                staminaBar ( B.numberOfConsecutiveClimbs );
                
                if ( B.currentFloorNum >= 2 ){
                        popUp();
                    }

                while ( chooseAction ){
                    if ( didPopUp == true ) {
                        chooseAction = false;
                    }
                    else {
                        try {
                            cout << "Do you want to climb or rest? Enter [C] to climb and [R] to rest: ";
                            cin >> action;
                            if ( ( action == "C" ) or ( action == "c" ) or ( action == "Climb" ) or ( action == "climb" ) ) { //better user experience
                                B.numberOfConsecutiveClimbs = B.numberOfConsecutiveClimbs + 1; // inputs climb streak
                                B.currentFloorNum = B.currentFloorNum + staminaLevel(B.numberOfConsecutiveClimbs); // updates current floor
                                chooseAction = false;
                            }
                            else if ( ( action == "R" ) or ( action == "r" ) or ( action == "Rest" ) or ( action == "rest" ) ) {
                                if ( B.numberOfConsecutiveClimbs == 0 ) {
                                    cout << "Silly Goose! You already had perfect stamina!" << endl;
                                    sleep(2);
                                }
                                else if ( B.numberOfConsecutiveClimbs < 2 ) {
                                    B.numberOfConsecutiveClimbs = 0;
                                }
                                else {
                                    B.numberOfConsecutiveClimbs = B.numberOfConsecutiveClimbs - 2;
                                }
                                chooseAction = false; //false because it breaks out of the while loop that prompts the user to pick an action, as an action has successfully been picked
                            }
                            else {
                                throw ( action );
                            }
                        }
                        catch ( ... ) {
                            cout << "Not a valid input!" << endl;
                        }
                    }
                }
            }
            chooseAction = true;
            currentNumberOfTurns = currentNumberOfTurns + 1;
        }

        if ( ( A.currentFloorNum >= totalFloors ) or ( B.currentFloorNum >= totalFloors ) ) { //checks if a player has won
            continuePlaying = false;
        }
        //cerr << "[runGame Function] continue playing state: " << continuePlaying << endl;

    }
}

auto congratsMessage() -> void { //emphasizes that a player has won, and generates user excitement
    cout << darkRedPrefix << "--------------------------------------" << endl;
    cout << darkBluePrefix << " _______  _______  __    _  _______  ______    _______  _______  _______    __  " << colourSuffix << endl;
    cout << darkGreenPrefix << "|       ||       ||  |  | ||       ||    _ |  |   _   ||       ||       |  |  | " << colourSuffix << endl;
    cout << darkBluePrefix << "|       ||   _   ||   |_| ||    ___||   | ||  |  |_|  ||_     _||  _____|  |  | " << colourSuffix << endl;
    cout << darkGreenPrefix << "|       ||  | |  ||       ||   | __ |   |_||_ |       |  |   |  | |_____   |  | " << colourSuffix << endl;
    cout << darkBluePrefix << "|      _||  |_|  ||  _    ||   ||  ||    __  ||       |  |   |  |_____  |  |__| " << colourSuffix << endl;
    cout << darkGreenPrefix << "|     |_ |       || | |   ||   |_| ||   |  | ||   _   |  |   |   _____| |   __  " << colourSuffix << endl;
    cout << darkBluePrefix << "|_______||_______||_|  |__||_______||___|  |_||__| |__|  |___|  |_______|  |__| \n" << colourSuffix << endl;
}

auto playAgain() -> void { 
    bool checking = true;
    while ( checking ) {
        try {
            cout << "Enter [Y] for YES or [N] for NO: ";
            cin >> checkPlayAgain;
            //cerr << "[PlayAgain Function] checkPlayAgain Variable: " << checkPlayAgain << endl;

            if ( ( checkPlayAgain == "y" ) or ( checkPlayAgain == "Y" ) ) {
                cout << "Great! Reloading the game..." << endl; //let users know that we are reloading the game to improve user experience
                sleep(2);
                system("clear");
                loopToContinuePlayingGame = true;
                checking = false;
                //cerr << "[PlayAgain Function] loopToContinuePlayingGame Variable: " << loopToContinuePlayingGame << endl;
            
            }
            else if ( ( checkPlayAgain == "n" ) or ( checkPlayAgain == "N" ) ) {
                cout << "Thanks for playing! See you next time!" << endl; //facilitates a more positive user experience 
                loopToContinuePlayingGame = false;
                checking = false;
                //cerr << "[PlayAgain Function] loopToContinuePlayingGame Variable: " << loopToContinuePlayingGame << endl;
            
            }
            else {
                throw ( checking );
            }
        }
        catch ( ... ) {
            cout << "Not a valid response!"<< endl;
        }
    }
    //cerr << "[PlayAgain Function] checking: " << checking << endl;

}

auto winner() -> void {
    if ( A.currentFloorNum >= totalFloors ) {
        cout << A.name << " has won Escalate!\nDo you want to play again?" << endl;
        playAgain();
    }
    else if ( B.currentFloorNum >= totalFloors ) {
        cout << B.name << " has won Escalate!\nDo you want to play again?" << endl;
        playAgain();
    }
}

auto resetGame() -> void { //resets variables to initial states for new game
    selectStage = true;
    chooseAction = true;
    continuePlaying = true;
    currentNumberOfTurns = 0;
    A.currentFloorNum = 0;
    B.currentFloorNum = 0;
    A.numberOfConsecutiveClimbs = 0;
    B.numberOfConsecutiveClimbs = 0;
}

auto main() -> int { 

    srand(time(0)); //initalizes rand(). used time to ensure that each generate value is unique while the function is being run
    choicesAndObstacles();

    while ( loopToContinuePlayingGame ) {
        instructions();
        gameIntro();
        A.name = chooseName("A");
        B.name = chooseName("B");

        //cerr << "Player A name: " << A.name << "\tPlayer B name: " << B.name << endl;

        sleep(2);
        system("clear");
        runGame();
        congratsMessage();
        winner();
        resetGame();
    }
}
