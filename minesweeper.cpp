#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <set>
#include <string>
#include <utility>
#include <queue>
#include <map>

const char MINE = 'X';
const char SPACE = '.';
const char UNCOVERED = '-';
const char FLAG = '<';
const char ERROR = '!';

int charToInt(char c) {
    int asciiNum = (int)c;
    if (48 <= asciiNum && asciiNum <= 57)
        return asciiNum-48;
    if (65 <= asciiNum && asciiNum <= 90)
        return asciiNum-55;
    if (97 <= asciiNum && asciiNum <= 122)
        return asciiNum-61;
    return (int) ERROR;
}

char intToChar(int i) {
    if (i < 10)
        return (char) (i+48);
    if (10 <= i && i <= 35)
        return (char) (i+55);
    if (36 <= i && i <= 61)
        return (char) (i+61);
    return ERROR; 
}

int randomNumber(int incStart, int incEnd) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 engine(seed);
    std::uniform_int_distribution<int> dist(incStart, incEnd);
    int number = dist(engine);
    return number; 
}

void clearScreen() {
    std::cout << "\033[2J\033[H";
}

void printBoard(std::vector<std::vector<char>> &grid) {
    std::cout << "   ";
    for (int i = 0; i < grid[0].size(); i++) {
        std::cout << intToChar(i) << " ";
    }
    std::cout << '\n';
    std::cout << "   _";
    for (int i = 0; i < grid[0].size()-1; i++) {
        std::cout << "__";
    }
    std::cout << '\n';

    std::map<char, std::string> ansi = {{'1', "94"}, {'2', "32"}, {'3', "31"}, {'4', "34"}, {'5', "35"}, {'6', "36"}, {'7', "32"}, {'8', "90"}, {FLAG, "33"}, {MINE, "93"}};
    for (int i = 0; i < grid.size(); i++) {
        std::cout << intToChar(i) << " |";
        for (int j = 0; j < grid[0].size(); j++) {
            if (ansi.find(grid[i][j]) == ansi.end())
                std::cout << grid[i][j] << " ";
            else
                std::cout << "\033[" + ansi[grid[i][j]] + "m" + grid[i][j] + " \033[0m";
        }
        std::cout << '\n';
    }
}

std::string padZeroes(std::string s, int length) {
    while (s.size() != length)
        s = '0' + s;
    return s;
}

int getNeighbor(std::vector<std::vector<char>> &grid, int row, int col) {
    int neighbors = 0;
    std::vector<int> dirX = {-1, -1, -1, 0, 0, 1, 1, 1};
    std::vector<int> dirY = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int i = 0; i < 8; i++) {
        int newX = dirX[i] + row;
        int newY = dirY[i] + col;
        if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size())
            continue;
        neighbors += (grid[newX][newY] == MINE);
    }
    return neighbors;
}

bool isValid(std::vector<std::vector<char>> &grid, int row, int col) {
    if (grid[row][col] != MINE)
        return true;
    std::vector<int> dirX = {-1, -1, -1, 0, 0, 1, 1, 1};
    std::vector<int> dirY = {-1, 0, 1, -1, 1, -1, 0, 1};
    bool free = false;
    for (int i = 0; i < 8; i++) {
        int newX = dirX[i] + row;
        int newY = dirY[i] + col;
        if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size())
            continue;
        free = (free || grid[newX][newY] != MINE);
    }
    return free;
}

void placeMines(std::vector<std::vector<char>> &grid, int mines) {
    int rows = grid.size();
    int cols = grid[0].size();
    grid.assign(rows, std::vector<char>(cols, SPACE)); 
    std::set<std::string> positions;
    while (mines--) {
        int r, c;
        std::string compressed;
        do {
            r = randomNumber(0, rows-1);
            c = randomNumber(0, cols-1);
            std::string sr = std::to_string(r);
            std:: string sc = std::to_string(c);
            compressed = padZeroes(sr, sr.size()) + padZeroes(sc, sc.size());
        } while (positions.find(compressed) != positions.end());
        positions.insert(compressed);
    }

    for (const std::string &s : positions) {
        int r = std::stoi(s.substr(0, s.size()/2));
        int c = std::stoi(s.substr(s.size()/2, s.size()/2));
        grid[r][c] = MINE;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!isValid(grid, i, j)) {
                placeMines(grid, mines);
                return;
            }
        }
    }
}

