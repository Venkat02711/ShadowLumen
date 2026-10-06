// =============================================================================
// player.cpp — Player physics, movement, controls, state & rendering
// Computer Graphics Project: SHADOW LUMEN
// =============================================================================

#include "game.h"

using namespace std;

void initPlayer() {
    player.width = 24.0f;
    player.normalHeight = 44.0f;
    player.duckHeight = 24.0f;
    player.height = player.normalHeight;

    player.x = 80.0f;
    player.y = 120.0f;
    player.vx = 0.0f;
    player.vy = 0.0f;

    player.onGround = false;
    player.facingRight = true;
    player.state = P_IDLE;
    player.animTimer = 0.0f;

    player.lives = 3;
    player.isInvulnerable = false;
    player.invulnerableTimer = 0.0f;
}

void resetPlayerForLevel(int level) {
    player.height = player.normalHeight;
    player.x = 80.0f;
    player.y = 120.0f;
    player.vx = 0.0f;
    player.vy = 0.0f;
    player.onGround = false;
    player.facingRight = true;
    player.state = P_IDLE;
    player.isInvulnerable = false;
    player.invulnerableTimer = 0.0f;
}

void playerTakeDamage() {
    if (player.isInvulnerable) return;

    player.lives--;
    player.isInvulnerable = true;
    player.invulnerableTimer = 1.8f; // 1.8 seconds invulnerability
    damageFlashTimer = 0.35f;       // Trigger red screen flash

    // Knockback
    player.vy = 7.0f;
    if (player.facingRight) {
        player.vx = -4.0f;
    } else {
        player.vx = 4.0f;
    }

    if (player.lives <= 0) {
        gameState = STATE_GAME_OVER;
    }
}

void updatePlayer() {
    bool moving = false;

    // 1. Ducking check (can only duck when on ground)
    if (keyDuck && player.onGround) {
        player.height = player.duckHeight;
        player.state = P_DUCKING;
    } else {
        player.height = player.normalHeight;
    }

    // 2. Horizontal movement
    player.vx = 0.0f;
    if (keyLeft) {
        player.vx -= MOVE_SPEED;
        player.facingRight = false;
        moving = true;
    }
    if (keyRight) {
        player.vx += MOVE_SPEED;
        player.facingRight = true;
        moving = true;
    }

    // Crawl slower if ducking while moving
    if (player.state == P_DUCKING) {
        player.vx *= 0.35f;
    }

    // 3. Jump handling
    if (keyJump && player.onGround && player.state != P_DUCKING) {
        player.vy = JUMP_FORCE;
        player.onGround = false;
        player.state = P_JUMPING;
    }

    // 4. Apply Gravity
    player.vy -= GRAVITY;
    if (player.vy < -15.0f) {
        player.vy = -15.0f;
    }

    // 5. Update X position and handle horizontal platform collisions
    player.x += player.vx;

    // Level world boundaries
    if (player.x < 30.0f) player.x = 30.0f;
    if (player.x > LEVEL_WIDTH - 60.0f) player.x = LEVEL_WIDTH - 60.0f;

    for (size_t i = 0; i < platforms.size(); ++i) {
        const Rect& plat = platforms[i];
        if (checkAABB(player.x, player.y, player.width, player.height,
                      plat.x, plat.y, plat.w, plat.h)) {
            if (player.vx > 0.0f) {
                player.x = plat.x - player.width;
            } else if (player.vx < 0.0f) {
                player.x = plat.x + plat.w;
            }
        }
    }

    // 6. Update Y position and handle vertical platform collisions
    player.y += player.vy;
    player.onGround = false;

    for (size_t i = 0; i < platforms.size(); ++i) {
        const Rect& plat = platforms[i];
        if (checkAABB(player.x, player.y, player.width, player.height,
                      plat.x, plat.y, plat.w, plat.h)) {
            if (player.vy <= 0.0f) {
                // Landing on platform top
                player.y = plat.y + plat.h;
                player.vy = 0.0f;
                player.onGround = true;
            } else if (player.vy > 0.0f) {
                // Hitting ceiling / bottom of platform
                player.y = plat.y - player.height;
                player.vy = 0.0f;
            }
        }
    }

    // Bottomless pit check (fell off map)
    if (player.y < -40.0f) {
        playerTakeDamage();
        // Respawn at safe start location
        player.x = 80.0f;
        player.y = 140.0f;
        player.vy = 0.0f;
    }

    // 7. Update animation state
    if (!player.onGround) {
        player.state = P_JUMPING;
    } else if (keyDuck) {
        player.state = P_DUCKING;
    } else if (moving) {
        player.state = P_WALKING;
        player.animTimer += 0.25f;
    } else {
        player.state = P_IDLE;
        player.animTimer = 0.0f;
    }

    // 8. Invulnerability flashing timer
    if (player.isInvulnerable) {
        player.invulnerableTimer -= 0.016f; // ~60fps step
        if (player.invulnerableTimer <= 0.0f) {
            player.isInvulnerable = false;
        }
    }
}

