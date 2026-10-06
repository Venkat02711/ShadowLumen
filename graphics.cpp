// =============================================================================
// graphics.cpp — 2D Geometric Primitives, Lighting, Levels, Traps, HUD & UI
// Computer Graphics Project: SHADOW LUMEN
// =============================================================================

#include "game.h"

using namespace std;

// -----------------------------------------------------------------------------
// Basic 2D Primitive Drawing Utilities
// -----------------------------------------------------------------------------
void drawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawCircle(float cx, float cy, float radius, int segments, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.14159265f * (float)i / (float)segments;
        float x = cx + radius * cos(theta);
        float y = cy + radius * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawText(float x, float y, const string& text, void* font) {
    glRasterPos2f(x, y);
    for (size_t i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(font, text[i]);
    }
}

void drawTextCenter(float x, float y, const string& text, void* font) {
    int totalWidth = 0;
    for (size_t i = 0; i < text.length(); ++i) {
        totalWidth += glutBitmapWidth(font, text[i]);
    }
    glRasterPos2f(x - totalWidth / 2.0f, y);
    for (size_t i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(font, text[i]);
    }
}

// -----------------------------------------------------------------------------
// Geometric Heart for HUD Lives Display
// -----------------------------------------------------------------------------
static void drawHeart(float x, float y, float size, bool filled) {
    if (filled) {
        glColor3f(0.95f, 0.18f, 0.24f); // Radiant red
        // Two upper circular lobes
        drawCircle(x - size * 0.35f, y + size * 0.25f, size * 0.38f, 14, 0.95f, 0.18f, 0.24f);
        drawCircle(x + size * 0.35f, y + size * 0.25f, size * 0.38f, 14, 0.95f, 0.18f, 0.24f);
        // Lower triangle pointing downward
        glBegin(GL_TRIANGLES);
        glColor3f(0.85f, 0.12f, 0.18f);
        glVertex2f(x - size * 0.72f, y + size * 0.20f);
        glVertex2f(x + size * 0.72f, y + size * 0.20f);
        glVertex2f(x, y - size * 0.65f);
        glEnd();
    } else {
        // Empty / Lost life heart outline
        glColor4f(0.35f, 0.35f, 0.40f, 0.6f);
        drawCircle(x - size * 0.35f, y + size * 0.25f, size * 0.38f, 14, 0.25f, 0.25f, 0.30f);
        drawCircle(x + size * 0.35f, y + size * 0.25f, size * 0.38f, 14, 0.25f, 0.25f, 0.30f);
        glBegin(GL_TRIANGLES);
        glColor3f(0.20f, 0.20f, 0.25f);
        glVertex2f(x - size * 0.72f, y + size * 0.20f);
        glVertex2f(x + size * 0.72f, y + size * 0.20f);
        glVertex2f(x, y - size * 0.65f);
        glEnd();
    }
}

// -----------------------------------------------------------------------------
// Atmospheric Dynamic Lighting & Shadow Mesh
// Demonstrates: Alpha Blending, Layered Geometric Shapes, Animation (flicker)
// -----------------------------------------------------------------------------
void drawLightingAndShadow(float px, float py) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dynamic subtle flicker simulation
    float flicker = sin(lightFlicker * 3.5f) * 5.0f + cos(lightFlicker * 5.8f) * 2.5f;
    float rInner = 85.0f  + flicker;
    float rMid   = 175.0f + flicker * 1.2f;
    float rOuter = 320.0f + flicker * 1.5f;
    float rFar   = 1500.0f; // Covers entire visible viewport

    // 1. Soft Warm & Cyan Lantern Light Aura (Concentric circles around player)
    drawCircle(px, py, rInner * 0.45f, 24, 1.0f, 0.95f, 0.75f, 0.22f); // Warm center
    drawCircle(px, py, rInner * 0.85f, 28, 0.35f, 0.80f, 1.0f,  0.14f); // Cyan fringe
    drawCircle(px, py, rMid   * 0.75f, 32, 0.15f, 0.45f, 0.90f, 0.08f); // Soft twilight aura

    // 2. Darkness Gradient Radial Mesh
    // Inner boundary (rInner) is 100% transparent (alpha = 0.0) -> light shines through
    // Mid ring (rMid) transitions to soft shadow (alpha = 0.65)
    // Outer perimeter transitions to impenetrable dungeon darkness (alpha = 0.96)
    const int SEGMENTS = 40;

    // Ring A: Inner Clear -> Mid Shadow
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= SEGMENTS; ++i) {
        float theta = 2.0f * 3.14159265f * (float)i / (float)SEGMENTS;
        float cosT = cos(theta);
        float sinT = sin(theta);

        // Inner vertex (transparent)
        glColor4f(0.015f, 0.015f, 0.025f, 0.0f);
        glVertex2f(px + cosT * rInner, py + sinT * rInner);

        // Mid vertex (translucent shadow)
        glColor4f(0.015f, 0.015f, 0.025f, 0.65f);
        glVertex2f(px + cosT * rMid, py + sinT * rMid);
    }
    glEnd();

    // Ring B: Mid Shadow -> Deep Darkness
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= SEGMENTS; ++i) {
        float theta = 2.0f * 3.14159265f * (float)i / (float)SEGMENTS;
        float cosT = cos(theta);
        float sinT = sin(theta);

        // Mid vertex
        glColor4f(0.015f, 0.015f, 0.025f, 0.65f);
        glVertex2f(px + cosT * rMid, py + sinT * rMid);

        // Outer vertex (dense darkness)
        glColor4f(0.015f, 0.015f, 0.025f, 0.95f);
        glVertex2f(px + cosT * rOuter, py + sinT * rOuter);
    }
    glEnd();

    // Ring C: Deep Darkness -> Far Screen Viewport Edge
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= SEGMENTS; ++i) {
        float theta = 2.0f * 3.14159265f * (float)i / (float)SEGMENTS;
        float cosT = cos(theta);
        float sinT = sin(theta);

        glColor4f(0.015f, 0.015f, 0.025f, 0.95f);
        glVertex2f(px + cosT * rOuter, py + sinT * rOuter);

        glColor4f(0.010f, 0.010f, 0.020f, 0.97f);
        glVertex2f(px + cosT * rFar, py + sinT * rFar);
    }
    glEnd();

    // 3. Screen Damage Flash Overlay (when player is damaged)
    if (damageFlashTimer > 0.0f) {
        float flashAlpha = (damageFlashTimer / 0.35f) * 0.45f;
        drawRect(cameraX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.85f, 0.08f, 0.08f, flashAlpha);
    }
}

