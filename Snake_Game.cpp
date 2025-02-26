#include <iostream>
#include <conio.h>  // For _kbhit() and _getch()
#include <windows.h> // For Sleep()

using namespace std;

bool gameOver;
bool easymode = false;
const int width = 20;
const int hieght = 20;  // Typo: should be "height"
int x, y, foodX, foodY, score;
int tailX[100], tailY[100];  // Arrays for storing tail positions
int Tail;  // Length of the snake's tail
int delayTime = 100;  // Control game speed

// Enum for snake movement directions
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

void setup() {
    gameOver = false;
    dir = STOP;
    
    // Initialize snake position
    x = width % 20;
    y = hieght % 20;

    // Generate random food position
    foodX = rand() % width;
    foodY = rand() % hieght;
    
    score = 0;
}

void draw() {
    system("cls");  // Clear screen

    // Draw top border
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    // Draw game area
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < hieght; j++) {
            if (j == 0) 
                cout << "#";  // Left border
            
            if (i == y && j == x)
                cout << "O";  // Snake head
            else if (i == foodY && j == foodX)
                cout << "*";  // Food
            else {
                bool print = false;
                for (int k = 0; k < Tail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";  // Snake tail
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";  // Empty space
            }
            
            if (j == width - 1)
                cout << "#";  // Right border
        }
        cout << endl;
    }

    // Draw bottom border
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    cout << "Score: " << score;
}

void input() {
    if (_kbhit()) {  // Check if a key is pressed
        switch (_getch()) {
            case 'a': dir = LEFT; break;
            case 'd': dir = RIGHT; break;
            case 'w': dir = UP; break;
            case 's': dir = DOWN; break;
            case 'x': gameOver = true; break;  // Quit game
        }
    }
}

void logic() {
    // Move the tail
    int prevX = tailX[0], prevY = tailY[0], prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    
    for (int i = 1; i < Tail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Move the snake head
    switch (dir) {
        case LEFT: x--; break;
        case RIGHT: x++; break;
        case UP: y--; break;
        case DOWN: y++; break;
    }

    // Teleport snake to opposite side if it goes out of bounds
    if (easymode){
        if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= hieght) y = 0; else if (y < 0) y = hieght - 1;
    }
    // Check collision with walls
    else{
    if (x >= width || x < 0 || y >= hieght || y < 0)
        gameOver = true;}

    // Check collision with itself
    for (int i = 0; i < Tail; i++) {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }

    // Check if food is eaten
    if (x == foodX && y == foodY) {
        score++;
        foodX = rand() % width;
        foodY = rand() % hieght;
        Tail++;  // Increase tail length
    }
}

int main() {
    cout << "Controls:\nW for Up\nS for Down\nA for Left\nD for Right\n";
    
    // Select difficulty level
    char diff;
    cout << "\nEnter difficulty level:\nH - Hard\nE - Easy\n";
    cin >> diff;

    if (diff == 'E' || diff == 'e') {
        easymode = true;
        delayTime = 100;  // Slow speed for easy mode
    }
    else if (diff == 'H' || diff == 'h') {
        delayTime = 50;  // Fast speed for hard mode
    } else {
        cout << "Invalid difficulty!";
        return 0;
    }

    setup();
    
    while (!gameOver) {
        draw();
        input();
        logic();
        Sleep(delayTime);  // Control game speed
    }

    return 0;
}
