#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool allowMove = false;
Color green = {15, 23, 42, 255};         
Color darkGreen = {255, 255, 255, 255}; // white snake/border/text

int cellSize = 22;
int cellCount = 25;
int offset = 50;

double lastUpdateTime = 0;

// ---------------- UTILITY FUNCTIONS ----------------
bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
            return true;
    }
    return false;
}

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

// ---------------- BASE CLASS (INHERITANCE) ----------------
class Entity
{
public:
    virtual void Draw() = 0; // pure virtual → polymorphism
};

// ---------------- SNAKE CLASS ----------------
class Snake : public Entity
{
private:
    deque<Vector2> body;
    Vector2 direction;
    bool addSegment;

public:
    Snake()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0}; 
        addSegment = false;
    }

    void Draw() override
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            Rectangle segment = {
                offset + body[i].x * cellSize,
                offset + body[i].y * cellSize,
                (float)cellSize,
                (float)cellSize};

            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));

        if (addSegment)
            addSegment = false;
        else
            body.pop_back();
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }

    // getters/setters → encapsulation
    deque<Vector2> getBody() { return body; }
    Vector2 getHead() { return body[0]; }
    Vector2 getDirection() { return direction; }

    void setDirection(Vector2 dir) { direction = dir; }
    void grow() { addSegment = true; }
};

// ---------------- FOOD CLASS ----------------
class Food : public Entity
{
private:
    Vector2 position;
    Texture2D texture;

public:
    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw() override
{
    DrawCircle(
        offset + position.x * cellSize + cellSize / 2,
        offset + position.y * cellSize + cellSize / 2,
        cellSize / 2.5,
        WHITE
    );
}

    Vector2 GenerateRandomCell()
    {
        return Vector2{
            (float)GetRandomValue(0, cellCount - 1),
            (float)GetRandomValue(0, cellCount - 1)};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 pos = GenerateRandomCell();

        while (ElementInDeque(pos, snakeBody))
            pos = GenerateRandomCell();

        return pos;
    }

    Vector2 getPosition() { return position; }
    void setPosition(Vector2 pos) { position = pos; }
};

// ---------------- GAME CLASS ----------------
class Game
{
private:
    Snake snake;
    Food food;
    bool running;
    int score;
    Sound eatSound;
    Sound wallSound;

public:
    Game() : food(snake.getBody())
    {
        running = true;
        score = 0;

        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        // polymorphism usage
        Entity *e1 = &food;
        Entity *e2 = &snake;

        e1->Draw();
        e2->Draw();
    }

    void Update()
    {
        if (!running)
            return;

        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.getHead(), food.getPosition()))
        {
            food.setPosition(food.GenerateRandomPos(snake.getBody()));
            snake.grow();
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        Vector2 head = snake.getHead();

        if (head.x == cellCount || head.x == -1 ||
            head.y == cellCount || head.y == -1)
        {
            GameOver();
        }
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> body = snake.getBody();
        Vector2 head = body[0];

        body.pop_front();

        if (ElementInDeque(head, body))
            GameOver();
    }

    void GameOver()
    {
        snake.Reset();
        food.setPosition(food.GenerateRandomPos(snake.getBody()));
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    // input handling abstraction
    void HandleInput()
    {
        Vector2 dir = snake.getDirection();

        if (IsKeyPressed(KEY_UP) && dir.y != 1 && allowMove)
        {
            snake.setDirection({0, -1});
            running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && dir.y != -1 && allowMove)
        {
            snake.setDirection({0, 1});
            running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && dir.x != 1 && allowMove)
        {
            snake.setDirection({-1, 0});
            running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && dir.x != -1 && allowMove)
        {
            snake.setDirection({1, 0});
            running = true;
            allowMove = false;
        }
    }

    int getScore() { return score; }
};

// ---------------- MAIN ----------------
int main()
{
    cout << "Starting the game..." << endl;

    InitWindow(2 * offset + cellSize * cellCount,
               2 * offset + cellSize * cellCount,
               "Retro Snake");

    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (EventTriggered(0.2))
        {
            allowMove = true;
            game.Update();
        }

        game.HandleInput();

        // Drawing
        ClearBackground(green);

        DrawRectangleLinesEx(
            Rectangle{(float)offset - 5, (float)offset - 5,
                      (float)cellSize * cellCount + 10,
                      (float)cellSize * cellCount + 10},
            5, darkGreen);

        DrawText("Retro Snake", offset, 10, 40, darkGreen);
        DrawText(TextFormat("%i", game.getScore()),
                 offset - 5,
                 offset + cellSize * cellCount + 10,
                 40, darkGreen);

        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}