// -----------------------------------------------------------------------------
// Level Renderers
// -----------------------------------------------------------------------------
void drawLevel1() {
    // Level 1: Forgotten Ruins
    // Stone masonry background arches (Demonstrates layered aesthetics)
    for (float x = 200.0f; x < LEVEL_WIDTH; x += 380.0f) {
        drawRect(x, 100.0f, 22.0f, 320.0f, 0.06f, 0.08f, 0.12f);
        drawRect(x + 100.0f, 100.0f, 22.0f, 320.0f, 0.06f, 0.08f, 0.12f);
        // Arch bridge
        drawRect(x, 400.0f, 122.0f, 20.0f, 0.07f, 0.09f, 0.14f);
    }

    // Platforms (Stone ruins with mossy highlights)
    for (size_t i = 0; i < platforms.size(); ++i) {
        const Rect& p = platforms[i];
        // Main block
        drawRect(p.x, p.y, p.w, p.h, 0.16f, 0.18f, 0.22f);
        // Top edge moss/stone highlight
        drawRect(p.x, p.y + p.h - 5.0f, p.w, 5.0f, 0.24f, 0.32f, 0.26f);
        // Subtle brick seam lines
        glColor3f(0.11f, 0.12f, 0.15f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        for (float bx = p.x + 40.0f; bx < p.x + p.w; bx += 40.0f) {
            glVertex2f(bx, p.y);
            glVertex2f(bx, p.y + p.h - 5.0f);
        }
        glEnd();
        glLineWidth(1.0f);
    }
}

void drawLevel2() {
    // Level 2: Collapsed Hall
    // Broken columns & dangerous background fissures
    for (float x = 150.0f; x < LEVEL_WIDTH; x += 320.0f) {
        // Fractured ruins
        drawRect(x, 80.0f, 26.0f, 180.0f, 0.07f, 0.06f, 0.10f);
        drawRect(x + 140.0f, 220.0f, 30.0f, 260.0f, 0.06f, 0.05f, 0.09f);
    }

    // Platforms (Fractured floating slabs)
    for (size_t i = 0; i < platforms.size(); ++i) {
        const Rect& p = platforms[i];
        drawRect(p.x, p.y, p.w, p.h, 0.18f, 0.16f, 0.22f);
        // Slate highlight
        drawRect(p.x, p.y + p.h - 5.0f, p.w, 5.0f, 0.32f, 0.28f, 0.38f);
    }
}

void drawLevel3() {
    // Level 3: Lumen Vault (Obsidian & Deep Amethyst)
    // Tower spires & ancient mystical pillars
    for (float x = 180.0f; x < LEVEL_WIDTH; x += 300.0f) {
        drawRect(x, 70.0f, 24.0f, 380.0f, 0.06f, 0.04f, 0.08f);
        drawCircle(x + 12.0f, 460.0f, 16.0f, 12, 0.14f, 0.08f, 0.20f);
    }

    // Vault Platforms (Obsidian with golden mystic trim)
    for (size_t i = 0; i < platforms.size(); ++i) {
        const Rect& p = platforms[i];
        // Obsidian core
        drawRect(p.x, p.y, p.w, p.h, 0.12f, 0.10f, 0.16f);
        // Gold mystic runes trim
        drawRect(p.x, p.y + p.h - 5.0f, p.w, 5.0f, 0.78f, 0.65f, 0.22f);
        // Base purple shadow
        drawRect(p.x, p.y, p.w, 4.0f, 0.08f, 0.06f, 0.11f);
    }
}

void drawCurrentLevel() {
    if (currentLevel == 1) drawLevel1();
    else if (currentLevel == 2) drawLevel2();
    else if (currentLevel == 3) drawLevel3();
}

// -----------------------------------------------------------------------------
// Traps Rendering (Spikes & Hazards)
// Demonstrates: Primitives (Triangles), Geometry & Color Coding
// -----------------------------------------------------------------------------
void drawTraps() {
    for (size_t i = 0; i < traps.size(); ++i) {
        const Trap& t = traps[i];
        int numSpikes = (int)(t.w / 12.0f);
        float spikeWidth = t.w / (float)numSpikes;

        for (int s = 0; s < numSpikes; ++s) {
            float sx = t.x + s * spikeWidth;

            glBegin(GL_TRIANGLES);
            if (t.type == 0) {
                // Floor Spikes (pointing UP)
                glColor3f(0.35f, 0.35f, 0.40f); // Base steel
                glVertex2f(sx, t.y);
                glVertex2f(sx + spikeWidth, t.y);
                glColor3f(0.95f, 0.20f, 0.15f); // Crimson sharp tip
                glVertex2f(sx + spikeWidth * 0.5f, t.y + t.h);
            } else {
                // Ceiling Trap / Stalactite (pointing DOWN)
                glColor3f(0.35f, 0.35f, 0.40f);
                glVertex2f(sx, t.y + t.h);
                glVertex2f(sx + spikeWidth, t.y + t.h);
                glColor3f(0.95f, 0.20f, 0.15f);
                glVertex2f(sx + spikeWidth * 0.5f, t.y);
            }
            glEnd();
        }
    }
}

// -----------------------------------------------------------------------------
// Falling Debris Rendering
// Demonstrates: Translation, Rotation, Animation
// -----------------------------------------------------------------------------
void drawDebris() {
    for (size_t i = 0; i < debrisList.size(); ++i) {
        const Debris& d = debrisList[i];

        glPushMatrix();
        // Translate to rock position & apply rotation animation
        glTranslatef(d.x, d.y, 0.0f);
        glRotatef(d.angle, 0.0f, 0.0f, 1.0f);

        // Jagged dungeon stone boulder (Polygon)
        glBegin(GL_POLYGON);
        float s = d.size * 0.5f;
        glColor3f(0.38f, 0.34f, 0.30f);
        glVertex2f(-s, -s * 0.6f);
        glVertex2f(-s * 0.4f, -s);
        glVertex2f(s * 0.8f, -s * 0.5f);
        glVertex2f(s, s * 0.4f);
        glVertex2f(s * 0.3f, s);
        glVertex2f(-s * 0.7f, s * 0.7f);
        glEnd();

        // Inner rock facet highlight
        glBegin(GL_TRIANGLES);
        glColor3f(0.50f, 0.45f, 0.40f);
        glVertex2f(-s * 0.4f, -s * 0.3f);
        glVertex2f(s * 0.4f, 0.0f);
        glVertex2f(-s * 0.2f, s * 0.6f);
        glEnd();

        glPopMatrix();
    }
}

// -----------------------------------------------------------------------------
// Treasure Chest Rendering (Normal vs Trap)
// Demonstrates: Layered Primitives, Subtle Tells for Traps
// -----------------------------------------------------------------------------
void drawTreasures() {
    for (size_t i = 0; i < treasures.size(); ++i) {
        const Treasure& tr = treasures[i];

        if (tr.isCollected) {
            // Already collected chest: draw open, looted chest
            drawRect(tr.x, tr.y, tr.w, tr.h * 0.5f, 0.22f, 0.18f, 0.15f);
            drawRect(tr.x + 2.0f, tr.y + tr.h * 0.5f, tr.w - 4.0f, 3.0f, 0.45f, 0.40f, 0.25f);
            continue;
        }

        // Floating hover animation
        float hoverY = tr.y + sin(lightFlicker * 3.0f + i) * 2.0f;

        // Chest Base (Wood body)
        drawRect(tr.x, hoverY, tr.w, tr.h * 0.65f, 0.45f, 0.28f, 0.12f);
        // Chest Lid (Arched gold-rimmed top)
        drawRect(tr.x - 1.0f, hoverY + tr.h * 0.65f, tr.w + 2.0f, tr.h * 0.35f, 0.55f, 0.34f, 0.16f);

        // Gold Corner Brackets
        drawRect(tr.x, hoverY, 4.0f, tr.h, 0.85f, 0.72f, 0.22f);
        drawRect(tr.x + tr.w - 4.0f, hoverY, 4.0f, tr.h, 0.85f, 0.72f, 0.22f);

        // Lock Buckle
        float lockX = tr.x + tr.w * 0.5f - 3.5f;
        float lockY = hoverY + tr.h * 0.40f;
        drawRect(lockX, lockY, 7.0f, 7.0f, 0.90f, 0.78f, 0.25f);

        // Lock Gem:
        // Subtle difference: Trap Chest has an ominous red glint!
        if (tr.isTrap) {
            drawCircle(lockX + 3.5f, lockY + 3.5f, 2.2f, 10, 0.95f, 0.15f, 0.15f); // Red trap gem
        } else {
            drawCircle(lockX + 3.5f, lockY + 3.5f, 2.2f, 10, 0.30f, 0.90f, 1.00f); // Pure cyan treasure gem
        }

        // Small sparkling glint
        float sparkle = abs(sin(lightFlicker * 4.0f + i * 2.0f));
        glColor4f(1.0f, 0.95f, 0.6f, sparkle);
        glBegin(GL_LINES);
        glVertex2f(tr.x + 4.0f, hoverY + tr.h + 2.0f);
        glVertex2f(tr.x + 4.0f, hoverY + tr.h + 8.0f);
        glVertex2f(tr.x + 1.0f, hoverY + tr.h + 5.0f);
        glVertex2f(tr.x + 7.0f, hoverY + tr.h + 5.0f);
        glEnd();
    }
}

// -----------------------------------------------------------------------------
// Portal Rendering (Level Exit)
// Demonstrates: Rotation, Scaling, Concentric Rings & Alpha Blending
// -----------------------------------------------------------------------------
void drawPortal() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(currentPortal.x, currentPortal.y, 0.0f);

    // 1. Outer Swirling Rune Gems (Rotation)
    glPushMatrix();
    glRotatef(portalAngle, 0.0f, 0.0f, 1.0f);
    const int RUNES = 12;
    for (int i = 0; i < RUNES; ++i) {
        float angle = 2.0f * 3.14159265f * (float)i / (float)RUNES;
        float rx = cos(angle) * (currentPortal.radius + 10.0f);
        float ry = sin(angle) * (currentPortal.radius + 10.0f);

        // Small diamond rune
        glColor4f(0.35f, 0.85f, 1.0f, 0.85f);
        glBegin(GL_POLYGON);
        glVertex2f(rx, ry + 4.0f);
        glVertex2f(rx + 3.5f, ry);
        glVertex2f(rx, ry - 4.0f);
        glVertex2f(rx - 3.5f, ry);
        glEnd();
    }
    glPopMatrix();

    // 2. Middle Pulsing Mystical Ring (Scaling + Counter Rotation)
    glPushMatrix();
    glScalef(portalPulse, portalPulse, 1.0f);
    glRotatef(-portalAngle * 1.4f, 0.0f, 0.0f, 1.0f);

    // Outer glow ring
    drawCircle(0.0f, 0.0f, currentPortal.radius, 32, 0.45f, 0.20f, 0.85f, 0.45f);
    drawCircle(0.0f, 0.0f, currentPortal.radius * 0.82f, 28, 0.20f, 0.75f, 1.0f, 0.65f);
    glPopMatrix();

    // 3. Inner Portal Vortex Core
    drawCircle(0.0f, 0.0f, currentPortal.radius * 0.55f * portalPulse, 24, 0.70f, 0.25f, 0.95f, 0.85f);
    drawCircle(0.0f, 0.0f, currentPortal.radius * 0.30f, 18, 0.35f, 0.95f, 1.0f, 0.95f);
    drawCircle(0.0f, 0.0f, currentPortal.radius * 0.12f, 12, 1.00f, 1.00f, 1.0f, 1.00f);

    glPopMatrix();
}

