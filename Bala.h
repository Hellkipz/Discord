#ifndef BALA_H_INCLUDED
#define BALA_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

class Bullet
{
    public:
        Bullet(int X, int Y);
        ~Bullet();

        void Movimiento(int MouseX, int MouseY, SDL_Surface* pantalla, SDL_Rect* sprite);
        int BalaX(void);
        int BalaY(void);

    private:

        int x;
        int y;
        float Dx;
        float Dy;
        bool Calculo;
};

#endif // BALA_H_INCLUDED
