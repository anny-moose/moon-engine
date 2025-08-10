#define RLGL_SRC_ALPHA 0x0302
#define RLGL_MIN 0x8007
#define RLGL_MAX 0x8008

#define MAX_SHADOWS 300         // 100 walls would be able to cast up to two shadow volumes for the edges it is away from, and one for the box itself

#pragma once

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "Map.h"

typedef struct ShadowGeometry {
    Vector2 vertices[4];
} ShadowGeometry;

class Light {
    bool valid;                 // Is this light in a valid position?
    float outerRadius;          // The distance the light touches
    bool dirty;                 // Does this light need to be updated?
    Rectangle bounds;           // A cached rectangle of the light bounds to help with culling
    bool active;                // Is this light active?
    Vector2 position;           // Light position
    RenderTexture mask;         // Alpha mask for the light
    ShadowGeometry shadows[MAX_SHADOWS];
    int shadowCount;

    Camera2D& camera;

public:
    void set_dirty() { dirty = true; }
    // Rectangle get_bounds() const { return bounds; }
    // bool is_active() const { return active; }
    // Vector2 get_position() const { return position; }


    explicit Light(float x, float y, float radius, Camera2D& camera) : camera(camera) {
        active = true;
        valid = false;  // The light must prove it is valid
        mask = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        outerRadius = radius;
        shadowCount = 0;

        bounds.width = radius * 2;
        bounds.height = radius * 2;

        MoveLight(x, y);

        // Force the render texture to have something in it
        DrawLightMask();
    }

    void MoveLight(float x, float y);
    void ComputeShadowVolumeForEdge(Vector2 sp, Vector2 ep);
    void DrawLightMask();
    bool UpdateLight(const std::vector<Wall>&, bool resolution_changed);

    void RenderLightMask();

    void UnloadRenderMask() { UnloadRenderTexture(mask); }
};