// -----------------------------------------------------------------------------
// Render Player using 2D geometric primitives
// Demonstrates: Primitives, Translation, Rotation, Scaling & Animation
// -----------------------------------------------------------------------------
void drawPlayer() {
    // If invulnerable, flicker visibility (skip rendering on alternate frames)
    if (player.isInvulnerable) {
        int blink = (int)(player.invulnerableTimer * 20.0f);
        if (blink % 2 == 0) {
            return;
        }
    }

    glPushMatrix();

    // 1. Coordinate Transformation: Translate to player world position
    glTranslatef(player.x, player.y, 0.0f);

    // 2. Facing direction flip (Scaling along X axis)
    if (!player.facingRight) {
        glTranslatef(player.width, 0.0f, 0.0f);
        glScalef(-1.0f, 1.0f, 1.0f);
    }

    // Ground Contact Shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    drawCircle(player.width * 0.5f, 2.0f, 11.0f, 14, 0.0f, 0.0f, 0.0f, 0.4f);

    // Determine vertical heights based on ducking state
    bool ducking = (player.state == P_DUCKING);
    float bodyBaseY  = ducking ? 6.0f  : 12.0f;
    float bodyTopY   = ducking ? 20.0f : 32.0f;
    float headCenter = ducking ? 22.0f : 36.0f;

    // --- LEGS & BOOTS (Demonstrates Animation & Rotation) ---
    glColor3f(0.12f, 0.14f, 0.20f); // Dark pants

    if (player.state == P_WALKING) {
        float legSwing = sin(player.animTimer) * 16.0f;

        // Left Leg (Rotated back)
        glPushMatrix();
        glTranslatef(8.0f, bodyBaseY, 0.0f);
        glRotatef(legSwing, 0.0f, 0.0f, 1.0f);
        drawRect(-3.0f, -12.0f, 6.0f, 12.0f, 0.15f, 0.17f, 0.24f);
        // Boot
        drawRect(-3.0f, -14.0f, 8.0f, 4.0f, 0.25f, 0.18f, 0.12f);
        glPopMatrix();

        // Right Leg (Rotated forward)
        glPushMatrix();
        glTranslatef(16.0f, bodyBaseY, 0.0f);
        glRotatef(-legSwing, 0.0f, 0.0f, 1.0f);
        drawRect(-3.0f, -12.0f, 6.0f, 12.0f, 0.12f, 0.14f, 0.20f);
        // Boot
        drawRect(-3.0f, -14.0f, 8.0f, 4.0f, 0.25f, 0.18f, 0.12f);
        glPopMatrix();
    } else if (player.state == P_JUMPING) {
        // Tucked jump legs
        drawRect(5.0f, bodyBaseY - 6.0f, 5.0f, 8.0f, 0.15f, 0.17f, 0.24f);
        drawRect(14.0f, bodyBaseY - 4.0f, 5.0f, 6.0f, 0.12f, 0.14f, 0.20f);
        // Boots angled
        drawRect(5.0f, bodyBaseY - 9.0f, 7.0f, 4.0f, 0.25f, 0.18f, 0.12f);
        drawRect(14.0f, bodyBaseY - 7.0f, 7.0f, 4.0f, 0.25f, 0.18f, 0.12f);
    } else if (ducking) {
        // Crouched legs
        drawRect(3.0f, 0.0f, 9.0f, 6.0f, 0.15f, 0.17f, 0.24f);
        drawRect(12.0f, 0.0f, 9.0f, 6.0f, 0.12f, 0.14f, 0.20f);
        // Boots flat on floor
        drawRect(2.0f, 0.0f, 11.0f, 4.0f, 0.25f, 0.18f, 0.12f);
        drawRect(11.0f, 0.0f, 11.0f, 4.0f, 0.25f, 0.18f, 0.12f);
    } else {
        // Standing legs
        drawRect(6.0f, 2.0f, 5.0f, 10.0f, 0.15f, 0.17f, 0.24f);
        drawRect(13.0f, 2.0f, 5.0f, 10.0f, 0.12f, 0.14f, 0.20f);
        // Boots
        drawRect(5.0f, 0.0f, 7.0f, 4.0f, 0.25f, 0.18f, 0.12f);
        drawRect(13.0f, 0.0f, 7.0f, 4.0f, 0.25f, 0.18f, 0.12f);
    }

    // --- TORSO & ADVENTURER CLOAK (Polygons & Quads) ---
    // Outer Tunic / Cloak
    glBegin(GL_POLYGON);
    glColor3f(0.18f, 0.24f, 0.35f); // Indigo cloak
    glVertex2f(4.0f, bodyBaseY);
    glVertex2f(20.0f, bodyBaseY);
    glVertex2f(18.0f, bodyTopY);
    glVertex2f(6.0f, bodyTopY);
    glEnd();

    // Inner Vest / Tunic detail
    glBegin(GL_POLYGON);
    glColor3f(0.25f, 0.30f, 0.42f);
    glVertex2f(8.0f, bodyBaseY);
    glVertex2f(16.0f, bodyBaseY);
    glVertex2f(15.0f, bodyTopY - 2.0f);
    glVertex2f(9.0f, bodyTopY - 2.0f);
    glEnd();

    // Belt & Buckle
    drawRect(5.0f, bodyBaseY + (bodyTopY - bodyBaseY) * 0.4f, 14.0f, 4.0f, 0.35f, 0.24f, 0.14f);
    drawRect(10.0f, bodyBaseY + (bodyTopY - bodyBaseY) * 0.4f - 1.0f, 4.0f, 6.0f, 0.88f, 0.75f, 0.25f); // Gold buckle

    // --- HOOD & HEAD (Circles) ---
    // Outer Dark Hood
    drawCircle(12.0f, headCenter, 8.5f, 18, 0.14f, 0.18f, 0.28f);
    // Inner Shadowed Face Aperture
    drawCircle(12.5f, headCenter - 0.5f, 6.0f, 16, 0.06f, 0.07f, 0.10f);

    // Glowing Visor / Eyes (Cyan light glint)
    glColor3f(0.35f, 0.95f, 1.0f);
    drawRect(12.0f, headCenter - 1.0f, 4.5f, 2.5f, 0.35f, 0.95f, 1.0f);

    // --- HAND & MYSTIC LANTERN ---
    float handX = 20.0f;
    float handY = bodyBaseY + 6.0f;
    float lanternX = handX + 4.0f;
    float lanternY = handY - 4.0f;

    // Arm reaching forward
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glColor3f(0.18f, 0.24f, 0.35f);
    glVertex2f(14.0f, bodyTopY - 4.0f);
    glVertex2f(handX, handY);
    glEnd();
    glLineWidth(1.0f);

    // Lantern chain & brass frame
    glBegin(GL_LINES);
    glColor3f(0.7f, 0.6f, 0.3f);
    glVertex2f(handX, handY);
    glVertex2f(lanternX, lanternY + 5.0f);
    glEnd();

    // Brass Lantern Cap & Base
    drawRect(lanternX - 3.5f, lanternY + 3.0f, 7.0f, 2.5f, 0.72f, 0.58f, 0.22f);
    drawRect(lanternX - 3.5f, lanternY - 4.5f, 7.0f, 2.5f, 0.72f, 0.58f, 0.22f);

    // Glowing Lantern Core
    drawRect(lanternX - 2.5f, lanternY - 2.0f, 5.0f, 5.0f, 1.0f, 0.92f, 0.55f);
    drawCircle(lanternX, lanternY + 0.5f, 2.5f, 10, 1.0f, 1.0f, 0.9f);

    // Small local lantern glow halo
    drawCircle(lanternX, lanternY + 0.5f, 6.0f + sin(lightFlicker * 3.0f) * 1.5f, 12,
               0.45f, 0.85f, 1.0f, 0.35f);

    glPopMatrix();
}
