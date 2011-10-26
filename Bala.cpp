#include "Bala.h"
#include <cmath>
#include <stdio.h>

// Constructor
Bullet::Bullet(int X, int Y)
{
    x = X;
    y = Y;
    Calculo = false;
}

// Destructor
Bullet::~Bullet()
{

}


void Bullet::Movimiento(int MouseX, int MouseY, SDL_Surface* pantalla, SDL_Rect* sprite)
{
    if(!Calculo)
    {

        float vy = MouseY - (sprite->y);
        float vx = MouseX - (sprite->x);

        Dy = 20 * (vy) / sqrt((vy*vy)+(vx*vx));
        Dx = 20 * (vx) / sqrt((vy*vy)+(vx*vx));

        Calculo = true;
    }

    x += (int)Dx;
    y += (int)Dy;

}

int Bullet::BalaX(void)
{
    return x;
}

int Bullet::BalaY(void)
{
    return y;
}
