#include <iostream>
#include <sstream>

#include "Sudoku.cpp"

#define functionArrayLength 9

char transfer(const char* (&functions)[functionArrayLength], std::string &input);
std::string stringToLower(std::string input);
std::string userInput(std::string input, std::string infoText, int numberOfOutputs, std::string output = "000");
void help();

//function calls
//insert remove new-game fill help exit
//insert(numb, Xi, Yi)
//debug(Xi, Yi)
int main() {
    Sudoku currentGame;
    const char* functions[functionArrayLength] = {"close", "restart", "insert", "fill", "solve", "logic", "debug", "print", "help"};
    std::string input;

    while (true)
    {
        //comand info
        for (std::string command : functions)
        {
            std::cout << '[' << command[0] << ']' << command.substr(1) << ' ';
        }
        std::cout << std::endl;

        //call transfer and direct the user acording to the return value
        //posibale return values are function[i][0]
        //all bad return valuse go to case 'b' / default
        switch (transfer(functions, input))
        {
        case 'c':
            return 0;
        case 'r':
            currentGame.Restart();
            break;
        I:
            std::cout << "error message" << std::endl;
        case 'i':
            input = userInput(input, "input the number you want to insert, X cord, Y cord", 2);
            
            if (input[0] == 'a')
                goto I;

            currentGame.insert(input[0] - '0', input[1] - '1', input[2] - '1');

            break;
        //allowes the user to fill in the inteir game at once
        case 'f':
            std::cout << "/////// ---------\n";
            for (int i = 0; i < 9; i++)
            {
                std::cout << "line " << (i + 1) << ": ";
                getline(std::cin, input);
                
                for (int j = 0; j < 9; j++)
                {
                    if (input[j] - '0' <= 0 || input[j] - '9' > 0)
                        continue;
                    
                    currentGame.insert(input[j] - '0', j, i, false);
                    input[j] = '0';
                }
                input = "";
            }
            
            break;
        case 's':
            currentGame.solve();
            break;
        case 'l':
            currentGame.logic();
            break;
        //prints debug info about chosen tile
        case 'd':
            if (input.empty())
            {
                currentGame.debug();
                break;
            }

            input = userInput(input, "input X cord, Y cord", 2);

            if (input[0] == 'a')
                break;
            if (input[2] == '0')
            {
                currentGame.debug(input[0] - '1', input[1] - '1');
            } else {
                currentGame.debug(input[0] - '1', input[1] - '1', input[2] - '0');
            }

            break;
        //Prints the current state of the game
        case 'p':
            currentGame.print();
            break;
        case 'h':
            help();
            break;
        //Bad input
        case 'b':
        default:
            break;
        }
    }
    
    return 0;
}

char transfer(const char* (&functions)[functionArrayLength], std::string &input) {
    getline(std::cin, input);
    input = stringToLower(input);
    std::stringstream ss(input);

    if (ss.good())
    {
        ss >> input;
        for (std::string token : functions)
        {
            if (input == token || input.length() == 1 && input[0] == token[0])
            {
                input = ss.str();
                do
                {
                    input.erase(0, 1);
                } while (input[0] == token[0]);

                while (input.length() > 0 && input[0] == ' ')
                {
                    input.erase(0, 1);
                }

                return token[0];
            }
        }
    }
    //return b for Bad input
    return 'b';
}

/// @brief copyde code from a friend
/// @param input user input
/// @return ABCD123 -> abcd123
std::string stringToLower(std::string input) {
	for (int i = 0; i < input.length(); i++) {
		input[i] = tolower(input[i]);
	}
	return input;
}

std::string userInput(std::string input, std::string infoText, int maxNumberOfOutputs, std::string output) {
    while (input.empty())
    {
        std::cout << infoText << std::endl;
        getline(std::cin, input);
    }

    //sets upp the inpus for the loop
    if (input.back() != ' ')
        input += ' ';
    
    for (int i = 0; !input.empty() && output[maxNumberOfOutputs] != 0;)
    {
        if (input.length() <= 1 || input.find(' ') != 1 || input[0] - '0' <= 0 || input[0] - '9' > 0)
        {
            if (input.length() >= 1)
            {
                input.erase(0, 1);
                continue;
            }

            output[0] = 'a';
            return output;
        }

        output[i] = input[0];
        i++;

        input.erase(0, 2);
    }

    if (!input.empty())
    {
        std::cout << "input overload" << std::endl;

        output[0] = 'a';
        return output;
    }
    return output;
}

void help() {
    /* info */
    std::cout << "\n/*-----Close-----*/\n";
    std::cout << "  Ends the programm and closes the window\n";
    std::cout << "\n/*-----Insert-----*/\n";
    std::cout << "  3 overloads: int int int, ex: insert 7 1 4\n";
    std::cout << "  The first number is the number you want to insert\n";
    std::cout << "  The secound number is the x-cord for that number (1 - 9)\n";
    std::cout << "  The third number is the y-cord for that number (1 - 9)\n";
    std::cout << "\n/*-----Fill-----*/\n";
    std::cout << "  9 overloads: int\n";
    std::cout << "  When this command is called it takes the next 9 inputs and\n";
    std::cout << "  converts them into numbers and inserts them into the gameboard.\n";
    std::cout << "  Each input chooed be length 9 and use the numbers 0 - 9. \n";
    std::cout << "\n/*-----Solve-----*/\n";
    std::cout << "  0 overloads; loops through all tiles\n";
    std::cout << "  and checks inserts any forced numbers\n";
    std::cout << "\n/*-----Logic-----*/\n";
    std::cout << "/*-----Weak and Broken-----*/\n";
    std::cout << "\n/*-----Debug-----*/\n";
    std::cout << "  0 overloads: Outputs a list of all tiles, theire number and flags i: n | f\n";
    std::cout << "  2 overloads: int int, ex: debug 4 7\n";
    std::cout << "  runs debug 3 overloads: debug 4 7 1\n";
    std::cout << "  3 overloads: int int int\n";
    std::cout << "  The first number is the x-cord for that number (1 - 9)\n";
    std::cout << "  The secound number is the y-cord for that number (1 - 9)\n";
    std::cout << "  The third number the number you want informatin about (1 - 9)\n";
    std::cout << "\n/*-----Print-----*/\n";
    std::cout << "  0 overloads: Prints a humen readebl vertion of the gameboard.\n";
    std::cout << "  The output dose not contain debuginfo\n\n";
}