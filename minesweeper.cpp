/* Minesweeper in terminal */
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif
#include <queue>
#include <cctype>
using namespace std;

// Key reading
#if defined(_WIN32) || defined(_WIN64)
    #include <conio.h>
    void initTerminal(){}
    void resetTerminal(){}
    char getKey(){
        return _getch();
    }

    // Windows key codes
    const int KEY_UP = 72;
    const int KEY_DOWN = 80;
    const int KEY_ENTER = 13;
    const int KEY_LEFT = 75;
    const int KEY_RIGHT = 77;
    const int KEY_d = 'd';
    const int KEY_f = 'f';
#endif

//Color codes for terminal for Linux, MacOS, and modern Windows
#define RESET "\033[0m"
#define WHITE "\033[37m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

//Function to clear terminal screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void waitForEnter() {
    cout << "Press ENTER to return to the main menu." << "\033[K\n";
    while (getKey() != KEY_ENTER);
}

int difficulty = 0;

void minesweeper() {
    srand(time(0));

    cout << "\033[?25l";
    cout << "\033[H";
    #ifdef _WIN32 Sleep(1000); #endif;

    int rows = 9;
    int cols = 9;
    int numOfBombs = 10;

    switch (difficulty) {
        case 0:
            break;
        case 1:
            rows = 16;
            cols = 16;
            numOfBombs = 40;
            break;
        case 2:
            rows = 16;
            cols = 30;
            numOfBombs = 99;
            break;
    }

    int total = rows * cols;

    // Initialize the board with total spaces
    vector<int> spaceValues(total, 0);

    // Track revealed spaces
    vector<bool> revealed(total, false);

    // Track flagged spaces
    vector<bool> flagged(total, false);


    bool playingMinesweeper = true;
    bool playerWon = false;
    bool boardInitialized = false;

    int numOfFlags = 0;
    int currentSpace = 0;
    int remainingSpaces = total - numOfBombs;
    int remainingBombs = numOfBombs;
    string message = "";

    while(playingMinesweeper == true) {
        cout << "\033[H";

        int spacesInRow = 0;
        cout << "Spaces Remaining: " << remainingSpaces << "\033[K\n";
        cout << " Bombs Remaining: " << remainingBombs << "\033[K\n";

                
        for (int i = 0; i < total; i++) {
            // Displays the board with set spaces in a row and colors for the current space and revealed spaces
            string displayValue = revealed[i] ? to_string(spaceValues[i]) : "O";

            if (i == currentSpace) {
                if (flagged[i]) {
                    cout << YELLOW << "F" << RESET;
                } else
                    cout << YELLOW << displayValue << RESET;
            } else if (revealed[i]) {
                cout << GREEN << displayValue << RESET;
            } else if (flagged[i]) {
                cout << RED << "F" << RESET;
            } else {
                cout << displayValue;
            }

            if (i % cols == (cols - 1)) {
                cout << "\033[K\n";
            } else {
                cout << "  ";
            }
        }
        cout << "Arrow keys to move. D or Enter to click. F to place flag." << "\033[K\n";
        cout << message << "\033[J\n";
        cout << flush;

        int key = getKey();
    
        // Handle Windows Arrow Key Escape Codes
        #if defined(_WIN32) || defined(_WIN64)
        if (key == 0 || key == 224) { // Arrow keys prefix
            key = getKey(); // Get the actual arrow key code
        }
        #endif

        if (key == 'D' || key == 'F') {
            key = tolower(key); // Lowercases capital D and F
        }

        if (key == KEY_UP){
            if (currentSpace > (cols - 1)) {
                currentSpace -= cols;
                message = "";
            } else {
                message = "Illegal Move";
            }
        }
        else if (key == KEY_DOWN) {
            if (currentSpace < (total - cols)) {
                currentSpace += cols;
                message = "";
            } else {
                message = "Illegal Move";
            }
        }
        else if (key == KEY_LEFT) {
            if (currentSpace % cols != 0) {
                currentSpace -= 1;
                message = "";
            } else {
                message = "Illegal Move";
            }
        }
        else if (key == KEY_RIGHT) {
            if (currentSpace % cols != (cols - 1)) {
                currentSpace += 1;
                message = "";
            } else {
                message = "Illegal Move";
            }
        }
        else if (key == KEY_ENTER || key == KEY_d) {
            //Generate the board on the first move
            if (!boardInitialized) {
                //Reset all spaces
                fill(spaceValues.begin(), spaceValues.end(), 0);
                int bombsPlaced = 0;
                int clickRow = currentSpace / cols;
                int clickCol = currentSpace % cols;

                while (bombsPlaced < numOfBombs) {
                    int bomb = rand() % total;

                    // Prevent duplicate bombs
                    if (spaceValues[bomb] == 100)
                        continue;
                    
                    int bombRow = bomb / cols;
                    int bombCol = bomb % cols;

                    // Check if bombs is within 1 space of the first move
                    if (abs(bombRow - clickRow) <= 1 && abs(bombCol - clickCol) <= 1) 
                        continue;

                    spaceValues[bomb] = 100; // Place a bomb
                    bombsPlaced++;
                }

                // Calculate surrounding numbers
                for (int i = 0; i < total; i++) {
                    if (spaceValues[i] == 100) {

                        int row = i / cols;
                        int col = i % cols;

                        // Check all 8 surrounding spaces
                        for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                            for (int colOffset = -1; colOffset <= 1; colOffset++) {
                                if (rowOffset == 0 && colOffset == 0)
                                    continue; // Skip the current space

                                int newRow = row + rowOffset;
                                int newCol = col + colOffset;

                                // Check boundaries
                                if (newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols)
                                    continue;

                                int adjacent = newRow * cols + newCol;
                                if (spaceValues[adjacent] != 100) {
                                    spaceValues[adjacent]++; // Increment the count of surrounding bombs
                                }
                            }
                        }
                    }
                }
                
                boardInitialized = true;
            }

            // Check if the selected space is a bomb
            if (spaceValues[currentSpace] >= 100) {
                cout << "\033[H";
                playingMinesweeper = false;
                spacesInRow = 0;
                for (int i = 0; i < total; i++) {
                    if (spacesInRow != (cols - 1)) {
                        if (spaceValues[i] >= 100) {
                            cout << RED << "B" << RESET << "  ";
                        } else {
                            cout << GREEN << spaceValues[i] << RESET << "  ";
                        }
                        spacesInRow += 1;
                    }
                    else if (spacesInRow == (cols - 1)) {
                        if (spaceValues[i] >= 100) {
                            cout << RED << "B" << RESET << "\033[K\n";
                        } else {
                            cout << GREEN << spaceValues[i] << RESET << "\033[J\n";
                        }
                        spacesInRow = 0;
                    }
                }

            cout << flush;
            cout << "\033[?25h";
            waitForEnter();

            } else {
                // Don't process already revealed spaces
                if (revealed[currentSpace] == true)
                    continue;

                // Don't process flagged spaces
                if (flagged[currentSpace] == true)
                    continue;

                // Reveal the selected space
                revealed[currentSpace] = true;

                // Only perform flood fill if the selected space is 0
                if (spaceValues.at(currentSpace) == 0) {
                    queue<int> revealQueue;

                    revealQueue.push(currentSpace);
                    revealed[currentSpace] = true;

                    while (!revealQueue.empty()) {
                        int current = revealQueue.front();
                        revealQueue.pop();

                        int row = current / cols;
                        int col = current % cols;

                        // Check all 8 surrounding spaces
                        for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                            for (int colOffset = -1; colOffset <= 1; colOffset++) {
                                if (rowOffset == 0 && colOffset == 0)
                                    continue; // Skip the current space

                                int newRow = row + rowOffset;
                                int newCol = col + colOffset;

                                // Check boundaries
                                if (newRow < 0|| newRow >= rows || newCol < 0 || newCol >= cols)
                                    continue;
                                
                                int adjacent = newRow * cols + newCol;

                                //Skip already proccessed spaces
                                if (revealed[adjacent])
                                    continue;

                                // Skip bombs
                                if (spaceValues.at(adjacent) >= 100)
                                    continue;

                                // Reveal the adjacent space and remove flags
                                revealed[adjacent] = true;
                                if (flagged[adjacent] == true) {
                                    flagged[adjacent] = false;
                                    remainingBombs++;
                                    numOfFlags--;
                                }

                                // Continue revealing if the space is empty
                                if (spaceValues.at(adjacent) == 0) {
                                    revealQueue.push(adjacent);
                                }
                            }
                        }
                    }
                }
                                
                //Check for victory
                int revealedCount = 0;

                for (int i = 0; i < total; i++) {
                    if (revealed[i] == true && spaceValues[i] < total) {
                        revealedCount++;
                    }
                }

                remainingSpaces = total - numOfBombs - revealedCount;

                // Player wins when all non-bomb spaces are revealed
                if (revealedCount == total - numOfBombs) {
                    playerWon = true;
                    playingMinesweeper = false;

                    cout << GREEN << "=============================" << RESET << "\033[K\n";
                    cout << GREEN << "           YOU WIN!          " << RESET << "\033[K\n";
                    cout << GREEN << "=============================" << RESET << "\033[K\n";

                    cout << "\033[K\n";
                    cout << "Congrats! You've successfully cleared the minefield!" << "\033[K\n";
                    cout << "\033[J\n";

                    cout << "\033[?25h";
                    waitForEnter();
                }
            }
        }
        else if (key == KEY_f) {
            if (revealed[currentSpace] == false) {
                if (numOfFlags >= numOfBombs) {
                    if (flagged[currentSpace] == true) {
                        flagged[currentSpace] = false;
                        numOfFlags--;
                        remainingBombs++;
                    } else {
                        message = "Too many flags placed.";
                    }
                } else {
                    if (flagged[currentSpace] == false) {
                        numOfFlags++;
                        remainingBombs--;
                    } else {
                        numOfFlags--;
                        remainingBombs++;
                    }
                    flagged[currentSpace] = !flagged[currentSpace];
                }
            }
        }
    }
}

