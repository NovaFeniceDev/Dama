#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define BOARD_SIZE 8
#define WINDOW_SIZE 800
#define SQ_SIZE WINDOW_SIZE / BOARD_SIZE

void initBoard(int board[BOARD_SIZE][BOARD_SIZE]);
void printBoard(int board[BOARD_SIZE][BOARD_SIZE]);
SDL_Renderer* initWindow();
void handleEvents(SDL_Renderer* renderer,int board[BOARD_SIZE][BOARD_SIZE]);
int getMoves(SDL_Point moves[8], int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void drawScene(SDL_Renderer* renderer, int board[BOARD_SIZE][BOARD_SIZE]);

SDL_Texture* textures[5];
SDL_Point moves[8];
int n_moves = 0;

int main(){
    /*
        -1 Nessun pezzo
        0 Pezzo bianco
        1 Pezzo nero
        2 Dama bianca
        3 Dama nera
    */
   
    int board[BOARD_SIZE][BOARD_SIZE];


    initBoard(board);
    SDL_Renderer* renderer = initWindow();
    
    textures[0] = IMG_LoadTexture(renderer,"assets/white.png");
    textures[1] = IMG_LoadTexture(renderer,"assets/black.png");
    // textures[2] = IMG_Load("assets/");
    // textures[3] = IMG_Load("assets/");
    textures[4] = IMG_LoadTexture(renderer,"assets/move.png");
    handleEvents(renderer,board);

}

void initBoard(int board[BOARD_SIZE][BOARD_SIZE]){
    for(int y = 0; y<BOARD_SIZE; y++){
        for(int x = 0; x<BOARD_SIZE; x++){
            board[x][y] = -1;
            int even = (x + y) % 2; 
            if( even && y < BOARD_SIZE/2 - 1)
                board[x][y] = 1;
            if( even && y > BOARD_SIZE/2)
                board[x][y] = 0;
        }
    }
}

void printBoard(int board[BOARD_SIZE][BOARD_SIZE]){
    for(int y = 0; y<BOARD_SIZE; y++){
        for(int x = 0; x<BOARD_SIZE; x++){
            printf(" %02d ", board[x][y]);
        }
        printf("\n");
    }
}

SDL_Renderer* initWindow(){
    SDL_Window* window = SDL_CreateWindow("Dama",0,0,WINDOW_SIZE,WINDOW_SIZE,0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0); 
    return renderer;
}

void handleEvents(SDL_Renderer* renderer,int board[BOARD_SIZE][BOARD_SIZE]){
    SDL_Event event;
    int running = 1;
    
    while (running){
        drawScene(renderer,board);
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    running = 0;
                break;
                case SDL_KEYDOWN:
                    
                break;
                case SDL_MOUSEBUTTONDOWN:
                    n_moves = 0;
                    int x,y;
                    SDL_GetMouseState(&x,&y);
                    x/= SQ_SIZE;
                    y/= SQ_SIZE;
                    int piece = board[x][y];
                    int enemy = (piece + 1) % 2;
                    //Se il pezzo è bianco inverti le direzioni 'su' e 'destra'
                    int up = -1;
                    int right = -1;
                    if(piece != -1){
                        int moveX, moveY;
                        if(piece % 2){
                            up = 1;
                            right = 1;
                        }
                        moveX = x+right;
                        moveY = y+up;
                        if(board[moveX][moveY] == -1){
                            moves[n_moves] = (SDL_Point){moveX,moveY};
                            printf("available move: %d: %d",moveX,moveY);
                            n_moves++;
                        }
                        moveX = x-right;
                        moveY = y+up;
                        if(board[moveX][moveY] == -1){
                            moves[n_moves] = (SDL_Point){moveX,moveY};
                            printf("available move: %d: %d",moveX,moveY);
                            n_moves++;
                        }
                        
                        moveX = x+right;
                        moveY = y+up; 
                        if(board[moveX][moveY] == enemy){
                        moveX = x+2*right;
                        moveY = y+2*up; 
                            if(board[moveX][moveY] == -1){
                                moves[n_moves] = (SDL_Point){moveX,moveY};
                                n_moves++;
                            }
                        }

                        printf("%d\n",n_moves);
                    }
                break;
            }
        }
    }
}

void drawScene(SDL_Renderer* renderer, int board[BOARD_SIZE][BOARD_SIZE]){
    SDL_SetRenderDrawColor(renderer,100,40,120,255);
    
    SDL_Rect rect = (SDL_Rect){0,0,WINDOW_SIZE/BOARD_SIZE,WINDOW_SIZE/BOARD_SIZE};
    for(int y = 0; y<BOARD_SIZE; y++){
        for(int x = 0; x<BOARD_SIZE; x++){
            if((x+y) % 2)
                SDL_SetRenderDrawColor(renderer,231,103,21,255);
            else
                SDL_SetRenderDrawColor(renderer,100,40,120,255);
                
            SDL_RenderFillRect(renderer,&rect);
            if(board[x][y] != -1)
                SDL_RenderCopy(renderer,textures[board[x][y]],NULL,&rect);

            (&rect)->x += WINDOW_SIZE/BOARD_SIZE;
        }
        (&rect)->x = 0;
        (&rect)->y += WINDOW_SIZE/BOARD_SIZE;
    }
    for(int i = 0; i<n_moves; i++){
        SDL_Point point = moves[i];
        SDL_Rect rect = {point.x * SQ_SIZE, point.y*SQ_SIZE,SQ_SIZE,SQ_SIZE};
        SDL_RenderCopy(renderer,textures[4],NULL,&rect);
    }
}

int getMoves(SDL_Point moves[8], int x, int y, int board[BOARD_SIZE][BOARD_SIZE]){
    
    return 0;
}
