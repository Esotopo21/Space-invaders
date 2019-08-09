 #include "raylib.h"

//--------------------------------------------------
//Game entities struct
//--------------------------------------------------

 typedef struct Entity{
     Rectangle source;
     Vector2 pos;
     Texture2D texture;
     Rectangle rec;
     Vector2 speed;
     bool active;
     Color color;
 }Player,Enemy,Shot;
 
 
 //-----------------------------------------------
 // Constants define
 //-----------------------------------------------
 
 #define NUM_SHOTS 300
 #define NUM_ENEMIES 24
 
 //------------------------------------------------------------------------------------
// Global Variables Declaration
//-------------------------------------------------------------------------------------

static const int screenWidth = 1200;
static const int screenHeight = 800;

static bool gameOver = false;
static bool pause = false;

static int points = 0;
static int wave = 0;
static int activeEnemies = 0;
static int activeShots = 0;

static Player player = { 0 };
static Enemy enemies[NUM_ENEMIES] = { 0 };
static Shot shots[NUM_SHOTS] = { 0 };

Texture2D baseEnemyTexture;
Texture2D spaceShipTexture;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------

static void InitGame(void);         // Initialize game
static void InitEnemies(void);      // Create enemies for the wave
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UpdateEnemies(void);    // Update enemies direction when on bounds, scale y and upgrade speed 
static void UpdateBullets(void);    //Update bullets positions
static void AddShot(void);          // Add an active shot
static void UnloadGame(void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

int main(void){
    
    InitWindow(screenWidth,screenHeight,"Space invaders");
    
    baseEnemyTexture = LoadTexture("src/enemy.png");
    //TO-DO Understand why spaceship.png can't be loaded 
    spaceShipTexture = LoadTexture("src/enemy.png");
    
    SetTargetFPS(60);

    InitGame();
    
   
     
     //Game main loop
     while(!WindowShouldClose()){
         
         // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateGame();
        DrawGame();
        //----------------------------------------------------------------------------------
     }
     
     UnloadGame();
     
     CloseWindow();
     
 }
 
 void InitGame(void){
     
     //Initialize game variables
     gameOver = false;
     pause = false;
     activeEnemies = 0;
     activeShots = 0;
     wave = 0;
     
     points = 0;
     
     //Initialize player

     //Source rectangle
     player.source.x = 0;
     player.source.y = 0;
     player.source.width = 50;
     player.source.height = 50;
     
     //Position
     player.pos.x = screenWidth/2 - player.source.width;
     player.pos.y = screenHeight - player.source.height*2;
     
     //Texture
     player.texture = spaceShipTexture;
     
     player.active = true;
     
     //Speed
     player.speed.x = 10;
     player.speed.y = 0;
     
     player.color = WHITE;
     
     //Initialize enemies
     InitEnemies();
     
 }
 
 void InitEnemies(){
    int i,j;
    
    activeEnemies = 0;
    
    //Create a 6x4 rectangle of enemies with a spacing of 5 px
    for(i=0;i<4;i++){
        for(j=0;j<6;j++){
            enemies[activeEnemies].source.x = 0;
            enemies[activeEnemies].source.y = 0;
            enemies[activeEnemies].source.width = 30;
            enemies[activeEnemies].source.height = 30;
            
            enemies[activeEnemies].pos.x = j*40+5; 
            enemies[activeEnemies].pos.y = i*40+35;
            
            enemies[activeEnemies].texture = baseEnemyTexture;
            
            enemies[activeEnemies].speed.x = 10+5*wave;
            enemies[activeEnemies].speed.y = 45;
            
            enemies[activeEnemies].active = true;
            
            enemies[activeEnemies].color = BLACK;
            
            activeEnemies++;
        }
    }
    
    //Increment wave
    wave++;
 }

 void UpdateGame(void){
     
     if(enemies[0].pos.y >= player.pos.y) gameOver = true;
     
     if(!gameOver){
         
         if(IsKeyPressed('P')) pause = !pause;
         
         if(!pause){
        
             UpdateEnemies();
             UpdateBullets();
             
             //Check if there are still enemies, otherwise a new wave starts 
             if(activeEnemies == 0) InitEnemies();
             
             //Handle inputs
             if (IsKeyDown(KEY_LEFT) && player.pos.x > 15) player.pos.x -= player.speed.x;
             if (IsKeyDown(KEY_RIGHT) && player.pos.x < screenWidth-50) player.pos.x += player.speed.x;
             if (IsKeyPressed(KEY_SPACE) && activeShots < NUM_SHOTS) AddShot();   
             
        }
    }else{
        
        //Restart the game when enter is pressed
        if (IsKeyPressed(KEY_ENTER)) InitGame();
        
    }
 }
 
 void UpdateEnemies(void){
     int i;
     
     if(enemies[0].pos.x <= 0 || enemies[NUM_ENEMIES-1].pos.x >= screenWidth-10){
         for(i=0;i<NUM_ENEMIES;i++){
             
             //Change direction
             enemies[i].speed.x = -enemies[i].speed.x;
             
             //Upgrade speed
             if(enemies[i].speed.x > 0) enemies[i].speed.x += 0.5f;
             if(enemies[i].speed.x < 0) enemies[i].speed.x -= 0.5f;
             
             //Scale y
             enemies[i].pos.y += enemies[i].speed.y;
             
         }
     }
      
      //Update enemies positions
      for(i=0;i<NUM_ENEMIES;i++){   
                 enemies[i].pos.x += enemies[i].speed.x; 
      }
 }
 
 void UpdateBullets(void){
     int i,j;
     
     for(i=0;i<NUM_SHOTS;i++){
         
         //Don't render a bullet wich is outside the screen
         if(shots[i].rec.y < -30) shots[i].active = false;
         
         
         if(shots[i].active) shots[i].rec.y += shots[i].speed.y;
         
         //Check if a bullet hits an enemy
         for(j=0;j<NUM_ENEMIES;j++){
            
            if(CheckCollisionRecs((Rectangle){enemies[j].pos.x,enemies[j].pos.y,30,30},shots[i].rec) && enemies[j].active && shots[i].active){
                enemies[j].active = false;
                activeEnemies--;
                
                shots[i].active = false;
                
                points += 100;
            }
             
         }
     }
 }
 
 void AddShot(void){
     
     //Initialize a bullet 
     
     shots[activeShots].rec.x = player.pos.x + player.source.width/2-5;
     shots[activeShots].rec.y = player.pos.y;
     shots[activeShots].rec.width = 10;
     shots[activeShots].rec.height = 10;
                 
     shots[activeShots].speed.x = 0;
     shots[activeShots].speed.y = -10;
                 
     shots[activeShots].active = true;
                 
     shots[activeShots].color = RED;
                 
     activeShots++;
     
     //PlaySound(shotFx);
}

void DrawGame(void){
    int i;
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    if(!pause && !gameOver){
        //DrawPlayer
        DrawTextureRec(player.texture, player.source, player.pos, GREEN);
        
        //Draw enemies
        for(i=0;i<NUM_ENEMIES;i++){
            if(enemies[i].active) DrawTextureRec(enemies[i].texture, enemies[i].source, enemies[i].pos, WHITE);
                /*DrawRectangleRec(enemies[i].rec, enemies[i].color);*/
            
        }
        
        //Draw bullets
        for(i=0;i<NUM_SHOTS;i++){
            if(shots[i].active) DrawRectangleRec(shots[i].rec, shots[i].color);
        }
        
        //Display game info
        DrawText(FormatText("points: %d \n wave: %d", points, wave), 0, 0, 50, GRAY);
    }
    
    if(pause) DrawText("game pause - press p to resume", 300, screenHeight/2, 30, GRAY);
    
    if(gameOver){
        DrawText("GAME OVER", screenWidth/2 - MeasureText("GAME OVER", 50), screenHeight/2, 50, RED);
        DrawText(FormatText("\tScore - %d", points), screenWidth/2 - MeasureText("Score - 0", 30), screenHeight/2+80, 30, GRAY);
        DrawText("Press enter to restart", screenWidth/2 - MeasureText("Press enter to restart", 30), screenHeight/2+160, 30, GRAY);
    } 
    
    EndDrawing();
    
}

void UnloadGame(void){
    UnloadTexture(baseEnemyTexture);
    UnloadTexture(spaceShipTexture);
}

 
 