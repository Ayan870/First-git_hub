#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <sstream>
using namespace std;

// Enums for direction of snake movement and difficulty level
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum Difficulty { EASY = 1, MEDIUM, HARD };

// Forward declaration
class ObstacleManager;

// --------------------- USER CLASS ---------------------
class User {
private:
    string username, password;
    int highScore;

public:
    User() : highScore(0) {}

    // Load user from file
    bool load(const string& user, const string& pass) {
        ifstream inFile("users.txt");
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string u, p;
            int hs;
            ss >> u >> p >> hs;
            if (u == user && p == pass) {
                username = u;
                password = p;
                highScore = hs;
                return true;
            }
        }
        return false;
    }

    // Create new user and save to file
    bool create(const string& user, const string& pass) {
        ifstream inFile("users.txt");
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string u;
            ss >> u;
            if (u == user) return false;
        }
        ofstream outFile("users.txt", ios::app);
        outFile << user << " " << pass << " 0" << endl;
        username = user;
        password = pass;
        highScore = 0;
        return true;
    }

    // Update high score in file
    void updateHighScore(int newScore) {
        if (newScore > highScore) {
            highScore = newScore;
            ifstream inFile("users.txt");
            ofstream tempFile("temp.txt");
            string line;
            while (getline(inFile, line)) {
                stringstream ss(line);
                string u, p;
                int hs;
                ss >> u >> p >> hs;
                if (u == username) {
                    tempFile << u << " " << p << " " << highScore << endl;
                } else {
                    tempFile << u << " " << p << " " << hs << endl;
                }
            }
            inFile.close();
            tempFile.close();
            remove("users.txt");
            rename("temp.txt", "users.txt");
        }
    }

    int getHighScore() { return highScore; }
    string getUsername() { return username; }
};

// --------------------- SNAKE CLASS ---------------------
class Snake {
public:
    int x, y;
    int tailX[100], tailY[100];
    int nTail;
    Direction dir;

    Snake() { reset(); }

    // Reset snake to starting position
    void reset() {
        x = 10;
        y = 10;
        nTail = 0;
        dir = STOP;
    }

    // Move the tail of the snake following the head
    void moveTail() {
        int prevX = tailX[0];
        int prevY = tailY[0];
        int prev2X, prev2Y;
        tailX[0] = x;
        tailY[0] = y;
        for (int i = 1; i < nTail; i++) {
            prev2X = tailX[i];
            prev2Y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }
    }

    // Move snake based on direction
    void move() {
        switch (dir) {
            case LEFT: x--; break;
            case RIGHT: x++; break;
            case UP: y--; break;
            case DOWN: y++; break;
            default: break;
        }
    }

    // Check if snake hits itself
    bool checkSelfCollision() {
        for (int i = 0; i < nTail; i++)
            if (tailX[i] == x && tailY[i] == y)
                return true;
        return false;
    }
};

// --------------------- OBSTACLE MANAGER ---------------------
class ObstacleManager {
public:
    int obsX[10];
    int obsY[10];
    int count;

    ObstacleManager() : count(0) {}

    // Generate random obstacles avoiding snake and fruit
    void generate(int width, int height, int snakeX, int snakeY, int fruitX, int fruitY) {
        count = 5;
        for (int i = 0; i < count; i++) {
            do {
                obsX[i] = rand() % width;
                obsY[i] = rand() % height;
            } while  ((obsX[i] == snakeX && obsY[i] == snakeY) ||
                     (obsX[i] == fruitX && obsY[i] == fruitY));
        }
    }

    // Check if position collides with an obstacle
    bool isCollision(int x, int y) {
        for (int i = 0; i < count; i++)
            if (obsX[i] == x && obsY[i] == y)
                return true;
        return false;
    }

    // Check if a position is an obstacle
    bool isObstacle(int x, int y) {
        return isCollision(x, y);
    }
};

// --------------------- FRUIT CLASS ---------------------
class Fruit {
public:
    int x, y;

    // Spawn fruit randomly (avoiding obstacles if present)
    void spawn(int width, int height, ObstacleManager* obsMgr = nullptr) {
        do {
            x = rand() % width;
            y = rand() % height;
        } while (obsMgr && obsMgr->isObstacle(x, y));
    }
};

// --------------------- GAME BOARD CLASS ---------------------
class GameBoard {
public:
    const int width = 20;
    const int height = 20;

    // Draws the entire game state
    void draw(Snake& snake, Fruit& fruit, ObstacleManager& obsMgr, Difficulty diff) {
        system("cls");
        for (int i = 0; i < width + 2; i++) cout << "#";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0) cout << "#";

