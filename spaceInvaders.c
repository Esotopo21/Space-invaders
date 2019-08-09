 #include "raylib.h"

//--------------------------------------------------
//Game entities struct
//--------------------------------------------------

 typedef struct Entity{
     Rectangle source;
     Vector2 pos;
     Texture2D texture;
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

Sound fxShot;
Sound fxBoom;
Sound fxLevelUp;

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
    spaceShipTexture = LoadTexture("src/spaceship.png");
    
    InitAudioDevice();
    
    fxShot = LoadSound("src/pew.wav");
    fxBoom = LoadSound("src/boom.wav");
    fxLevelUp = LoadSound("src/coin.wav");
    
    
    
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
     int i;
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
     
     //Initiliaze bullets array
     for(i=0;i<NUM_SHOTS;i++) {
         shots[i].active = false;
     }
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
             if(activeEnemies == 0) {
                 InitEnemies();
                 PauseSound(fxLevelUp);
             }
             
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
         if(shots[i].source.y < -30) shots[i].active = false;
         
         
         if(shots[i].active) shots[i].source.y += shots[i].speed.y;
         
         //Check if a bullet hits an enemy
         for(j=0;j<NUM_ENEMIES;j++){
            
            if(CheckCollisionRecs((Rectangle){enemies[j].pos.x,enemies[j].pos.y,30,30},shots[i].source) && enemies[j].active && shots[i].active){
                enemies[j].active = false;
                activeEnemies--;
                
                shots[i].active = false;
                
                points += 100;
                
                PlaySound(fxBoom);
            }
             
         }
     }
 }
 
 void AddShot(void){
     
     //Initialize a bullet 
     
     shots[activeShots].source.x = player.pos.x + player.source.width/2-5;
     shots[activeShots].source.y = player.pos.y;
     shots[activeShots].source.width = 10;
     shots[activeShots].source.height = 10;
                 
     shots[activeShots].speed.x = 0;
     shots[activeShots].speed.y = -10;
                 
     shots[activeShots].active = true;
                 
     shots[activeShots].color = RED;
                 
     activeShots++;
     
     PlaySound(fxShot);
}

void DrawGame(void){
    int i;
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    if(!pause && !gameOver){
        //DrawPlayer
        DrawTextureRec(player.texture, player.source, player.pos, WHITE);
        
        //Draw enemies
        for(i=0;i<NUM_ENEMIES;i++){
            if(enemies[i].active) DrawTextureRec(enemies[i].texture, enemies[i].source, enemies[i].pos, WHITE);
                /*DrawRectangleRec(enemies[i].rec, enemies[i].color);*/
            
        }
        
        //Draw bullets
        for(i=0;i<NUM_SHOTS;i++){
            if(shots[i].active) DrawRectangleRec(shots[i].source, shots[i].color);
        }
        
        //Display game info
        DrawText(FormatText("points: %d \n wave: %d", points, wave), 0, 0, 50, GRAY);
    }
    
    if(pause) DrawText("\tgame pause - press p to resume", 300, screenHeight/2, 30, GRAY);
    
    if(gameOver){
        DrawText("\tGAME OVER", screenWidth/2 - MeasureText("GAME OVER", 50), screenHeight/6, 50, RED);
        DrawText(FormatText("\t Score - %d", points), screenWidth/2 - MeasureText("GAME OVER", 50), screenHeight/5+30, 30, GRAY);
        DrawText("\t Press enter to restart", screenWidth/2 - MeasureText("GAME OVER", 50), screenHeight/4+30, 30, GRAY);
    } 
    
    EndDrawing();
    
}

void UnloadGame(void){
    UnloadTexture(baseEnemyTexture);
    UnloadTexture(spaceShipTexture);
    UnloadSound(fxShot);
    UnloadSound(fxBoom);
    UnloadSound(fxLevelUp);
    CloseAudioDevice();
}

 
 