// -----------------------------------------------------------------------------
// Heads-Up Display (HUD)
// Rendered in static screen coordinates (glPopMatrix from camera)
// -----------------------------------------------------------------------------
void drawHUD() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Top Dark Banner
    drawRect(0.0f, SCREEN_HEIGHT - 44.0f, SCREEN_WIDTH, 44.0f, 0.04f, 0.05f, 0.08f, 0.92f);
    // Banner Accent Line
    drawRect(0.0f, SCREEN_HEIGHT - 45.0f, SCREEN_WIDTH, 2.0f, 0.25f, 0.45f, 0.65f, 0.75f);

    // Title / Game Name
    glColor3f(0.40f, 0.90f, 1.0f);
    drawText(24.0f, SCREEN_HEIGHT - 28.0f, "SHADOW LUMEN", GLUT_BITMAP_HELVETICA_18);

    // Elapsed Time (MM:SS format)
    int totalSec = (int)gameTimeSeconds;
    int mins = totalSec / 60;
    int secs = totalSec % 60;
    char timeBuffer[32];
    sprintf(timeBuffer, "TIME: %02d:%02d", mins, secs);
    glColor3f(0.85f, 0.85f, 0.90f);
    drawText(220.0f, SCREEN_HEIGHT - 28.0f, timeBuffer, GLUT_BITMAP_HELVETICA_18);

    // Player Lives (3 Geometric Hearts)
    drawText(400.0f, SCREEN_HEIGHT - 28.0f, "LIVES:", GLUT_BITMAP_HELVETICA_18);
    for (int i = 0; i < 3; ++i) {
        float hx = 480.0f + i * 26.0f;
        float hy = SCREEN_HEIGHT - 23.0f;
        drawHeart(hx, hy, 9.0f, (player.lives > i));
    }

    // Treasures Collected
    char treasureBuffer[32];
    sprintf(treasureBuffer, "TREASURE: %02d", totalTreasuresCollected);
    glColor3f(0.95f, 0.80f, 0.20f); // Gold
    drawText(620.0f, SCREEN_HEIGHT - 28.0f, treasureBuffer, GLUT_BITMAP_HELVETICA_18);

    // Current Level Progress
    char levelBuffer[32];
    sprintf(levelBuffer, "LEVEL: %d/3", currentLevel);
    glColor3f(0.70f, 0.75f, 0.90f);
    drawText(840.0f, SCREEN_HEIGHT - 28.0f, levelBuffer, GLUT_BITMAP_HELVETICA_18);
}