void placeNumbers(std::vector<std::vector<char>> &grid) {
    int rows = grid.size();
    int cols = grid[0].size();

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == MINE)
                continue;
            grid[i][j] = getNeighbor(grid, i, j) + '0';
            if (grid[i][j] == '0')
                grid[i][j] = SPACE; 
        }
    }
}

bool flagCell(std::vector<std::vector<char>> &grid, int row, int col) {
    clearScreen();
    grid[row][col] = FLAG;
    printBoard(grid);
    return true;
}

bool gameWon(std::vector<std::vector<char>> grid, std::vector<std::vector<char>> answers) {
    int bombGrid = 0;
    int bombAnswer = 0;
    for (int i = 0; i < answers.size(); i++) {
        for (int j = 0; j < answers[0].size(); j++) {
            if (answers[i][j] == MINE && grid[i][j] != FLAG) {
                return false;
            }
            bombGrid += (grid[i][j] == FLAG);
            bombAnswer += (answers[i][j] == MINE);
        }
    }
    return bombGrid == bombAnswer;
}

std::pair<char, std::pair<int, int>> getInput() {
    std::string s;
    std::cin >> s;
    if (s[0] == 'f')
        return std::make_pair('f', std::make_pair(charToInt(s[1]), charToInt(s[2])));
    if (s[0] == 'q')
        return std::make_pair('q', std::make_pair(0, 0));
    return std::make_pair('d', std::make_pair(charToInt(s[0]), charToInt(s[1])));
}

char digCell(std::vector<std::vector<char>> &grid, std::vector<std::vector<char>> &answers, int row, int col) {
    clearScreen();
    if (answers[row][col] == MINE) {
        printBoard(answers);
    }
    else if (answers[row][col] != SPACE) {
        grid[row][col] = answers[row][col];
        printBoard(grid);
    }
    else {
        std::queue<std::pair<int, int>> q;
        q.push(std::make_pair(row, col));
        std::vector<int> dirX = {-1, -1, -1, 0, 0, 1, 1, 1};
        std::vector<int> dirY = {-1, 0, 1, -1, 1, -1, 0, 1};
        while (!q.empty()) {
            std::pair<int, int> position = q.front();
            q.pop();
            if (grid[position.first][position.second] != UNCOVERED) 
                continue;
            grid[position.first][position.second] = answers[position.first][position.second];
            if (grid[position.first][position.second] != SPACE)
                continue;

            for (int i = 0; i < 8; i++) {
                int newX = dirX[i] + position.first;
                int newY = dirY[i] + position.second;
                if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size())
                    continue;
                q.push(std::make_pair(newX, newY));
            }
        }
        printBoard(grid);
    }
    return answers[row][col];
}

bool processDig(std::vector<std::vector<char>> &grid, std::vector<std::vector<char>> &answers, int row, int col) {
    if (grid[row][col] == UNCOVERED)
        return (digCell(grid, answers, row, col) != MINE);
    
    if (grid[row][col] == SPACE || grid[row][col] == FLAG)
        return true;

    std::vector<int> dirX = {-1, -1, -1, 0, 0, 1, 1, 1};
    std::vector<int> dirY = {-1, 0, 1, -1, 1, -1, 0, 1};

    bool b = true;

    for (int i = 0; i < 8; i++) {
        int newX = dirX[i] + row;
        int newY = dirY[i] + col;
        if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size())
            continue;
        if (grid[newX][newY] == FLAG)
            continue;
        b = b && (digCell(grid, answers, newX, newY) != MINE);
    }
    return b;
}

