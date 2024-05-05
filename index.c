#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <string.h>

#define SPOSTA_Y(a, b) a.x, a.y + b //Sposta SDL_Point in verticale secondo b
#define COORD(a) a.x, a.y //Semplifica il codice e sostituisce le coordinate usando un SDL_Point

#define toggleTurn turn = !turn
#define resetGame turn = 0; for(int y = 0; y<BOARD_SIZE; y++){\
        for(int x = 0; x<BOARD_SIZE; x++){\
            board[x][y] = -1;\
            int even = (x + y) % 2; \
            if( even && y < BOARD_SIZE/2 - 1)\
                board[x][y] = 1;\
            if( even && y > BOARD_SIZE/2)\
                board[x][y] = 0;\
        }\
    }\
    ate = 0; n_moves = 0;\
    game_state = 0;

#define BOARD_SIZE 8
#define to_grid BOARD_SIZE / WINDOW_WIDTH
#define SQUARE_SIZE WINDOW_WIDTH / BOARD_SIZE
#define IMAGE_WIDTH 150
#define IMAGE_HEIGHT 150
#define GRID_WIDTH 500
#define GRID_HEIGHT 400

/*
Controlla chi deve fare una mossa
    0 Bianco
    1 Nero
*/

int main(int argc, char* argv[]) {
    const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
    const int BUTTON_WIDTH = 150, BUTTON_HEIGHT = 80, SPACE_BETWEEN_BUTTONS = 20;
    
    //Variabili per i temi
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

    
    int selected_theme = 2;
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
    int n_moves = 0;                    //Numero di mosse trovate per uno specifico pezzo
    SDL_Point moves[8];                 //Posizione delle mosse trovate per uno specifico pezzo
    SDL_Point eating_pieces[8];         //Posizione delle pedine mangiate in caso di mossa
    int ate = 0;                        //Controlla se nell'ultima mossa è stato mangiato un pezzo 

    bool turn = 0;
    /*
    Represents game state 
    0 playing
    1 white won
    2 black won
    */
    int game_state = 0;
    int movingPiece = -1;
    int lastX = -1, lastY = -1;

    // Crea la damiera
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

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Dama", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event event;
    bool playButtonFlag = false;
    bool themeButtonFlag = false;
    TTF_Init();

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 1024);
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Play", textColor);
    SDL_Surface* themeSurface = TTF_RenderText_Solid(font, "Theme", textColor);
    SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Quit", textColor);

    SDL_Texture* textTexturePlay = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Texture* textTextureTheme = SDL_CreateTextureFromSurface(renderer, themeSurface);
    SDL_Texture* textTextureQuit = SDL_CreateTextureFromSurface(renderer, quitSurface);

    SDL_Texture* end_game_texture = NULL;

    /*A ogni pezzo assegno un indice 
        0 Pezzo bianco
        1 Pezzo nero
        2 Dama bianca
        3 Dama nera
    */
    SDL_Texture* textures[4];
    SDL_Texture* themes[4];

    themes[0] = IMG_LoadTexture(renderer, "Previews/GreenTheme.png");
    themes[1] = IMG_LoadTexture(renderer, "Previews/FrostTheme.png");
    themes[2] = IMG_LoadTexture(renderer, "Previews/ClassicTheme.png");
    themes[3] = IMG_LoadTexture(renderer, "Previews/PurpleTheme.png");


    SDL_Point centro = {(WINDOW_WIDTH - BUTTON_WIDTH) /2 , (WINDOW_HEIGHT - BUTTON_HEIGHT)/2 };

    SDL_Rect playButton  = { SPOSTA_Y(centro, - BUTTON_HEIGHT - SPACE_BETWEEN_BUTTONS),  BUTTON_WIDTH, BUTTON_HEIGHT };
    SDL_Rect themeButton = { COORD(centro),  BUTTON_WIDTH, BUTTON_HEIGHT };
    SDL_Rect quitButton  = { SPOSTA_Y(centro, BUTTON_HEIGHT + SPACE_BETWEEN_BUTTONS),  BUTTON_WIDTH, BUTTON_HEIGHT };

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

                        if(themeButtonFlag){
                            for(int i = 0; i<4; i++){
                                if(SDL_PointInRect(&mousePoint,&themeRects[i])){
                                    selected_theme = i;
                                    themeButtonFlag = 0;
                                    playButtonFlag = 1;
                                    break;
                                }
                            }
                        }

                        if(game_state != 0){
                            resetGame;
                        }
                        //Se si sta muovendo un pezzo controllare se la mossa è valida, in caso positivo la mossa è fatta.
                        if(movingPiece % 2 == turn){
                            ate = 0;
                            for(int i = 0; i < n_moves; i++){
                                if(moves[i].x == x && moves[i].y == y){
                                    
                                    board[x][y] = board[lastX][lastY];
                                    board[lastX][lastY] = -1;
                                    int eatX = eating_pieces[i].x;
                                    int eatY = eating_pieces[i].y;
                                    if(eatX + eatY != -2){
                                        board[eatX][eatY] = -1;
                                        ate = 1;
                                    }
                                    movingPiece = -1;
                                    n_moves = 0;
                                    if(y == 7 && board[x][y] % 2)
                                        board[x][y] = 3;
                                    if(y == 0 && !(board[x][y] % 2))
                                        board[x][y] = 2;
                                    if(ate){
                                        n_moves = 0;
                                        
                                        int piece = board[x][y];
                                        if(turn == piece % 2) {
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
                                                if (piece <= 1 && board[moveX][moveY] == enemy) {
                                                    moveX = x + 2 * right;
                                                    moveY = y + 2 * up;
                                                    if (board[moveX][moveY] == -1) {
                                                        moves[n] = (SDL_Point) { moveX,moveY };
                                                        eating_pieces[n] = (SDL_Point) { x + right,y + up };
                                                        n++;
                                                    }
                                                }

                                                moveX = x - right;
                                                moveY = y + up;
                                                if (piece <= 1 && board[moveX][moveY] == enemy) {
                                                    moveX = x - 2 * right;
                                                    moveY = y + 2 * up;
                                                    if (board[moveX][moveY] == -1) {
                                                        moves[n] = (SDL_Point) { moveX,moveY };
                                                        eating_pieces[n] = (SDL_Point) { x - right,y + up };
                                                        n++;
                                                    }
                                                }

                                                //Se il pezzo è una dama
                                                if(piece > 1){
                                                    moveX = x + right;
                                                    moveY = y - up;
                                                    if (board[moveX][moveY] % 2 == enemy ) {
                                                        moveX = x + 2 * right;
                                                        moveY = y - 2 * up;
                                                        if (board[moveX][moveY] == -1) {
                                                            moves[n] = (SDL_Point) { moveX,moveY };
                                                            eating_pieces[n] = (SDL_Point) { x + right,y - up };
                                                            n++;
                                                        }
                                                    }

                                                    moveX = x - right;
                                                    moveY = y - up;
                                                    if (board[moveX][moveY] % 2 == enemy) {
                                                        moveX = x - 2 * right;
                                                        moveY = y - 2 * up;
                                                        if (board[moveX][moveY] == -1) {
                                                            moves[n] = (SDL_Point) { moveX,moveY };
                                                            eating_pieces[n] = (SDL_Point) { x - right,y - up };
                                                            n++;
                                                        }
                                                    }

                                                    moveX = x + right;
                                                    moveY = y + up;
                                                    if (board[moveX][moveY]%2 == enemy) {
                                                        moveX = x + 2 * right;
                                                        moveY = y + 2 * up;
                                                        if (board[moveX][moveY] == -1) {
                                                            moves[n] = (SDL_Point) { moveX,moveY };
                                                            eating_pieces[n] = (SDL_Point) { x + right,y + up };
                                                            n++;
                                                        }
                                                    }

                                                    moveX = x - right;
                                                    moveY = y + up;
                                                    if (board[moveX][moveY]%2 == enemy) {
                                                        moveX = x - 2 * right;
                                                        moveY = y + 2 * up;
                                                        if (board[moveX][moveY] == -1) {
                                                            moves[n] = (SDL_Point) { moveX,moveY };
                                                            eating_pieces[n] = (SDL_Point) { x - right,y + up };
                                                            n++;
                                                        }
                                                    }
                                                }
                                                n_moves = n;
                                            }
                                        }

                                    }
                                    if(n_moves == 0){
                                        toggleTurn;
                                        ate = 0;
                                    }
                                }
                            }

                        }

                        /*Controlla se il pezzo cliccato è una pedina e se il pezzo cliccato è una pedina con mosse disponibili
                         board[x][y]%2 == turn
                        */
                        if(playButtonFlag && (movingPiece == -1 || board[x][y]%2 == turn)){
                            // Piece e' la pedina
                            int piece = board[x][y];
                            //verifichiamo se il turno e' (bianco / nero) e il pezzo e' (bianco / nero) e che l'ultima mossa che ha fatto non ha mangiato nessuno pezzo
                            if(turn == piece % 2 && ate == 0) {
                                // la variabile enemy intende dire che il pezzo opposto, nel senso che se e' il turno del bianco allora il nero e' il suo enemy e viceversa
                                int enemy = (piece + 1) % 2;
                                int n = 0;
                                // Se il pezzo è bianco inverti le direzioni 'su' e 'destra'
                                int up = -1;
                                int right = -1;
                                //verifichiamo se non c'e' nessun pezzo al movimento
                                if (piece != -1) {
                                    int moveX, moveY;
                                    //Se il pezzo e' bianco
                                    if (piece % 2) {
                                        up = 1;
                                        right = 1;
                                    }
                                    //Impostiamo la variabile moveX e moveY in alto a destra
                                    moveX = x + right;
                                    moveY = y + up;
                                    //Se in alto a destra non c'e' nessun pezzo
                                    if (board[moveX][moveY] == -1) {
                                        //aggiunge in alto a destra una mossa disponibile
                                        moves[n] = (SDL_Point) { moveX,moveY };
                                        //aggiunge all' array dei pezzi mangiabili il niente ovvero un pezzo vuoto
                                        eating_pieces[n] = (SDL_Point) { -1,-1 };
                                        n++;
                                    }
                                    //Se il pezzo e' una pedina e se la pedina in alto a destra e' un nemico controlla ancora in alto a destra del nemico se c'e' una casella libera
                                    if (piece <= 1 && board[moveX][moveY] == enemy) {
                                        moveX = x + 2 * right;
                                        moveY = y + 2 * up;
                                        if (board[moveX][moveY] == -1) {
                                            moves[n] = (SDL_Point) { moveX,moveY };
                                            //aggiunge all' array dei pezzi mangiabili la pedina che si trova in alto a destra
                                            eating_pieces[n] = (SDL_Point) { x + right,y + up };
                                            n++;
                                        }
                                    }
                                    //Impostiamo la variabile moveX e moveY in alto a sinistra
                                    moveX = x - right;
                                    moveY = y + up;
                                    //Se in alto a sinistra non c'e' nessun pezzo
                                    if (board[moveX][moveY] == -1) {
                                        //aggiunge in alto a sinistra una mossa disponibile
                                        moves[n] = (SDL_Point) { moveX,moveY };
                                        //aggiunge all' array dei pezzi mangiabili il niente ovvero un pezzo vuoto
                                        eating_pieces[n] = (SDL_Point) { -1,-1 };
                                        n++;
                                    }
                                    //Se il pezzo e' una pedina e se la pedina in alto a sinistra e' un nemico controlla ancora in alto a destra del nemico se c'e' una casella libera
                                    if (piece <= 1 && board[moveX][moveY] == enemy) {
                                        moveX = x - 2 * right;
                                        moveY = y + 2 * up;
                                        if (board[moveX][moveY] == -1) {
                                            moves[n] = (SDL_Point) { moveX,moveY };
                                            //aggiunge all' array dei pezzi mangiabili la pedina che si trova in alto a sinistra
                                            eating_pieces[n] = (SDL_Point) { x - right,y + up };
                                            n++;
                                        }
                                    }

                                    //Se il pezzo è una dama
                                    if(piece > 1){
                                        //mosse disponibili in basso a destra
                                        moveX = x + right;
                                        moveY = y - up;
                                        //Se c'e' una casella libera in basso a destra 
                                        if (board[moveX][moveY] == -1) {
                                            //Aggiunge che c'e' una mossa disponibile in basso a destra
                                            moves[n] = (SDL_Point) { moveX,moveY };
                                            //aggiunge all' array dei pezzi mangiabili il niente ovvero un pezzo vuoto
                                            eating_pieces[n] = (SDL_Point) { -1,-1 };
                                            n++;
                                        }
                                        //Se nella casella in basso a destra c'e' un nemico allora verifichiamo ancora dopo se una casella e' libera 
                                        if (board[moveX][moveY] % 2 == enemy ) {
                                            moveX = x + 2 * right;
                                            moveY = y - 2 * up;
                                            if (board[moveX][moveY] == -1) {
                                                //aggiungiamo alle mosse disponibili quella in basso a destra
                                                moves[n] = (SDL_Point) { moveX,moveY };
                                                //aggiunge all' array dei pezzi mangiabili la pedina che si trova in basso a destra
                                                eating_pieces[n] = (SDL_Point) { x + right,y - up };
                                                n++;
                                            }
                                        }

                                        moveX = x - right;
                                        moveY = y - up;
                                        //verifichiamo se in basso a sinistra c'e' una casella vuota
                                        if (board[moveX][moveY] == -1) {
                                            //aggiungiamo alle mosse disponibili quella in basso a sinistra
                                            moves[n] = (SDL_Point) { moveX,moveY };
                                            //aggiunge all' array dei pezzi mangiabili il niente ovvero un pezzo vuoto
                                            eating_pieces[n] = (SDL_Point) { -1,-1 };
                                            n++;
                                        }
                                        //Se nella casella in basso a sinistra c'e' un nemico allora verifichiamo ancora dopo se una casella e' libera 
                                        if (board[moveX][moveY] % 2 == enemy) {
                                            moveX = x - 2 * right;
                                            moveY = y - 2 * up;
                                            if (board[moveX][moveY] == -1) {
                                                //aggiungiamo alle mosse disponibili quella in basso a sinistra
                                                moves[n] = (SDL_Point) { moveX,moveY };
                                                //aggiunge all' array dei pezzi mangiabili la pedina che si trova in basso a sinistra
                                                eating_pieces[n] = (SDL_Point) { x - right,y - up };
                                                n++;
                                            }
                                        }

                                        moveX = x + right;
                                        moveY = y + up;
                                        //Se nella casella in alto a destra c' e' un nemico allora verifichiamo ancora dopo se una casella e' libera
                                        if (board[moveX][moveY]%2 == enemy) {
                                            moveX = x + 2 * right;
                                            moveY = y + 2 * up;
                                            if (board[moveX][moveY] == -1) {
                                                //aggiungiamo alle mosse disponibili quella in alto a destra
                                                moves[n] = (SDL_Point) { moveX,moveY };
                                                //aggiunge all'array dei pezzi mangiabili la pedina che si trova in alto a destra
                                                eating_pieces[n] = (SDL_Point) { x + right,y + up };
                                                n++;
                                            }
                                        }

                                        moveX = x - right;
                                        moveY = y + up;
                                        //Se nella casella in alto a sinistra c' e' un nemico allora verifichiamo ancora dopo se una casella e' libera
                                        if (board[moveX][moveY]%2 == enemy) {
                                            moveX = x - 2 * right;
                                            moveY = y + 2 * up;
                                            if (board[moveX][moveY] == -1) {
                                                //aggiungiamo alle mosse disponibili quella in alto a sinistra
                                                moves[n] = (SDL_Point) { moveX,moveY };
                                                //aggiunge all'array dei pezzi mangiabili la pedina che si trova in alto a sinistra
                                                eating_pieces[n] = (SDL_Point) { x - right,y + up };
                                                n++;
                                            }
                                        }
                                    }
                                    n_moves = n;
                                }
                            }
                            
                            
                            //Se ci sono piu' di zero movimenti allora movingPiece sara' uguale al turno
                            if(n_moves > 0){
                                lastX = x;
                                lastY = y;
                                movingPiece = turn;
                            }

                        }

                        if(!playButtonFlag && !themeButtonFlag)
                        if (SDL_PointInRect(&mousePoint, &playButton)) {
                            playButtonFlag = true;
                        } else if (SDL_PointInRect(&mousePoint, &themeButton)) {
                            themeButtonFlag = true;
                        } else if (SDL_PointInRect(&mousePoint, &quitButton)) {
                            quit = true;
                        }
                    }
                    
                break;

            }
            
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &playButton);
        SDL_RenderFillRect(renderer, &themeButton);
        SDL_RenderFillRect(renderer, &quitButton);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderCopy(renderer, textTexturePlay, NULL, &playButton);
        SDL_RenderCopy(renderer, textTextureTheme, NULL, &themeButton);
        SDL_RenderCopy(renderer, textTextureQuit, NULL, &quitButton);
        
        if (playButtonFlag){
            if(selected_theme != -1){
                textures[0]  = IMG_LoadTexture(renderer,"Neo/white.png");
                textures[1]  = IMG_LoadTexture(renderer,"Neo/black.png");
                textures[2]  = IMG_LoadTexture(renderer,"Neo/white_dama.png");
                textures[3]  = IMG_LoadTexture(renderer,"Neo/black_dama.png");
                textures[4]  = IMG_LoadTexture(renderer,"Neo/move.png");
                white = colorBoards[selected_theme];
                black = colorBoards[selected_theme+4];
                selected_theme = -1;
                
            }
            bool black_missing = 1, white_missing = 1; 
            // Disegna damiera
            for (int y = 0; y < BOARD_SIZE; y++){
               for (int x = 0; x < BOARD_SIZE; x++){
                    SDL_Rect rect = {x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                    if ((x + y) % 2) {
                        SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
                    } else {
                        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
                    }
                    SDL_RenderFillRect(renderer, &rect);
                    if(board[x][y] != -1 && selected_theme == -1){
                        SDL_RenderCopy(renderer, textures[board[x][y]], NULL, &rect);
                        if(board[x][y] % 2 == 0){
                            black_missing = 0;
                        }
                        if(board[x][y] % 2 == 1){
                            white_missing = 0;
                        }
                    }
               }
               
            }

            if(white_missing){
                game_state = 1;
            }

            if(black_missing){
                game_state = 2;
            }

            //Disegna le mosse
            for(int i = 0; i<n_moves; i++){
                SDL_Point point = moves[i];
                SDL_Rect rect = {point.x * SQUARE_SIZE, point.y*SQUARE_SIZE,SQUARE_SIZE,SQUARE_SIZE};
                SDL_RenderCopy(renderer,textures[4],NULL,&rect);
            }

            //Disegna scritta di fine 
            if(game_state != 0){
                char* testo;
                if(game_state == 1){
                    testo = "Il bianco ha vinto";
                }else{
                    testo = "Il nero ha vinto";
                }

                SDL_Surface* text_surface; 
                if(end_game_texture == NULL){
                    text_surface =  TTF_RenderText_Solid(font,testo,(SDL_Color){255,0,0,255});
                    end_game_texture =  SDL_CreateTextureFromSurface(renderer, text_surface);
                }
                SDL_Rect testo_rect = text_surface->clip_rect;
                testo_rect.h *= ((float)(WINDOW_WIDTH) / testo_rect.w);
                testo_rect.w = WINDOW_WIDTH;
                testo_rect.y = (WINDOW_HEIGHT - testo_rect.h) / 2;

                SDL_RenderCopy(renderer,end_game_texture,NULL,&testo_rect);
            }
        } 
        else if (themeButtonFlag) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

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
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}