// -----------------------------------------------------------------------------
// Name Input Screen
// -----------------------------------------------------------------------------
void drawNameInputScreen() {
    // Atmospheric dark background
    drawRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.03f, 0.04f, 0.06f, 1.0f);

    // Background decorative tower pillars
    for (float x = 60.0f; x < SCREEN_WIDTH; x += 160.0f) {
        drawRect(x, 0.0f, 16.0f, SCREEN_HEIGHT, 0.06f, 0.07f, 0.10f, 1.0f);
    }

    // Title Card
    glColor3f(0.35f, 0.92f, 1.0f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 480.0f, "S H A D O W   L U M E N", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.65f, 0.70f, 0.82f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 445.0f, "A 2D Dungeon Exploration in Darkness", GLUT_BITMAP_HELVETICA_18);

    // Name Prompt Card Box
    float boxW = 460.0f;
    float boxH = 140.0f;
    float boxX = (SCREEN_WIDTH - boxW) * 0.5f;
    float boxY = 250.0f;

    drawRect(boxX, boxY, boxW, boxH, 0.08f, 0.10f, 0.15f, 0.95f);
    // Border
    glColor3f(0.28f, 0.55f, 0.75f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(boxX, boxY);
    glVertex2f(boxX + boxW, boxY);
    glVertex2f(boxX + boxW, boxY + boxH);
    glVertex2f(boxX, boxY + boxH);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(0.90f, 0.90f, 0.95f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, boxY + 95.0f, "ENTER PLAYER NAME:", GLUT_BITMAP_HELVETICA_18);

    // Player input string display with blinking cursor
    string displayInput = ">  " + (playerName.empty() ? "_" : playerName);
    int blink = (int)(lightFlicker * 3.0f);
    if (!playerName.empty() && (blink % 2 == 0)) {
        displayInput += "_";
    }

    glColor3f(1.0f, 0.85f, 0.25f); // Bright gold input
    drawTextCenter(SCREEN_WIDTH * 0.5f, boxY + 45.0f, displayInput, GLUT_BITMAP_HELVETICA_18);

    // Controls guide card
    float ctrlY = 160.0f;
    glColor3f(0.55f, 0.65f, 0.75f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, ctrlY + 30.0f,
                   "CONTROLS: [A / D / Arrows] Move    [W / Space] Jump    [S / Down] Duck",
                   GLUT_BITMAP_9_BY_15);

    // Pulsing Start Prompt
    float pulse = 0.7f + 0.3f * abs(sin(lightFlicker * 2.5f));
    glColor4f(0.25f, 0.95f, 0.55f, pulse);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 90.0f, "[ PRESS ENTER TO ENTER THE DUNGEON ]", GLUT_BITMAP_HELVETICA_18);
}

