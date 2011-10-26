#ifndef PLATY_H_INCLUDED
#define PLATY_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

class Platy
{
    public:

        Platy(int X, int Y);
        ~Platy();
        void PlatyMov(int, SDL_Surface* area);
        int DameX(void);
        int DameY(void);
        float DameAnguloGiro(int X, int Y, int X1, int Y1);

    private:

        int x;
        int y;

};

#endif // PLATY_H_INCLUDED