void play(int rows, int cols, int mines) {
    clearScreen();
    std::vector<std::vector<char>> answers(rows, std::vector<char>(cols, SPACE)); 
    std::vector<std::vector<char>> grid(rows, std::vector<char>(cols, UNCOVERED));
    printBoard(grid);
    std::pair<char, std::pair<int, int>> input = getInput();
    std::pair<int, int> position = input.second;
    if (input.first == 'q')
        return;
    do {
        placeMines(answers, mines);
        placeNumbers(answers);
    } while (grid[position.first][position.second] == MINE);
    digCell(grid, answers, position.first, position.second);
    char cell;
    do {
        std::pair<char, std::pair<int, int>> input = getInput();
        char type = input.first;
        std::pair<int, int> position = input.second;
        if (type == 'f')
            cell = flagCell(grid, position.first, position.second);
        else if (type == 'q') {
            return;
        }
        else
            cell = processDig(grid, answers, position.first, position.second);
    } while (cell && !gameWon(grid, answers)); 

    if (!cell) {
        std::cout << "You Lose!\n";
    }
    else {
        std::cout << "You Win!\n";
    }

    std::cout << "Play again? (y/n) ";
    char c;
    std::cin >> c;
    if (c == 'y')
        play(rows, cols, mines);
}

void settings(int &rows, int &cols, int &mines) {
    clearScreen();
    bool isBeginner = (rows == 8) && (cols == 8) && (mines == 10);
    bool isIntermediate = (rows == 16) && (cols == 16) && (mines == 40);
    bool isExpert = (rows == 30) && (cols == 16) && (mines == 99);
    bool isCustom = !isBeginner && !isIntermediate && !isExpert;
    std::cout << "0. Back\n";
    std::cout << "1. Difficulty:\n";
    std::cout << "   " << (isBeginner ? "<" : " ")     << " Beginner      8x8    (10 mines) " << (isBeginner ? ">" : " ") << "\n";
    std::cout << "   " << (isIntermediate ? "<" : " ") << " Intermediate  16x16  (40 mines) " << (isIntermediate ? ">" : " ") << "\n";
    std::cout << "   " << (isExpert ? "<" : " ")       << " Expert        30x16  (99 mines) " << (isExpert ? ">" : " ") << "\n";
    std::cout << "   " << (isCustom ? "<" : " ")       << " Custom        ";
    std::cout << (isCustom ? std::to_string(rows) : "??") << "x" << (isCustom ? std::to_string(cols) : "??") << "  (" << (isCustom ? std::to_string(mines) : "??") << " mines) ";
    std::cout << (isCustom ? ">" : " ") << "\n";
    std::cout << "\n";
    std::cout << "Option: ";
    char c;
    std::cin >> c;
    if (c == '0')
        return;
    else if (c == '1') {
        std::cout << "\n";
        std::cout << "(b)eginner, (i)ntermediate, (e)xpert, or (c)ustom? ";
        char difficulty;
        std::cin >> difficulty;
        if (difficulty == 'b') {
            rows = 8;
            cols = 8;
            mines = 10;
        }
        else if (difficulty == 'i') {
            rows = 16;
            cols = 16;
            mines = 40;
        }
        else if (difficulty == 'e') {
            rows = 30;
            cols = 16;
            mines = 99;
        }
        else {
            std::cout << "\n";
            std::cout << "rows? ";
            std::cin >> rows;
            std::cout << "cols? ";
            std::cin >> cols;
            std::cout << "mines? ";
            std::cin >> mines;
        }
    }

    settings(rows, cols, mines);
}

void mainMenu(int &rows, int &cols, int &mines) {
    clearScreen();
    std::cout << "=============================================================\n";
    std::cout << "  __  __ _                                                   \n";
    std::cout << " |  \\/  (_)                                                  \n";
    std::cout << " | \\  / |_ _ __   ___  _____      _____  ___ _ __   ___ _ __ \n";
    std::cout << " | |\\/| | | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__|\n";
    std::cout << " | |  | | | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |   \n";
    std::cout << " |_|  |_|_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|   \n";
    std::cout << "                                            | |              \n";
    std::cout << "                                            |_|              \n";
    std::cout << "=============================================================\n";
    std::cout << " 0. Quit\n";
    std::cout << " 1. Play\n";
    std::cout << " 2. Settings\n";
    std::cout << "\n";
    std::cout << "Option: ";
    char c;
    std::cin >> c;
    if (c == '0')
        return;
    else if (c == '1') {
        play(rows, cols, mines);
    }
    else if (c == '2')
        settings(rows, cols, mines);
    mainMenu(rows, cols, mines);
}

int main() {
    int rows = 8;
    int cols = 8;
    int mines = 10;
    mainMenu(rows, cols, mines);
}