// -----------------------------------------------------------------------------
// Game Over Screen
// -----------------------------------------------------------------------------
void drawGameOverScreen() {
    // Red-tinted darkness
    drawRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.08f, 0.02f, 0.02f, 0.92f);

    glColor3f(0.95f, 0.15f, 0.15f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 450.0f, "G A M E   O V E R", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.75f, 0.65f, 0.65f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 405.0f, "You were consumed by the darkness.", GLUT_BITMAP_HELVETICA_18);

    // Statistics card
    float cardW = 380.0f;
    float cardH = 130.0f;
    float cardX = (SCREEN_WIDTH - cardW) * 0.5f;
    float cardY = 220.0f;

    drawRect(cardX, cardY, cardW, cardH, 0.12f, 0.04f, 0.05f, 0.90f);
    // Border
    glColor3f(0.70f, 0.20f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(cardX, cardY);
    glVertex2f(cardX + cardW, cardY);
    glVertex2f(cardX + cardW, cardY + cardH);
    glVertex2f(cardX, cardY + cardH);
    glEnd();
    glLineWidth(1.0f);

    char statBuf[64];
    sprintf(statBuf, "Player: %s", playerName.c_str());
    glColor3f(0.90f, 0.85f, 0.85f);
    drawText(cardX + 30.0f, cardY + 90.0f, statBuf, GLUT_BITMAP_HELVETICA_18);

    sprintf(statBuf, "Level Reached: %d/3", currentLevel);
    drawText(cardX + 30.0f, cardY + 60.0f, statBuf, GLUT_BITMAP_HELVETICA_18);

    sprintf(statBuf, "Treasures Collected: %02d", totalTreasuresCollected);
    glColor3f(0.95f, 0.80f, 0.20f);
    drawText(cardX + 30.0f, cardY + 30.0f, statBuf, GLUT_BITMAP_HELVETICA_18);

    // Options
    glColor3f(0.40f, 0.85f, 1.0f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 150.0f, "[ 1 / R ]  RETRY - SAME NAME", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.85f, 0.85f, 0.90f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 110.0f, "[ 2 / N ]  NEW NAME", GLUT_BITMAP_HELVETICA_18);
}

