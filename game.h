#ifndef GAME_H
#define GAME_H

// =============================================================================
// SHADOW LUMEN — 2D Dungeon Platform Game
// Computer Graphics Project (OpenGL + FreeGLUT)
// =============================================================================

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <ctime>

using namespace std;

// -----------------------------------------------------------------------------
// Global Constants
// -----------------------------------------------------------------------------
const int SCREEN_WIDTH  = 1000;
const int SCREEN_HEIGHT = 600;
const float LEVEL_WIDTH = 2200.0f;

// Simple physics parameters
const float GRAVITY     = 0.55f;
const float MOVE_SPEED  = 4.5f;
const float JUMP_FORCE  = 12.0f;

// -----------------------------------------------------------------------------
// Enums
// -----------------------------------------------------------------------------
enum GameState {
    STATE_NAME_INPUT,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_LEADERBOARD
};

enum PlayerState {
    P_IDLE,
    P_WALKING,
    P_JUMPING,
    P_DUCKING
};

// -----------------------------------------------------------------------------
// Basic 2D Geometric Data Structures
// -----------------------------------------------------------------------------
struct Rect {
    float x, y;
    float w, h;

    Rect(float _x = 0, float _y = 0, float _w = 0, float _h = 0)
        : x(_x), y(_y), w(_w), h(_h) {}
};

struct Treasure {
    float x, y;
    float w, h;
    bool isTrap;        // Trap chest hurts player if opened
    bool isCollected;

    Treasure(float _x = 0, float _y = 0, bool _isTrap = false)
        : x(_x), y(_y), w(28.0f), h(22.0f), isTrap(_isTrap), isCollected(false) {}
};

struct Trap {
    float x, y;
    float w, h;
    int type;           // 0: Floor Spikes, 1: Wall/Ceiling Hazard

    Trap(float _x = 0, float _y = 0, float _w = 36.0f, float _h = 24.0f, int _type = 0)
        : x(_x), y(_y), w(_w), h(_h), type(_type) {}
};

struct Debris {
    float x, y;
    float startY;
    float speed;
    float size;
    float angle;

    Debris(float _x = 0, float _startY = 580.0f, float _speed = 3.2f, float _size = 18.0f)
        : x(_x), y(_startY), startY(_startY), speed(_speed), size(_size), angle(0.0f) {}
};

struct Portal {
    float x, y;
    float radius;

    Portal(float _x = 0, float _y = 0, float _r = 34.0f)
        : x(_x), y(_y), radius(_r) {}
};

struct LeaderboardEntry {
    string name;
    string timeStr;
    int treasure;
    int score;
};

// Player Structure
struct Player {
    float x, y;
    float vx, vy;
    float width, height;
    float normalHeight;
    float duckHeight;
    bool onGround;
    bool facingRight;
    PlayerState state;
    float animTimer;
    int lives;
    bool isInvulnerable;
    float invulnerableTimer;
};

// -----------------------------------------------------------------------------
// Shared Global Variables (Declared extern; defined in main.cpp)
// -----------------------------------------------------------------------------
extern GameState gameState;
extern int currentLevel;
extern int totalTreasuresCollected;
extern float gameTimeSeconds;
extern string playerName;
extern float cameraX;

extern Player player;

// Level entities
extern vector<Rect> platforms;
extern vector<Trap> traps;
extern vector<Debris> debrisList;
extern vector<Treasure> treasures;
extern Portal currentPortal;
extern vector<LeaderboardEntry> leaderboard;

// Keyboard input states
extern bool keyLeft;
extern bool keyRight;
extern bool keyJump;
extern bool keyDuck;

// Animation & visual effects variables
extern float portalAngle;
extern float portalPulse;
extern float lightFlicker;
extern float damageFlashTimer;

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

// --- Collision Detection ---
bool checkAABB(float x1, float y1, float w1, float h1,
               float x2, float y2, float w2, float h2);

// --- Player Functions (player.cpp) ---
void initPlayer();
void resetPlayerForLevel(int level);
void updatePlayer();
void drawPlayer();
void playerTakeDamage();

// --- Graphics & Level Rendering (graphics.cpp) ---
void drawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f);
void drawCircle(float cx, float cy, float radius, int segments, float r, float g, float b, float a = 1.0f);
void drawText(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18);
void drawTextCenter(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18);

void drawLightingAndShadow(float px, float py);
void drawCurrentLevel();
void drawLevel1();
void drawLevel2();
void drawLevel3();
void drawTraps();
void drawDebris();
void drawTreasures();
void drawPortal();

void drawHUD();
void drawNameInputScreen();
void drawGameOverScreen();
void drawLeaderboardScreen();

// --- Game Logic & Progression (main.cpp) ---
void loadLevel(int level);
void updateEnvironment();
void resetGame(bool sameName);
void advanceToNextLevel();
void completeGame();
void loadLeaderboard();
void saveScore();

#endif // GAME_H
