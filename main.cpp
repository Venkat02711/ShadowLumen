// =============================================================================
// main.cpp — Application Entry Point, Game Loop, State Machine & Input
// Computer Graphics Project: SHADOW LUMEN
// =============================================================================

#include "game.h"

using namespace std;

// -----------------------------------------------------------------------------
// Global Variable Definitions
// -----------------------------------------------------------------------------
GameState gameState = STATE_NAME_INPUT;
int currentLevel = 1;
int totalTreasuresCollected = 0;
float gameTimeSeconds = 0.0f;
string playerName = "";
float cameraX = 0.0f;

Player player;

// Level entities
vector<Rect> platforms;
vector<Trap> traps;
vector<Debris> debrisList;
vector<Treasure> treasures;
Portal currentPortal;
vector<LeaderboardEntry> leaderboard;

// Key states
bool keyLeft = false;
bool keyRight = false;
bool keyJump = false;
bool keyDuck = false;

// Visual & animation states
float portalAngle = 0.0f;
float portalPulse = 1.0f;
float lightFlicker = 0.0f;
float damageFlashTimer = 0.0f;

// -----------------------------------------------------------------------------
// Collision Detection (Axis-Aligned Bounding Box - AABB)
// Demonstrates: Geometric Bounding Box Intersection
// -----------------------------------------------------------------------------
bool checkAABB(float x1, float y1, float w1, float h1,
               float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

// -----------------------------------------------------------------------------
// Level Layouts & Setup
// -----------------------------------------------------------------------------
void loadLevel(int level) {
    currentLevel = level;
    platforms.clear();
    traps.clear();
    debrisList.clear();
    treasures.clear();

    // Reset player position for new level
    resetPlayerForLevel(level);

    // Left and right boundary dungeon walls
    platforms.push_back(Rect(0.0f, 0.0f, 30.0f, 600.0f));
    platforms.push_back(Rect(LEVEL_WIDTH - 30.0f, 0.0f, 30.0f, 600.0f));

    if (level == 1) {
        // =====================================================================
        // LEVEL 1: Forgotten Ruins
        // Introductory layout: Stone platforms, basic floor spikes, 3 treasures
        // =====================================================================

        // Ground platforms with gaps
        platforms.push_back(Rect(0.0f, 0.0f, 650.0f, 75.0f));
        platforms.push_back(Rect(750.0f, 0.0f, 750.0f, 75.0f));
        platforms.push_back(Rect(1600.0f, 0.0f, 600.0f, 75.0f));

        // Floating stone platforms
        platforms.push_back(Rect(260.0f, 160.0f, 170.0f, 24.0f));
        platforms.push_back(Rect(520.0f, 230.0f, 180.0f, 24.0f));
        platforms.push_back(Rect(780.0f, 170.0f, 170.0f, 24.0f));
        platforms.push_back(Rect(1040.0f, 260.0f, 190.0f, 24.0f));
        platforms.push_back(Rect(1320.0f, 190.0f, 170.0f, 24.0f));
        platforms.push_back(Rect(1560.0f, 250.0f, 180.0f, 24.0f));
        platforms.push_back(Rect(1820.0f, 160.0f, 170.0f, 24.0f));

        // Spikes
        traps.push_back(Trap(650.0f, 0.0f, 100.0f, 25.0f, 0));  // Gap 1 spike pit
        traps.push_back(Trap(1180.0f, 75.0f, 70.0f, 25.0f, 0)); // Ground spikes
        traps.push_back(Trap(1500.0f, 0.0f, 100.0f, 25.0f, 0)); // Gap 2 spike pit

        // Treasures (2 Normal, 1 Trap Chest)
        treasures.push_back(Treasure(320.0f, 184.0f, false));   // Normal
        treasures.push_back(Treasure(1120.0f, 284.0f, false));  // Normal
        treasures.push_back(Treasure(1620.0f, 274.0f, true));   // Trap chest!

        // Falling Debris (Gentle introduction)
        debrisList.push_back(Debris(620.0f, 580.0f, 3.2f, 18.0f));
        debrisList.push_back(Debris(1440.0f, 580.0f, 3.5f, 20.0f));

        // Exit Portal
        currentPortal = Portal(2040.0f, 130.0f, 36.0f);

    } else if (level == 2) {
        // =====================================================================
        // LEVEL 2: Collapsed Hall
        // More vertical elements, broken platforms, gaps, falling debris
        // =====================================================================

        // Ground broken segments
        platforms.push_back(Rect(0.0f, 0.0f, 440.0f, 75.0f));
        platforms.push_back(Rect(700.0f, 0.0f, 220.0f, 55.0f));
        platforms.push_back(Rect(1320.0f, 0.0f, 260.0f, 55.0f));
        platforms.push_back(Rect(1950.0f, 0.0f, 250.0f, 75.0f));

        // Ascending & Descending Platform Steps
        platforms.push_back(Rect(500.0f, 130.0f, 150.0f, 24.0f));
        platforms.push_back(Rect(730.0f, 210.0f, 160.0f, 24.0f));
        platforms.push_back(Rect(980.0f, 150.0f, 150.0f, 24.0f));
        platforms.push_back(Rect(1200.0f, 240.0f, 160.0f, 24.0f));
        platforms.push_back(Rect(1440.0f, 320.0f, 170.0f, 24.0f));
        platforms.push_back(Rect(1700.0f, 230.0f, 160.0f, 24.0f));

        // Low ceiling archway requiring ducking
        platforms.push_back(Rect(860.0f, 100.0f, 110.0f, 18.0f));

        // Spikes
        traps.push_back(Trap(440.0f, 0.0f, 260.0f, 25.0f, 0));  // Chasm 1 spikes
        traps.push_back(Trap(920.0f, 0.0f, 400.0f, 25.0f, 0));  // Chasm 2 spikes
        traps.push_back(Trap(1580.0f, 0.0f, 370.0f, 25.0f, 0)); // Chasm 3 spikes
        traps.push_back(Trap(1260.0f, 360.0f, 50.0f, 30.0f, 1)); // Hanging stalactite trap

        // Treasures
        treasures.push_back(Treasure(550.0f, 154.0f, false));  // Normal
        treasures.push_back(Treasure(780.0f, 234.0f, true));   // Trap chest!
        treasures.push_back(Treasure(1500.0f, 344.0f, false)); // Normal

        // Falling Debris
        debrisList.push_back(Debris(480.0f, 580.0f, 4.0f, 20.0f));
        debrisList.push_back(Debris(1100.0f, 580.0f, 4.4f, 22.0f));
        debrisList.push_back(Debris(1620.0f, 580.0f, 3.8f, 19.0f));

        // Exit Portal
        currentPortal = Portal(2050.0f, 130.0f, 36.0f);

    } else if (level == 3) {
        // =====================================================================
        // LEVEL 3: Lumen Vault
        // Final level: Dark obsidian fortress, hazard gauntlets, grand portal
        // =====================================================================

        platforms.push_back(Rect(0.0f, 0.0f, 380.0f, 75.0f));
        platforms.push_back(Rect(2000.0f, 0.0f, 200.0f, 75.0f));

        // Multi-tiered vault platforms
        platforms.push_back(Rect(450.0f, 140.0f, 140.0f, 24.0f));
        platforms.push_back(Rect(670.0f, 230.0f, 150.0f, 24.0f));
        platforms.push_back(Rect(900.0f, 150.0f, 140.0f, 24.0f));
        platforms.push_back(Rect(1120.0f, 250.0f, 160.0f, 24.0f));
        platforms.push_back(Rect(1370.0f, 340.0f, 160.0f, 24.0f));
        platforms.push_back(Rect(1620.0f, 260.0f, 150.0f, 24.0f));
        platforms.push_back(Rect(1840.0f, 170.0f, 150.0f, 24.0f));

        // High secret treasure vault platform
        platforms.push_back(Rect(980.0f, 390.0f, 180.0f, 24.0f));

        // Spikes across bottom abyss
        traps.push_back(Trap(380.0f, 0.0f, 520.0f, 25.0f, 0));
        traps.push_back(Trap(1040.0f, 0.0f, 960.0f, 25.0f, 0));

        // Ceiling hazard stalactites
        traps.push_back(Trap(720.0f, 380.0f, 40.0f, 32.0f, 1));
        traps.push_back(Trap(1430.0f, 460.0f, 45.0f, 32.0f, 1));

        // Treasures (Generous rewards + deceptive traps)
        treasures.push_back(Treasure(500.0f, 164.0f, false)); // Normal
        treasures.push_back(Treasure(720.0f, 254.0f, true));  // Trap chest!
        treasures.push_back(Treasure(1040.0f, 414.0f, false)); // Secret high chest!
        treasures.push_back(Treasure(1430.0f, 364.0f, true)); // Trap chest!
        treasures.push_back(Treasure(1890.0f, 194.0f, false)); // Normal

        // Multiple falling debris hazards
        debrisList.push_back(Debris(420.0f, 580.0f, 4.2f, 21.0f));
        debrisList.push_back(Debris(840.0f, 580.0f, 4.6f, 23.0f));
        debrisList.push_back(Debris(1300.0f, 580.0f, 4.0f, 20.0f));
        debrisList.push_back(Debris(1760.0f, 580.0f, 4.4f, 22.0f));

        // Grand Final Portal
        currentPortal = Portal(2060.0f, 135.0f, 42.0f);
    }
}

// -----------------------------------------------------------------------------
// Environment Updates (Debris, Traps, Treasures, Portal)
// -----------------------------------------------------------------------------
void updateEnvironment() {
    // 1. Portal Animation (Rotation & Pulsing Scale)
    portalAngle += 2.2f;
    if (portalAngle > 360.0f) portalAngle -= 360.0f;
    portalPulse = 1.0f + 0.12f * sin(portalAngle * 0.05f);

    // 2. Damage flash screen timer
    if (damageFlashTimer > 0.0f) {
        damageFlashTimer -= 0.016f;
    }

    // 3. Falling Debris Update (Animation: y = y - speed)
    for (size_t i = 0; i < debrisList.size(); ++i) {
        Debris& d = debrisList[i];
        d.y -= d.speed;
        d.angle += 3.2f; // Spin animation

        // Reset to ceiling upon reaching the bottom
        if (d.y < 30.0f) {
            d.y = d.startY;
        }

        // Collision with player
        if (!player.isInvulnerable) {
            float halfSize = d.size * 0.5f;
            if (checkAABB(player.x, player.y, player.width, player.height,
                          d.x - halfSize, d.y - halfSize, d.size, d.size)) {
                playerTakeDamage();
                d.y = d.startY; // Reset debris immediately
            }
        }
    }

    // 4. Trap Collisions
    if (!player.isInvulnerable) {
        for (size_t i = 0; i < traps.size(); ++i) {
            const Trap& t = traps[i];
            if (checkAABB(player.x, player.y, player.width, player.height,
                          t.x + 3.0f, t.y, t.w - 6.0f, t.h)) {
                playerTakeDamage();
                break;
            }
        }
    }

    // 5. Treasure Collection
    for (size_t i = 0; i < treasures.size(); ++i) {
        Treasure& tr = treasures[i];
        if (!tr.isCollected) {
            if (checkAABB(player.x, player.y, player.width, player.height,
                          tr.x, tr.y, tr.w, tr.h)) {
                tr.isCollected = true;
                if (tr.isTrap) {
                    playerTakeDamage(); // Trap chest hurts player!
                } else {
                    totalTreasuresCollected++; // Normal treasure collected
                }
            }
        }
    }

    // 6. Portal Collision Check
    float playerCenterX = player.x + player.width * 0.5f;
    float playerCenterY = player.y + player.height * 0.5f;
    float dx = playerCenterX - currentPortal.x;
    float dy = playerCenterY - currentPortal.y;
    float distSq = dx * dx + dy * dy;

    if (distSq < (currentPortal.radius * currentPortal.radius)) {
        if (currentLevel < 3) {
            advanceToNextLevel();
        } else {
            completeGame();
        }
    }
}

// -----------------------------------------------------------------------------
// Camera Update (Smooth Horizontal Follow)
// Demonstrates: Coordinate Transformation (Camera glTranslatef)
// -----------------------------------------------------------------------------
void updateCamera() {
    cameraX = player.x - SCREEN_WIDTH * 0.5f;
    if (cameraX < 0.0f) {
        cameraX = 0.0f;
    }
    if (cameraX > LEVEL_WIDTH - SCREEN_WIDTH) {
        cameraX = LEVEL_WIDTH - SCREEN_WIDTH;
    }
}

// -----------------------------------------------------------------------------
// Game State Transitions
// -----------------------------------------------------------------------------
void advanceToNextLevel() {
    loadLevel(currentLevel + 1);
}

void completeGame() {
    gameState = STATE_LEADERBOARD;
    saveScore();
}

void resetGame(bool sameName) {
    if (!sameName) {
        playerName = "";
        gameState = STATE_NAME_INPUT;
        return;
    }

    // Same name: restart directly at Level 1
    initPlayer();
    totalTreasuresCollected = 0;
    gameTimeSeconds = 0.0f;
    loadLevel(1);
    cameraX = 0.0f;
    gameState = STATE_PLAYING;
}

// -----------------------------------------------------------------------------
// Leaderboard Persistence (leaderboard.txt)
// Format: NAME|MM:SS|TREASURE|SCORE
// -----------------------------------------------------------------------------
static bool compareLeaderboard(const LeaderboardEntry& a, const LeaderboardEntry& b) {
    return a.score > b.score;
}

void loadLeaderboard() {
    leaderboard.clear();
    ifstream inFile("leaderboard.txt");

    if (!inFile.is_open()) {
        // If file does not exist, initialize with classic benchmark scores
        leaderboard.push_back({"ARUN",  "01:42", 6, 5858});
        leaderboard.push_back({"RAHUL", "02:01", 7, 5899});
        leaderboard.push_back({"KIRAN", "02:18", 4, 3782});
        leaderboard.push_back({"LUMEN", "02:45", 5, 4200});
        saveScore(); // Write initial file
        return;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name, timeStr, treasureStr, scoreStr;

        if (getline(ss, name, '|') &&
            getline(ss, timeStr, '|') &&
            getline(ss, treasureStr, '|') &&
            getline(ss, scoreStr, '|')) {
            LeaderboardEntry entry;
            entry.name = name;
            entry.timeStr = timeStr;
            entry.treasure = atoi(treasureStr.c_str());
            entry.score = atoi(scoreStr.c_str());
            leaderboard.push_back(entry);
        }
    }
    inFile.close();

    sort(leaderboard.begin(), leaderboard.end(), compareLeaderboard);
}

void saveScore() {
    // Beginner-friendly scoring formula:
    // score = (Treasure * 1000) + Completion Bonus (3000) - (Elapsed Time * 8)
    int totalSec = (int)gameTimeSeconds;
    int mins = totalSec / 60;
    int secs = totalSec % 60;
    char timeBuf[16];
    sprintf(timeBuf, "%02d:%02d", mins, secs);

    int finalScore = (totalTreasuresCollected * 1000) + 3000 - (totalSec * 8);
    if (finalScore < 0) finalScore = 0;

    LeaderboardEntry newEntry;
    newEntry.name = playerName.empty() ? "LUMEN" : playerName;
    newEntry.timeStr = timeBuf;
    newEntry.treasure = totalTreasuresCollected;
    newEntry.score = finalScore;

    leaderboard.push_back(newEntry);
    sort(leaderboard.begin(), leaderboard.end(), compareLeaderboard);

    // Write back to leaderboard.txt
    ofstream outFile("leaderboard.txt");
    if (outFile.is_open()) {
        for (size_t i = 0; i < leaderboard.size(); ++i) {
            outFile << leaderboard[i].name << "|"
                    << leaderboard[i].timeStr << "|"
                    << leaderboard[i].treasure << "|"
                    << leaderboard[i].score << "\n";
        }
        outFile.close();
    }
}

// -----------------------------------------------------------------------------
// FreeGLUT Callbacks
// -----------------------------------------------------------------------------
void display() {
    glClearColor(0.015f, 0.015f, 0.025f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gameState == STATE_NAME_INPUT) {
        drawNameInputScreen();
    } else if (gameState == STATE_GAME_OVER) {
        drawGameOverScreen();
    } else if (gameState == STATE_LEADERBOARD) {
        drawLeaderboardScreen();
    } else if (gameState == STATE_PLAYING) {
        // --- 1. World Rendering with Camera Transformation ---
        glPushMatrix();
        glTranslatef(-cameraX, 0.0f, 0.0f);

        // Draw level architecture, hazards, collectibles, portal
        drawCurrentLevel();
        drawTraps();
        drawTreasures();
        drawPortal();
        drawDebris();
        drawPlayer();

        // Atmospheric Shadow & Lantern Lighting Overlay
        drawLightingAndShadow(player.x + player.width * 0.5f,
                              player.y + player.height * 0.55f);

        glPopMatrix();

        // --- 2. Static Screen Overlay (HUD) ---
        drawHUD();
    }

    glutSwapBuffers();
}

void timer(int val) {
    lightFlicker += 0.08f;

    if (gameState == STATE_PLAYING) {
        gameTimeSeconds += 0.016f; // ~60fps time step
        updatePlayer();
        updateEnvironment();
        updateCamera();
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Next frame in ~16ms (~60 FPS)
}

// Keyboard Input: Normal Keys (ASCII)
void keyboard(unsigned char key, int x, int y) {
    if (gameState == STATE_NAME_INPUT) {
        if (key == 13) { // ENTER key
            if (playerName.empty()) {
                playerName = "LUMEN";
            }
            // Capitalize player name for crisp leaderboard display
            for (size_t i = 0; i < playerName.length(); ++i) {
                playerName[i] = (char)toupper(playerName[i]);
            }
            resetGame(true);
        } else if (key == 8) { // BACKSPACE key
            if (!playerName.empty()) {
                playerName.pop_back();
            }
        } else if (key >= 32 && key <= 126) { // Printable characters
            if (playerName.length() < 10) {
                playerName += (char)toupper(key);
            }
        }
        return;
    }

    if (gameState == STATE_PLAYING) {
        if (key == 'a' || key == 'A') keyLeft = true;
        if (key == 'd' || key == 'D') keyRight = true;
        if (key == 'w' || key == 'W' || key == ' ') keyJump = true;
        if (key == 's' || key == 'S') keyDuck = true;
    } else if (gameState == STATE_GAME_OVER || gameState == STATE_LEADERBOARD) {
        if (key == '1' || key == 'r' || key == 'R') {
            resetGame(true);  // Retry - same name
        } else if (key == '2' || key == 'n' || key == 'N') {
            resetGame(false); // New name
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (gameState == STATE_PLAYING) {
        if (key == 'a' || key == 'A') keyLeft = false;
        if (key == 'd' || key == 'D') keyRight = false;
        if (key == 'w' || key == 'W' || key == ' ') keyJump = false;
        if (key == 's' || key == 'S') keyDuck = false;
    }
}

// Special Keys: Arrow Keys
void special(int key, int x, int y) {
    if (gameState == STATE_PLAYING) {
        if (key == GLUT_KEY_LEFT)  keyLeft = true;
        if (key == GLUT_KEY_RIGHT) keyRight = true;
        if (key == GLUT_KEY_UP)    keyJump = true;
        if (key == GLUT_KEY_DOWN)  keyDuck = true;
    }
}

void specialUp(int key, int x, int y) {
    if (gameState == STATE_PLAYING) {
        if (key == GLUT_KEY_LEFT)  keyLeft = false;
        if (key == GLUT_KEY_RIGHT) keyRight = false;
        if (key == GLUT_KEY_UP)    keyJump = false;
        if (key == GLUT_KEY_DOWN)  keyDuck = false;
    }
}

// -----------------------------------------------------------------------------
// Window Reshape & 2D Orthographic Setup
// -----------------------------------------------------------------------------
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)SCREEN_WIDTH, 0.0, (GLdouble)SCREEN_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// -----------------------------------------------------------------------------
// Application Entry Point
// -----------------------------------------------------------------------------
int main(int argc, char** argv) {
    // 1. Initialize FreeGLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Shadow Lumen — 2D Computer Graphics Project");

    // 2. Initialize Game Systems
    loadLeaderboard();
    initPlayer();
    loadLevel(1);

    // 3. Register Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutTimerFunc(16, timer, 0);

    // 4. Start FreeGLUT Main Loop
    glutMainLoop();
    return 0;
}