// -----------------------------------------------------------------------------
// Leaderboard & Victory Screen
// -----------------------------------------------------------------------------
void drawLeaderboardScreen() {
    // Deep starry obsidian background
    drawRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.02f, 0.03f, 0.06f, 1.0f);

    // Mystic header
    glColor3f(0.30f, 0.95f, 0.70f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 550.0f, "D U N G E O N   C O N Q U E R E D !", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.85f, 0.75f, 0.25f);
    drawTextCenter(SCREEN_WIDTH * 0.5f, 520.0f, "SHADOW LUMEN — LEADERBOARD", GLUT_BITMAP_HELVETICA_18);

    // Leaderboard Table Box
    float tableW = 680.0f;
    float tableH = 340.0f;
    float tableX = (SCREEN_WIDTH - tableW) * 0.5f;
    float tableY = 130.0f;

    drawRect(tableX, tableY, tableW, tableH, 0.06f, 0.08f, 0.12f, 0.92f);

    // Border
    glColor3f(0.25f, 0.55f, 0.80f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(tableX, tableY);
    glVertex2f(tableX + tableW, tableY);
    glVertex2f(tableX + tableW, tableY + tableH);
    glVertex2f(tableX, tableY + tableH);
    glEnd();
    glLineWidth(1.0f);

    // Table Header Row
    drawRect(tableX, tableY + tableH - 35.0f, tableW, 35.0f, 0.10f, 0.14f, 0.22f, 1.0f);
    glColor3f(0.40f, 0.85f, 1.0f);
    drawText(tableX + 30.0f,  tableY + tableH - 24.0f, "RANK",      GLUT_BITMAP_9_BY_15);
    drawText(tableX + 110.0f, tableY + tableH - 24.0f, "NAME",      GLUT_BITMAP_9_BY_15);
    drawText(tableX + 290.0f, tableY + tableH - 24.0f, "TIME",      GLUT_BITMAP_9_BY_15);
    drawText(tableX + 430.0f, tableY + tableH - 24.0f, "TREASURE",  GLUT_BITMAP_9_BY_15);
    drawText(tableX + 570.0f, tableY + tableH - 24.0f, "SCORE",     GLUT_BITMAP_9_BY_15);

    // Draw Entries (Up to top 7)
    int count = min((int)leaderboard.size(), 7);
    for (int i = 0; i < count; ++i) {
        float rowY = tableY + tableH - 65.0f - i * 36.0f;

        // Alternate row background highlight
        if (i % 2 == 1) {
            drawRect(tableX + 4.0f, rowY - 6.0f, tableW - 8.0f, 30.0f, 0.08f, 0.10f, 0.16f, 0.6f);
        }

        char rankBuf[16];
        sprintf(rankBuf, "%2d", i + 1);

        char treasureBuf[16];
        sprintf(treasureBuf, "%d", leaderboard[i].treasure);

        char scoreBuf[32];
        sprintf(scoreBuf, "%d", leaderboard[i].score);

        // Highlight top rank or player entry
        if (i == 0) {
            glColor3f(1.0f, 0.85f, 0.20f); // Gold for #1
        } else if (leaderboard[i].name == playerName) {
            glColor3f(0.35f, 0.95f, 0.85f); // Cyan highlight for current player
        } else {
            glColor3f(0.85f, 0.88f, 0.92f);
        }

        drawText(tableX + 40.0f,  rowY, rankBuf,                     GLUT_BITMAP_9_BY_15);
        drawText(tableX + 110.0f, rowY, leaderboard[i].name,         GLUT_BITMAP_9_BY_15);
        drawText(tableX + 295.0f, rowY, leaderboard[i].timeStr,      GLUT_BITMAP_9_BY_15);
        drawText(tableX + 460.0f, rowY, treasureBuf,                 GLUT_BITMAP_9_BY_15);
        drawText(tableX + 570.0f, rowY, scoreBuf,                    GLUT_BITMAP_9_BY_15);
    }

    // Play again options
    glColor3f(0.35f, 0.90f, 1.0f);
    drawTextCenter(SCREEN_WIDTH * 0.5f - 180.0f, 65.0f, "[ 1 / R ]  SAME NAME", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.85f, 0.85f, 0.90f);
    drawTextCenter(SCREEN_WIDTH * 0.5f + 180.0f, 65.0f, "[ 2 / N ]  NEW NAME", GLUT_BITMAP_HELVETICA_18);
}
