#include "Platy.h"
#include "malos.h"
#include "Bala.h"
#include "Network/NetClient.h"
#include "Network/NetServer.h"
#include <time.h>
#include <cmath>
#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/sge.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#include <sge.h>
#include <SDL_ttf.h>
#endif

MalosBasico* Enemigo[30] = {NULL};
Bullet* B[30] = {NULL};
bool gamestarted = false;

void SDL_SurfaceInfo(char * name, SDL_Surface *thing)
{
    printf("Surface %s: w:%d h:%d bpp:%d\n", name, thing->w, thing->h, thing->format->BitsPerPixel);
}

int main ( int argc, char** argv )
{
    // Inicializa el video en SDL
    if ( SDL_Init( SDL_INIT_VIDEO| SDL_INIT_TIMER ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // Inicializa el TTF
    TTF_Init();

    // Limpia datos al Salir
    atexit(TTF_Quit);
    atexit(SDL_Quit);

    // Comentario de Ventana
    SDL_WM_SetCaption("Discord", NULL);

    // Crea la ventana
    SDL_Surface* screen = SDL_SetVideoMode(1024, 768, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF/*|SDL_FULLSCREEN*/);
    if ( !screen )
    {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 1;
    }

    // Semilla del Random
    srand(time(0));

    // Carga el sprite de Platy, de los enemigos y de las balas
    SDL_Surface* Bmp = IMG_Load("PlatyRight.png");
    SDL_Surface* BmpDst = IMG_Load("PlatyRight.png");
    SDL_Surface* Malo = IMG_Load("Hexapuss.png");
    SDL_Surface* Bala = IMG_Load("Bala.png");

    // Crea el area de juego
    SDL_Surface* PlayArea = IMG_Load("Background.png");

    // Crea el rectangulo de lo que se vera en la pantalla
    SDL_Rect ViewSize;
    ViewSize.x = 0;
    ViewSize.y = 0;
    ViewSize.w = screen->w;
    ViewSize.h = screen->h;

    // Coordenadas de Platy en la pantalla
    SDL_Rect ScreenCoords;

    // Coordenadas para colocar el sprite de Platy y Enemigos
    SDL_Rect SpriteCoords;
    SDL_Rect SpriteCoordsM;
    SDL_Rect SpriteBalas;

    // Crea al jugador y lo ponemos en el centro del Area de juego
    Platy Jugador1(PlayArea->w/2, PlayArea->h/2);

    // Crea lo necesario para la pantalla de Game Over
    SDL_Rect TextRect;
    TTF_Font* Fuente = TTF_OpenFont("18holes.ttf", 24);
    SDL_Color BgColor, FgColor;
    const char GO[10] = "Game Over";

    BgColor.r = 0;
    BgColor.g = 0;
    BgColor.b = 0;

    FgColor.r = 255;
    FgColor.g = 0;
    FgColor.b = 0;

    SDL_Surface* Texto = TTF_RenderText_Shaded(Fuente, GO, FgColor, BgColor);

    TextRect.x = screen->w - Texto->w /2;
    TextRect.y = screen->h - Texto->h /2;

    // Crea el manejador de eventos y los manejadores de teclas
    SDL_Event event;
    SDL_Event Salida;
    Uint8* Keys = NULL;
    bool Boton1 = false;
    SDL_Rect MousePos;
    float Bl = 0.0f, Bu = 0.0f, Br = 0.0f, Bd = 0.0f;
    float Tiempo = 0.0f, FireRate = 0.0f;
    int CantidadEnemigos = 0;
    int CantidadBalas = 0;
    int Aux = 0;
    int Vidas = 3;

    /// CICLO DEL PROGRAMA
    bool done = false;
    while (!done)
    {
        // Ciclo de proceso de mensajes
        while (SDL_PollEvent(&event))
        {
            // Checa si hay mensajes
            switch (event.type)
            {
                // Salir si la ventana es cerrada
            case SDL_QUIT:
                done = true;
                break;

                // Checa si alguna tecla fue presionada
            case SDL_KEYDOWN:
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            done = true;
                            break;
                        case SDLK_n:
                            if (!gamestarted)
                            {
                                gamestarted = true;
                                g_server.Init();
                            }
                            break;
                        case SDLK_m:
                            if (!gamestarted)
                            {
                                gamestarted = true;
                                g_netclient.Init();
                            }
                            break;
                    }
                    break;
                }
            case SDL_MOUSEMOTION:
                MousePos.x = event.motion.x;
                MousePos.y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                Boton1 = SDL_BUTTON(1);
                MousePos.x = event.button.x;
                MousePos.y = event.button.y;
                break;
            case SDL_MOUSEBUTTONUP:
                Boton1 = false;
                break;
            } // Termina switch
        } // Finaliza ciclo de proceso de mensajes

        /// EMPIEZA CICLO DE JUEGO

        Keys = SDL_GetKeyState(NULL);

        //no entramos al ciclo del juego
        if (gamestarted == false)
            continue;


        //Crea un enemigo cada 4 segundos (aprox)
        if (Tiempo > 2.0f * CantidadEnemigos && CantidadEnemigos <= 29)
        {
            Aux = 0;
            while (Enemigo[Aux] != NULL)
            {
                Aux ++;
            }
            Enemigo[Aux] = new MalosBasico(PlayArea->w, PlayArea->h);
            CantidadEnemigos ++;
        }

        // Disparos
        FireRate += 0.02;
        if(Boton1 && CantidadBalas <= 29 && FireRate >= 0.1f)
        {
            Aux = 0;
            while (B[Aux] != NULL)
            {
                Aux ++;
            }
            B[Aux] = new Bullet(Jugador1.DameX(), Jugador1.DameY());
            CantidadBalas ++;
            FireRate = 0.0f;
        }

        //Gira a Platy segun a donde se este apuntando
        BmpDst = sge_transform_surface(Bmp, 16,  Jugador1.DameAnguloGiro(MousePos.x, MousePos.y, ScreenCoords.x, ScreenCoords.y) * -180/PI, 0.7f, 0.7f, SGE_TSAFE);

        // Checa las teclas de dirección y mueve al personaje en dicha direccion
        if (Keys[SDLK_UP] || Keys[SDLK_w])
        {
            Jugador1.PlatyMov(0, PlayArea);
        }

        if (Keys[SDLK_RIGHT] || Keys[SDLK_d])
        {
            Jugador1.PlatyMov(1, PlayArea);
        }

        if (Keys[SDLK_DOWN] || Keys[SDLK_s])
        {
            Jugador1.PlatyMov(2, PlayArea);
        }

        if (Keys[SDLK_LEFT]|| Keys[SDLK_a])
        {
            Jugador1.PlatyMov(3, PlayArea);
        }

        // Obtiene las nuevas coordenadas de Platy y previene que este salga del area de juego
        ScreenCoords.x = (Jugador1.DameX() * screen->w / PlayArea->w);
        ScreenCoords.y = (Jugador1.DameY() * screen->h / PlayArea->h);

        Bl = (screen->w * screen->w) / (2*PlayArea->w);
        Bu = (screen->h * screen->h) / (2*PlayArea->h);

        Br = screen->w - ((screen->w * screen->w) / (2*PlayArea->w));
        Bd = screen->h - ((screen->h * screen->h) / (2*PlayArea->h));

        if (ScreenCoords.x > Bl && ScreenCoords.x < Br)
        {
            ScreenCoords.x = screen->w /2;
        }

        else
        {
            if (ScreenCoords.x >= Br)
                ScreenCoords.x = screen->w/2 + ((PlayArea->w/screen->w) * (ScreenCoords.x-Br));

            if (ScreenCoords.x <= Bl)
                ScreenCoords.x = screen->w/2 - ((PlayArea->w/screen->w) * (Bl - ScreenCoords.x));
        }

        if (ScreenCoords.y > Bu && ScreenCoords.y < Bd)
        {
            ScreenCoords.y = screen->h /2;
        }

        else
        {
            if (ScreenCoords.y >= Bd)
                ScreenCoords.y = screen->h/2 + (((float)PlayArea->h/(float)screen->h) * (ScreenCoords.y-Bd));

            if (ScreenCoords.y <= Bu)
                ScreenCoords.y = screen->h/2 - (((float)PlayArea->h/(float)screen->h) * (Bu - ScreenCoords.y));
        }

        // Obtiene las coordenadas donde se dibujara el sprit de Platy
        SpriteCoords.x = ScreenCoords.x - BmpDst->w /2;
        SpriteCoords.y = ScreenCoords.y - BmpDst->h /2;

        // Obtiene la nueva vista para mantener siempre centrado a Platy y previene que la vista salga del area de juego
        ViewSize.x = Jugador1.DameX() - screen->w/2;
        ViewSize.y = Jugador1.DameY() - screen->h/2;

        //printf("ViewSize.x: %i,\tViewSize.y: %i\n", ViewSize.x, ViewSize.y);

        if (ViewSize.x <= 0)
            ViewSize.x = 0;
        if (ViewSize.y <= 0)
            ViewSize.y = 0;
        if (ViewSize.x >= PlayArea->w - screen->w)
            ViewSize.x = PlayArea->w - screen->w;
        if (ViewSize.y >= PlayArea->h - screen->h)
            ViewSize.y = PlayArea->h - screen->h;

        // El enemigo sigue al jugador
        for (int i = 0; i < CantidadEnemigos; i++)
        {
            if (Enemigo[i] != NULL)
            {
                Enemigo[i]->SeguirJugador(Jugador1.DameX(), Jugador1.DameY());
                for (int j = 0; j < CantidadEnemigos; j++)
                {
                    if (j != i && Enemigo[i] != NULL && Enemigo[j] != NULL)
                        Enemigo[i]->ReajustarPosicion(Enemigo[j]->MaloX(), Enemigo[j]->MaloY(), 50);
                }
            }
        }

        // Movimiento de las balas
        for (int i = 0; i < CantidadBalas; i++)
        {
            if(B[i] != NULL)
            {
                B[i]->Movimiento(MousePos.x, MousePos.y, screen, &ScreenCoords);
            }
        }

        // Detecta colisiones de balas con el enemigo
        for (int i = 0; i < CantidadEnemigos; i++)
        {
            for (int j = 0; j < CantidadBalas; j++)
            {
                if(Enemigo[i] != NULL && B[j] != NULL)
                {
                    Aux = abs(sqrt(pow(Enemigo[i]->MaloX()-B[j]->BalaX(),2) + pow(Enemigo[i]->MaloY()-B[j]->BalaY(),2)));
                    //printf ("Distancia: %i\n", Aux);
                    if (Aux < 25 && Enemigo[i] != NULL && B[j] != NULL)
                    {
                        //free(Enemigo[i]);
                        delete Enemigo[i];
                        //free (B[j]);
                        delete B[j];
                        Enemigo[i] = NULL;
                        B[j] = NULL;
                        CantidadBalas --;
                        CantidadEnemigos --;
                    }
                }
            }
        }

        // Detecta colisiones de los enemigos con Platy y finaliza el juego
        for (int i = 0; i < CantidadEnemigos; i++)
        {
            if (Enemigo[i] != NULL)
            {
                Aux = abs(sqrt(pow(Enemigo[i]->MaloX()-Jugador1.DameX(),2) + pow(Enemigo[i]->MaloY()-Jugador1.DameY(),2)));
                if (Aux < 25 && Enemigo[i] != NULL)
                {
                    Vidas --;
                    for (int j = 0; j < CantidadEnemigos; j++)
                    {
                            delete Enemigo[j];
                            Enemigo[j] = NULL;
                    }
                    CantidadEnemigos = 0;
                }
            }
        }

        if (Vidas  <= 0)
        {
            //TODO: implementar game over
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_BlitSurface(Texto, 0, screen, &TextRect);
        }

        else
        {
        // Limpia pantalla y el area de juego
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 255, 255));
        //SDL_BlitSurface(PlayAreaClear, 0, PlayArea, 0);

        // Dibuja la respectiva parte de area de juego en la pantalla
        SDL_BlitSurface(PlayArea, &ViewSize, screen, 0);

        // Dibuja a los Enemigos en la pantalla, solo si estan en una posicion visible
        for (int i = 0; i < CantidadEnemigos; i++)
        {
            if (Enemigo[i] != NULL)
            {
                if ((Enemigo[i]->MaloX() > ViewSize.x -20 && Enemigo[i]->MaloX() < ViewSize.x + ViewSize.w + 20) && (Enemigo[i]->MaloY() > ViewSize.y -20 && Enemigo[i]->MaloY() < ViewSize.y + ViewSize.h + 20))
                {
                    SpriteCoordsM.x = Enemigo[i]->MaloX() - ViewSize.x - Malo->w/2;
                    SpriteCoordsM.y = Enemigo[i]->MaloY() - ViewSize.y - Malo->h/2;
                    SDL_BlitSurface(Malo, 0, screen, &SpriteCoordsM);
                }
            }
        }

        // Dibuja las balas en la pantalla, solo si estan en una posición visible. Las elimina si salen de la apantalla.
        for (int i = 0; i < CantidadBalas; i++)
        {
            if (B[i] != NULL)
            {
                if ((B[i]->BalaX() > ViewSize.x -20 && B[i]->BalaX() < ViewSize.x + ViewSize.w + 20) && (B[i]->BalaY() > ViewSize.y -20 && B[i]->BalaY() < ViewSize.y + ViewSize.h + 20))
                {
                    SpriteBalas.x = B[i]->BalaX() - ViewSize.x - Bala->w/2;
                    SpriteBalas.y = B[i]->BalaY() - ViewSize.y - Bala->h/2;
                    SDL_BlitSurface(Bala, 0, screen, &SpriteBalas);
                }
                else
                {
                    delete B[i];
                    B[i] = NULL;
                    CantidadBalas --;
                }
            }
        }

        // Dibuja a Platy en el area de juego
        SDL_BlitSurface(BmpDst, 0, screen, &SpriteCoords);
        }

        /// TERMINA CICLO DEL JUEGO

        // Finalmente muestra la pantalla
        SDL_Flip(screen);
        SDL_Delay(20);
        Tiempo += 0.02f;

    } /// TERMINA CICLO DEL PROGRAMA

    // Libera bitmaps y fuentes creados
    SDL_FreeSurface(screen);
    SDL_FreeSurface(Bmp);
    SDL_FreeSurface(PlayArea);
    SDL_FreeSurface(BmpDst);
    SDL_FreeSurface(Malo);
    SDL_FreeSurface(Bala);
    SDL_FreeSurface(Texto);
    TTF_CloseFont(Fuente);

    // Todo Bien ;)
    printf("Exited cleanly\n");
    return 0;
}
