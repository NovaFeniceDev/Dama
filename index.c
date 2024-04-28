#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <string.h>

#define CONCAT(a, b) ({ \
    char result[256]; \
    strcpy(result, (a)); \
    strcat(result, (b)); \
    result; \
})
#define toggleTurn turn = !turn

#define BOARD_SIZE 8
#define to_grid BOARD_SIZE / WINDOW_WIDTH
#define SQUARE_SIZE WINDOW_WIDTH / BOARD_SIZE
#define IMAGE_WIDTH 200
#define IMAGE_HEIGHT 150
#define GRID_WIDTH 500
#define GRID_HEIGHT 400

/*
Controlla chi deve fare una mossa
    0 Bianco
    1 Nero
*/
bool turn = 0;

void validateMove(int board[BOARD_SIZE][BOARD_SIZE], int* n_moves, SDL_Point moves[8], SDL_Point eating[8], int x, int y) {
    int piece = board[x][y];
    if(turn != piece % 2) { return;}
    int enemy = (piece + 1) % 2;
    int n = 0;
    // Se il pezzo è bianco inverti le direzioni 'su' e 'destra'
    int up = -1;
    int right = -1;
    if (piece != -1) {

        int moveX, moveY;
        if (piece % 2) {
            up = 1;
            right = 1;
        }

        moveX = x + right;
        moveY = y + up;
        if (board[moveX][moveY] == -1) {
            moves[n] = (SDL_Point) { moveX,moveY };
            eating[n] = (SDL_Point) { -1,-1 };
            n++;
        }

        moveX = x - right;
        moveY = y + up;
        if (board[moveX][moveY] == -1) {
            moves[n] = (SDL_Point) { moveX,moveY };
            eating[n] = (SDL_Point) { -1,-1 };
            n++;
        }

        moveX = x + right;
        moveY = y + up;
        if (board[moveX][moveY] == enemy) {
            moveX = x + 2 * right;
            moveY = y + 2 * up;
            if (board[moveX][moveY] == -1) {
                moves[n] = (SDL_Point) { moveX,moveY };
                eating[n] = (SDL_Point) { x + right,y + up };
                n++;
            }
        }

        moveX = x - right;
        moveY = y + up;
        if (board[moveX][moveY] == enemy) {
            moveX = x - 2 * right;
            moveY = y + 2 * up;
            if (board[moveX][moveY] == -1) {
                moves[n] = (SDL_Point) { moveX,moveY };
                eating[n] = (SDL_Point) { x - right,y + up };
                n++;
            }
        }

        //Se il pezzo è una dama
        if(piece > 1){
            printf("il pezzo è una dama\n");
            moveX = x + right;
            moveY = y - up;

            if (board[moveX][moveY] == -1) {
                moves[n] = (SDL_Point) { moveX,moveY };
                eating[n] = (SDL_Point) { -1,-1 };
                n++;
            }

            moveX = x - right;
            moveY = y - up;
            if (board[moveX][moveY] == -1) {
                moves[n] = (SDL_Point) { moveX,moveY };
                eating[n] = (SDL_Point) { -1,-1 };
                n++;
            }

            moveX = x + right;
            moveY = y - up;
            if (board[moveX][moveY] == enemy) {
                moveX = x + 2 * right;
                moveY = y - 2 * up;
                if (board[moveX][moveY] == -1) {
                    moves[n] = (SDL_Point) { moveX,moveY };
                    eating[n] = (SDL_Point) { x + right,y - up };
                    n++;
                }
            }

            moveX = x - right;
            moveY = y - up;
            if (board[moveX][moveY] == enemy) {
                moveX = x - 2 * right;
                moveY = y - 2 * up;
                if (board[moveX][moveY] == -1) {
                    moves[n] = (SDL_Point) { moveX,moveY };
                    eating[n] = (SDL_Point) { x - right,y - up };
                    n++;
                }
            }
        }
        *n_moves = n;
    }
}