int main() {
    clearScreen();
    initTerminal();

    vector<string> options = {
        "          PLAY          ",
        "        SETTINGS        ",
        "          EXIT          "
    };

    vector<string> settingsOptions = {
        "   DIFFICULTY: [EASY]   ",
        "         BACK           "
    };

    int currentSelection = 0;
    bool running = true;

    while (running){
        cout << "\033[H";

        // Display menu 
        cout << "============================" << "\033[K\n";
        cout << "            MENU            " << "\033[K\n";
        cout << "============================" << "\033[K\n";

        // Render buttons
        for (size_t i = 0; i < options.size(); ++i) {
            if (i == currentSelection) {
                cout << "> " << options[i] << " <" << "\033[K\n";
            } else {
                cout << "  " << options[i] << "\033[K\n";
            }
        }
        cout << "============================" << "\033[K\n";

        #if defined(_WIN32) || defined(_WIN64)
            cout << "\033[K\n";
            cout << " Use Arrow Keys to navigate and ENTER to select." << "\033[J\n";
        #endif

        cout << flush;

        // Read user input
        int key = getKey();

        // Handle Windows Arrow Key Escape Codes
        #if defined(_WIN32) || defined(_WIN64)
            if (key == 0 || key == 224) { // Arrow keys prefix
                key = getKey(); // Get the actual arrow key code
            }
        #endif

        // Navigation logic
        if (key == KEY_UP) {
            currentSelection = (currentSelection - 1 + options.size()) % options.size();
        }
        else if (key == KEY_DOWN) {
            currentSelection = (currentSelection + 1) % options.size();
        }
        else if (key == KEY_ENTER) {
            if (currentSelection == 0) {
                    // Start Game
                    cout << "\033[H";
                    cout << "=== STARTING MINESWEEPER ===" << "\033[J\n";
                    #ifdef _WIN32 Sleep(1000); #endif; // Simulate game loading
                    minesweeper();
            }
            else if (currentSelection == 1) {
                currentSelection = 0;
                while (true) {
                    // Settings
                    cout << "\033[H";

                    cout << "========= SETTINGS =========" << "\033[K\n";

                    // Settings button rendering
                    for (size_t i = 0; i < settingsOptions.size(); ++i) {
                        if (i == currentSelection) {
                            cout << "> " << settingsOptions[i] << " <" << "\033[K\n";
                        } else {
                            cout << "  " << settingsOptions[i] << "\033[K\n";
                        }
                    }
                    cout << "============================" << "\033[J\n";
                    
                    cout << flush;

                    key = getKey();

                    // Handle Windows Arrow Key Escape Codes
                    #if defined(_WIN32) || defined(_WIN64)
                        if (key == 0 || key == 224) { // Arrow keys prefix
                            key = getKey(); // Get the actual arrow key code
                        }
                    #endif

                    if (key == KEY_UP) {
                        currentSelection = (currentSelection - 1 + settingsOptions.size()) % settingsOptions.size();
                    } else if (key == KEY_DOWN) {
                        currentSelection = (currentSelection + 1) % settingsOptions.size();
                    } else if (key == KEY_ENTER) {
                        if (currentSelection == 0) {
                            switch (difficulty) {
                                case 0: 
                                    settingsOptions.at(0) = "   DIFFICULTY: [MEDIUM]";
                                    difficulty++;
                                    break;
                                case 1: 
                                    settingsOptions.at(0) = "   DIFFICULTY: [HARD]  ";
                                    difficulty++;
                                    break;
                                case 2: 
                                    settingsOptions.at(0) = "   DIFFICULTY: [EASY]  ";
                                    difficulty = 0;
                                    break;
                            }
                        }
                        else if (currentSelection == 1) {
                            break;
                        }
                    }
                }
            }
            else if (currentSelection == 2) {
                // Exit
                running = false;
            }
        }
    }

    cout << "\033[H";
    cout << "Exiting Minesweeper. Goodbye!" << "\033[J\n";
    resetTerminal();
    return 0;
}
