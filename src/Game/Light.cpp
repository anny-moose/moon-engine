#include "Light.h"


// Move a light and mark it as dirty so that we update it's mask next frame

void Light::MoveLight(float x, float y)
{
    dirty = true;
    position.x = x;
    position.y = y;

    // update the cached bounds
    bounds.x = x - outerRadius;
    bounds.y = y - outerRadius;
}


// Compute a shadow volume for the edge
// It takes the edge and projects it back by the light radius and turns it into a quad

void Light::ComputeShadowVolumeForEdge(Vector2 sp, Vector2 ep)
{
    if (shadowCount >= MAX_SHADOWS) return;

    float extension = outerRadius * 20;

    Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, position));
    Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

    Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, position));
    Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

    shadows[shadowCount].vertices[0] = sp;
    shadows[shadowCount].vertices[1] = ep;
    shadows[shadowCount].vertices[2] = epProjection;
    shadows[shadowCount].vertices[3] = spProjection;

    shadowCount++;
}

// Draw the light and shadows to the mask for a light

void Light::DrawLightMask()
{
    // Use the light mask
    BeginTextureMode(mask);

        ClearBackground(WHITE);

        BeginMode2D(camera);

        // Force the blend mode to only set the alpha of the destination
        rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
        rlSetBlendMode(BLEND_CUSTOM);

        // If we are valid, then draw the light radius to the alpha mask
        if (valid) DrawCircleGradient((int)position.x, (int)position.y, outerRadius, ColorAlpha(WHITE, 0), WHITE);

        rlDrawRenderBatchActive();

        // Cut out the shadows from the light radius by forcing the alpha to maximum
        rlSetBlendMode(BLEND_ALPHA);
        rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
        rlSetBlendMode(BLEND_CUSTOM);

        // Draw the shadows to the alpha mask
        for (int i = 0; i < shadowCount; i++)
        {
            DrawTriangleFan(shadows[i].vertices, 4, WHITE);
        }

        rlDrawRenderBatchActive();

        // Go back to normal blend mode
        rlSetBlendMode(BLEND_ALPHA);

        EndMode2D();

    EndTextureMode();
}

// See if a light needs to update it's mask
bool Light::UpdateLight(const std::vector<Wall>& walls,  bool resolution_changed)
{
    if (!active) return false;

    if (!dirty && !resolution_changed) return false;

    dirty = false;
    shadowCount = 0;
    valid = false;

    if (resolution_changed) {
        UnloadRenderTexture(mask);
        mask = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    for (const auto& wall : walls)
    {
        // Are we in a box? if so we are not valid
        if (CheckCollisionPointRec(position, wall.bound)) return false;

        // If this box is outside our bounds, we can skip it
        if (!CheckCollisionRecs(bounds, wall.bound)) continue;

        // Check the edges that are on the same side we are, and cast shadow volumes out from them

        // Top
        Vector2 sp = (Vector2){ wall.bound.x, wall.bound.y };
        Vector2 ep = (Vector2){ wall.bound.x + wall.bound.width, wall.bound.y };

        if (position.y > ep.y) ComputeShadowVolumeForEdge(sp, ep);

        // Right
        sp = ep;
        ep.y += wall.bound.height;
        if (position.x < ep.x) ComputeShadowVolumeForEdge(sp, ep);

        // Bottom
        sp = ep;
        ep.x -= wall.bound.width;
        if (position.y < ep.y) ComputeShadowVolumeForEdge(sp, ep);

        // Left
        sp = ep;
        ep.y -= wall.bound.height;
        if (position.x > ep.x) ComputeShadowVolumeForEdge(sp, ep);

        // The box itself
        shadows[shadowCount].vertices[0] = (Vector2){ wall.bound.x, wall.bound.y };
        shadows[shadowCount].vertices[1] = (Vector2){ wall.bound.x, wall.bound.y + wall.bound.height };
        shadows[shadowCount].vertices[2] = (Vector2){ wall.bound.x + wall.bound.width, wall.bound.y + wall.bound.height };
        shadows[shadowCount].vertices[3] = (Vector2){ wall.bound.x + wall.bound.width, wall.bound.y };
        shadowCount++;
    }

    valid = true;

    DrawLightMask();

    return true;
}


void Light::RenderLightMask() {
    DrawTextureRec(mask.texture, (Rectangle){ 0, 0, (float)GetScreenWidth(), -(float)GetScreenHeight() }, camera.target - camera.offset, ColorAlpha(BLACK, 1.0f));
}