int main(int argc, char* argv[]) {
    const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
    const int BUTTON_START_WIDTH = 150, BUTTON_START_HEIGHT = 80, SPACE_BETWEEN_BUTTONS = 20;
    const int totalButtonHeight = 4 * BUTTON_START_HEIGHT;
    const int totalSpace = SPACE_BETWEEN_BUTTONS * 3;
    const int startY = (WINDOW_HEIGHT - totalButtonHeight - totalSpace) / 2;
    const char* texture[4] = {"Neo","Classic","Wood","Purple"};
    const SDL_Color colorBoards[8] = {
        (SDL_Color){235,236,208,255}, // Tema 1 bianco
        (SDL_Color){216,227,231,255}, // Tema 2 bianco
        (SDL_Color){199,171,121,255}, // Tema 3 bianco
        (SDL_Color){240,241,240,255}, // Tema 4 bianco
        (SDL_Color){115,149,82,255},  // Tema 1 nero
        (SDL_Color){119,154,175,255}, // Tema 2 nero
        (SDL_Color){124,87,52,255},   // Tema 3 nero
        (SDL_Color){132,118,186,255}, // Tema 4 nero   
    };
    int selected_theme = 0;
    SDL_Rect themeRects[4];
    SDL_Color white;
    SDL_Color black;
    
    /*
        -1 Nessun pezzo
        0 Pezzo bianco
        1 Pezzo nero
        2 Dama bianca
        3 Dama nera
    */
    int board[BOARD_SIZE][BOARD_SIZE];
    int n_moves = 0;
    SDL_Point moves[8];
    SDL_Point eating_pieces[8];

    int movingPiece = -1;
    int lastX = -1, lastY = -1;

    // Crea la scacchiera
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
    board[1][0] = 1;

    int startThemeX = WINDOW_WIDTH / 2;
    int startThemeY = WINDOW_HEIGHT / 2;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Dama", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event event;
    bool playButtonFlag = false;
    bool themeButtonFlag = false;
    bool infoButtonFlag = false;
    Uint32 startTime = SDL_GetTicks();
    int frames = 0;
    TTF_Init();

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 1024);
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Play", textColor);
    SDL_Surface* themeSurface = TTF_RenderText_Solid(font, "Theme", textColor);
    SDL_Surface* infoSurface = TTF_RenderText_Solid(font, "Info", textColor);
    SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Quit", textColor);

    SDL_Texture* textTexturePlay = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Texture* textTextureTheme = SDL_CreateTextureFromSurface(renderer, themeSurface);
    SDL_Texture* textTextureInfo = SDL_CreateTextureFromSurface(renderer, infoSurface);
    SDL_Texture* textTextureQuit = SDL_CreateTextureFromSurface(renderer, quitSurface);

    /*A ogni pezzo assegno un indice 
        0 Pezzo bianco
        1 Pezzo nero
        2 Dama bianca
        3 Dama nera
    */
    SDL_Texture* textures[12];
    SDL_Texture* themes[5];
    char* themesDir[] = {
        "ClassicThemes/ClassicTheme.png", "ClassicThemes/FrostTheme.png", "ClassicThemes/WoodTheme.png", "ClassicThemes/PurpleTheme.png"
    };

    themes[0] = IMG_LoadTexture(renderer, "ClassicThemes/ClassicTheme.png");
    themes[1] = IMG_LoadTexture(renderer, "ClassicThemes/FrostTheme.png");
    themes[2] = IMG_LoadTexture(renderer, "ClassicThemes/WoodTheme.png");
    themes[3] = IMG_LoadTexture(renderer, "ClassicThemes/PurpleTheme.png");


    SDL_Rect playButton  = { (WINDOW_WIDTH - BUTTON_START_WIDTH) / 2, startY, BUTTON_START_WIDTH, BUTTON_START_HEIGHT };
    SDL_Rect themeButton = { (WINDOW_WIDTH - BUTTON_START_WIDTH) / 2, startY + BUTTON_START_HEIGHT + SPACE_BETWEEN_BUTTONS, BUTTON_START_WIDTH, BUTTON_START_HEIGHT };
    SDL_Rect infoButton  = { (WINDOW_WIDTH - BUTTON_START_WIDTH) / 2, startY + 2 * (BUTTON_START_HEIGHT + SPACE_BETWEEN_BUTTONS), BUTTON_START_WIDTH, BUTTON_START_HEIGHT };
    SDL_Rect quitButton  = { (WINDOW_WIDTH - BUTTON_START_WIDTH) / 2, startY + 3 * (BUTTON_START_HEIGHT + SPACE_BETWEEN_BUTTONS), BUTTON_START_WIDTH, BUTTON_START_HEIGHT };

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            SDL_Point mousePoint;
            switch(event.type){
                case SDL_QUIT:
                    quit = true;
                break;

                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT){
                        SDL_GetMouseState(&mousePoint.x, &mousePoint.y);
                        int x = mousePoint.x*to_grid;
                        int y = mousePoint.y*to_grid;

                        //Se si sta muovendo un pezzo controllare se la mossa è valida, in caso positivo la mossa è fatta.
                        if(movingPiece % 2 == turn){
                            for(int i = 0; i < n_moves; i++){
                                if(moves[i].x == x && moves[i].y == y){
                                    board[x][y] = board[lastX][lastY];
                                    board[lastX][lastY] = -1;
                                    int eatX = eating_pieces[i].x;
                                    int eatY = eating_pieces[i].y;
                                    board[eatX][eatY] = -1;
                                    movingPiece = -1;
                                    n_moves = 0;
                                    if(y == 7 && board[x][y] % 2)
                                        board[x][y] = 3;
                                    if(y == 0 && !(board[x][y] % 2))
                                        board[x][y] = 2;
                                    // validateMove(board, &n_moves, moves,eating_pieces,x,y);
                                    toggleTurn;
                                    // if(n_moves != 0){
                                        // printf("n_moves: %d\n",n_moves);
                                        // for(int k = 0; k < n_moves; k++){
                                            // Entrambe le coordinate del pezzo sono -1 (quindi non esiste un pezzo mangiabile per quella mossa)
                                            // printf("Checking %d\n",eating_pieces[k].x + eating_pieces[k].y);
                                            // 
                                            // if((eating_pieces[k].x + eating_pieces[k].y) == -2){
                                                // bqwerty = 0;
                                            // }
                                        // }
                                    // }else{
                                        // toggleTurn;
                                    // }
                                }
                            }

                        }

                        /*Controlla se il pezzo cliccato è una pedina e se il pezzo cliccato è una pedina 
                         board[x][y]%2 == turn
                        */
                        if(playButtonFlag && (movingPiece == -1 || board[x][y]%2 == turn)){
                            validateMove(board, &n_moves, moves,eating_pieces,x,y);
                            if(n_moves > 0){
                                lastX = x;
                                lastY = y;
                                movingPiece = turn;
                            }

                        }

                        if(!playButtonFlag && !themeButtonFlag && !infoButtonFlag)
                        if (SDL_PointInRect(&mousePoint, &playButton)) {
                            playButtonFlag = true;
                        } else if (SDL_PointInRect(&mousePoint, &themeButton)) {
                            themeButtonFlag = true;
                        } else if (SDL_PointInRect(&mousePoint, &infoButton)) {
                            infoButtonFlag = true;
                        } else if (SDL_PointInRect(&mousePoint, &quitButton)) {
                            quit = true;
                        }
                    }
                    
                break;

            }
            
        }

        Uint32 endTime = SDL_GetTicks();
        Uint32 deltaTime = endTime - startTime;
        frames++;
        if (deltaTime >= 1000){
            float fps = frames / (deltaTime / 1000.0f);
            //printf("FPS: %.2f\n", fps);
            frames = 0;
            startTime = endTime;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &playButton);
        SDL_RenderFillRect(renderer, &themeButton);
        SDL_RenderFillRect(renderer, &infoButton);
        SDL_RenderFillRect(renderer, &quitButton);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderCopy(renderer, textTexturePlay, NULL, &playButton);
        SDL_RenderCopy(renderer, textTextureTheme, NULL, &themeButton);
        SDL_RenderCopy(renderer, textTextureInfo, NULL, &infoButton);
        SDL_RenderCopy(renderer, textTextureQuit, NULL, &quitButton);
        
        if (playButtonFlag){
            if(selected_theme != -1){
                textures[0]  = IMG_LoadTexture(renderer,CONCAT(texture[selected_theme],"/white.png"));
                textures[1]  = IMG_LoadTexture(renderer,CONCAT(texture[selected_theme],"/black.png"));
                textures[2]  = IMG_LoadTexture(renderer,CONCAT(texture[selected_theme],"/white_dama.png"));
                textures[3]  = IMG_LoadTexture(renderer,CONCAT(texture[selected_theme],"/black_dama.png"));
                textures[4]  = IMG_LoadTexture(renderer,CONCAT(texture[selected_theme],"/move.png"));
                white = colorBoards[selected_theme];
                black = colorBoards[selected_theme+4];
                selected_theme = -1;
                
            }
            // Disegna scacchiera
            for (int y = 0; y < BOARD_SIZE; y++){
               for (int x = 0; x < BOARD_SIZE; x++){
                    SDL_Rect rect = {x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                    if ((x + y) % 2) {
                        SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
                    } else {
                        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
                    }
                    SDL_RenderFillRect(renderer, &rect);
                    if(board[x][y] != -1 && selected_theme == -1)
                        SDL_RenderCopy(renderer, textures[board[x][y]], NULL, &rect);
               }
            }
            for(int i = 0; i<n_moves; i++){
                SDL_Point point = moves[i];
                SDL_Rect rect = {point.x * SQUARE_SIZE, point.y*SQUARE_SIZE,SQUARE_SIZE,SQUARE_SIZE};
                SDL_RenderCopy(renderer,textures[4],NULL,&rect);
            }
        } 
        else if (themeButtonFlag) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
            int xTheme = (startThemeX * 150) + IMAGE_WIDTH /2;
            int yTheme = (startThemeY * 150) + IMAGE_WIDTH /2;

            int x = (WINDOW_WIDTH - GRID_WIDTH) / 2;
            int y = (WINDOW_HEIGHT - GRID_HEIGHT) / 2;
            int x2 = (WINDOW_WIDTH + GRID_WIDTH)/2 - IMAGE_WIDTH;
            int y2 = (WINDOW_HEIGHT + GRID_HEIGHT)/2 - IMAGE_HEIGHT;

            themeRects[0] = (SDL_Rect){x, y2, IMAGE_WIDTH, IMAGE_HEIGHT};
            themeRects[1] = (SDL_Rect){x2, y, IMAGE_WIDTH, IMAGE_HEIGHT};
            themeRects[2] = (SDL_Rect){x2, y2, IMAGE_WIDTH, IMAGE_HEIGHT};
            themeRects[3] = (SDL_Rect){x, y, IMAGE_WIDTH, IMAGE_HEIGHT};

            for(int i = 0; i < 4; i++)
                SDL_RenderCopy(renderer, themes[i], NULL, &themeRects[i]);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            #ifdef DEBUG
                SDL_RenderDrawLine(renderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);
                SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT / 2);
                SDL_RenderDrawRect(renderer,  &(SDL_Rect){x, y, GRID_WIDTH, GRID_HEIGHT});
            #endif
            
        }


        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}