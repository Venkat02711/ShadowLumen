# Shadow Lumen

A 2D atmospheric dungeon platformer built in C++ using OpenGL and FreeGLUT as a Computer Graphics project.

## Overview

The player guides an adventurer carrying a small light source through a dark dungeon tower across 3 levels:
1. **Forgotten Ruins** — Introductory ruins with stone platforms, low spikes, and ancient architecture.
2. **Collapsed Hall** — Fractured floating platforms, deep chasms, and falling debris.
3. **Lumen Vault** — Dark obsidian stronghold with hazardous gauntlets, deceptive chests, and the final exit portal.

## Computer Graphics Concepts Demonstrated

- **2D Geometric Primitives**: Quads, triangles, lines, polygons, and circles.
- **Geometric Transformations**: Translation (`glTranslatef`), Rotation (`glRotatef`), Scaling (`glScalef`).
- **Coordinate Systems**: World-to-camera coordinate transformation with horizontal scrolling.
- **Collision Detection**: Axis-Aligned Bounding Box (AABB) intersection.
- **Animation**: Dynamic falling debris, spinning & pulsing portal vortex, flickering lantern radius, walk cycles.
- **Alpha Blending & Transparency**: Dynamic 2D lantern lighting and soft radial shadow mesh (`GL_BLEND`).

## Controls

| Action | Keys |
| :--- | :--- |
| **Move Left** | `A` or `Left Arrow` |
| **Move Right** | `D` or `Right Arrow` |
| **Jump** | `W`, `Up Arrow`, or `Space` |
| **Duck / Crawl** | `S` or `Down Arrow` |

## How to Build & Run

### Code::Blocks
1. Open `ShadowLumen.cbp` in Code::Blocks.
2. Press **F9** (or click **Build and Run**).