                if (i == snake.y && j == snake.x)
                    cout << "O";
                else if (i == fruit.y && j == fruit.x)
                    cout << "*";
                else {
                    bool printed = false;
                    for (int k = 0; k < snake.nTail; k++) {
                        if (snake.tailX[k] == j && snake.tailY[k] == i) {
                            cout << "o";
                            printed = true;
                            break;
                        }
                    }
                    if (!printed && diff == HARD && obsMgr.isObstacle(j, i)) {
                        cout << "X";
                        printed = true;
                    }
                    if (!printed) cout << " ";
                }

                if (j == width - 1) cout << "#";
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++) cout << "#";
        cout << endl;
    }
};

// --------------------- GAME ENGINE ---------------------
class GameEngine {
private:
    Snake snake;
    Fruit fruit;
    ObstacleManager obsMgr;
    GameBoard board;
    User& user;
    Difficulty difficulty;
    bool gameOver;
    int score;

public:
    GameEngine(User& u, Difficulty diff) : user(u), difficulty(diff), score(0), gameOver(false) {
        snake.reset();
        fruit.spawn(board.width, board.height);
        if (difficulty == HARD) {
            obsMgr.generate(board.width, board.height, snake.x, snake.y, fruit.x, fruit.y);
        }
    }

    // Game loop
    void run() {
        while (!gameOver) {
            board.draw(snake, fruit, obsMgr, difficulty);
            cout << "Score: " << score << endl;
            input();
            logic();
            Sleep(100);
        }
        cout << "\nGame Over! Your score: " << score << endl;
        user.updateHighScore(score);
    }

    // Handles keyboard input
    void input() {
        if (_kbhit()) {
            switch (_getch()) {
                case 'a': snake.dir = LEFT; break;
                case 'd': snake.dir = RIGHT; break;
                case 'w': snake.dir = UP; break;
                case 's': snake.dir = DOWN; break;
                case 'x': gameOver = true; break;
            }
        }
    }

    // Game logic for movement, collision, scoring
    void logic() {
        snake.moveTail();
        snake.move();

        if (difficulty != EASY) {
            if (snake.x >= board.width || snake.x < 0 || snake.y >= board.height || snake.y < 0)
                gameOver = true;
        } else {
            if (snake.x >= board.width) snake.x = 0;
            else if (snake.x < 0) snake.x = board.width - 1;
            if (snake.y >= board.height) snake.y = 0;
            else if (snake.y < 0) snake.y = board.height - 1;
        }

        if (snake.checkSelfCollision()) gameOver = true;
        if (difficulty == HARD && obsMgr.isCollision(snake.x, snake.y)) gameOver = true;

        if (snake.x == fruit.x && snake.y == fruit.y) {
            score += 10;
            fruit.spawn(board.width, board.height, difficulty == HARD ? &obsMgr : nullptr);
            snake.nTail++;
        }
    }
};

// --------------------- PASSWORD MASKING ---------------------
string getHiddenPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') {  // '\r' = Enter key
        if (ch == '\b') {  // Backspace
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        } else {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

// --------------------- MAIN FUNCTION ---------------------
int main() {
    while (true) {
        User user;
        int choice;
        string username, password;

        cout << "1. Login\n2. Register\nChoose option: ";
        cin >> choice;

        if (cin.fail() || (choice != 1 && choice != 2)) {
            cout << "Invalid option. Exiting.\n";
            return 0;
        }

        cin.ignore();
        cout << "Username: "; getline(cin, username);
        cout << "Password: ";
        password = getHiddenPassword();

        bool success = (choice == 1) ? user.load(username, password) : user.create(username, password);
        if (!success) {
            cout << "Login/Register failed. Exiting.\n";
            return 0;
        }

        cout << "Welcome " << user.getUsername() << "! High Score: " << user.getHighScore() << endl;

        bool loggedIn = true;
        while (loggedIn) {
            int diff;
            cout << "\nSelect difficulty: \n1. Easy \n2. Medium \n3. Hard\nChoose: ";
            cin >> diff;
            if (diff < 1 || diff > 3) {
                cout << "Invalid difficulty. Exiting.\n";
                return 0;
            }

            GameEngine game(user, static_cast<Difficulty>(diff));
            game.run();

            int postGameChoice;
            cout << "\n1. Play Again\n2. Logout\n3. Quit\nChoose option: ";
            cin >> postGameChoice;

            if (postGameChoice == 2) {
                loggedIn = false;
            } else if (postGameChoice == 3) {
                return 0;
            }
        }
    }
}
