#include "raylib.h"
#include <iostream>

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
    Color color;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

void Setup() {

}

AnimData updateAnimData(AnimData data, float deltaTime, int cols, int rows, int maxFrame) {
    data.runningTime += deltaTime;

    if (data.runningTime >= data.updateTime) {
        data.runningTime = 0;
        data.rec.x = (data.frame % cols) * data.rec.width;
        data.rec.y =  (data.frame / rows) * data.rec.height;
        data.frame++;

        if (data.frame > maxFrame) {
            data.frame = 0;
        }
    }

    return data;
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int windowWidth {512};
    const int windowHeight {380};

    InitWindow(windowWidth, windowHeight, "Dapper Dasher C++");

    //Textures
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    //Nebula data
    const int sizeOfNebulae{10};
    AnimData nebulae[sizeOfNebulae] {};
    Color nebulaColors[6] {WHITE, RED, GREEN, YELLOW, PINK, BLUE};

    //Create Nebula objects
    for (int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.x = windowWidth + i * 300;
        nebulae[i].pos.y = windowHeight - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].updateTime = 1.0 / 12.0;
        nebulae[i].runningTime = 0;
        nebulae[i].color = nebulaColors[GetRandomValue(0, 5)];
    }

    float finishLine{nebulae[sizeOfNebulae -1].pos.x};

    //nebula x velocity (pixels/s)
    int nebulaVelocity {-200};
    
    //Scarfy data
    AnimData scarfyData{
        {0.0, 0.0, scarfy.width / 6, scarfy.height},
        {windowWidth / 2 - scarfy.width / 6, windowHeight - scarfy.height},
        0,
        1.0 / 12.0,
        0
    };

    // gravity (aceleration in pixel/s^2)
    int gravity{1'000};
    int velocity {0};
    int jumpVel {-600};
    bool isInAir{};

    float bgScale{2.0};
    float bgX{};
    float mgX{};
    float fgX{};

    // Update Loop
    //--------------------------------------------------------------------------------------
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        const float deltaTime { GetFrameTime() };

        BeginDrawing();
        ClearBackground(BLACK);

        //update bg position
        bgX -= 20 * deltaTime;
        mgX -= 40 * deltaTime;
        fgX -= 80 * deltaTime;
        
        if (bgX <= -background.width * bgScale) {
            bgX = 0.0;
        }

        if (mgX <= -midground.width * bgScale) {
            mgX = 0.0;
        }

        if (fgX <= -foreground.width * bgScale) {
            fgX = 0.0;
        }

        // draw bg
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, bgScale, WHITE);
        Vector2 bg2Pos{bgX + background.width * bgScale, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, bgScale, WHITE);

        // draw mg
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, bgScale, WHITE);
        Vector2 mg2Pos{mgX + midground.width * bgScale, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, bgScale, WHITE);

        // draw fg
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, bgScale, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * bgScale, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, bgScale, WHITE);

        // ground check
        if (isOnGround(scarfyData, windowHeight))
        {
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // in air, apply gravity
            velocity += gravity * deltaTime;
            isInAir = true;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isInAir) {
            velocity += jumpVel;
        }

        //check collisions using range based loop
        bool collision {false};

        Rectangle scarfyRec {
            scarfyData.pos.x,
            scarfyData.pos.y,
            scarfyData.rec.width,
            scarfyData.rec.height
        };

        for (AnimData nebula : nebulae) {
            //Making a rectangle that represents the position on screen (rec is the position in the spritesheet)
            float padding{40}; //to remove the whitespace from the spritesheet
            Rectangle nebRec {
                nebula.pos.x + padding,
                nebula.pos.y + padding,
                nebula.rec.width - padding * 2,
                nebula.rec.height - padding * 2
            };

            if (CheckCollisionRecs(nebRec, scarfyRec)) {
                collision = true;      
            }
        }

        if (collision) {
            int fontSize{50};
            DrawText("Game Over", windowWidth/2 - MeasureText("Game Over", fontSize)/2, windowHeight /2 - fontSize/2, fontSize, WHITE);
        } else if (scarfyData.pos.x >= finishLine) {
            int fontSize{50};
            DrawText("You Win!", windowWidth/2 - MeasureText("You Win!", fontSize)/2, windowHeight /2 - fontSize/2, fontSize, GREEN);
        } else {
            //update nebulae
            for (int i = 0; i < sizeOfNebulae; i++) {
                nebulae[i] = updateAnimData(nebulae[i], deltaTime, 8, 8, 60);
                
                nebulae[i].pos.x += nebulaVelocity * deltaTime;

                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, nebulae[i].color);
            }

            //update finish line
            finishLine += nebulaVelocity * deltaTime;

            // update scarfy pos
            scarfyData.pos.y += velocity * deltaTime;
                
            if (!isInAir) {
                scarfyData = updateAnimData(scarfyData, deltaTime, 8, 1, 5);
            }

            //Draw Scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    
    CloseWindow();
}
