#include "Platy.h"
#include <cmath>

//Constructor
Platy::Platy(int X, int Y)
{
    x = X;
    y = Y;
}

//Destructor
Platy::~Platy()
{

}

//Mueve a Platy segun la direccion recibida
void Platy::PlatyMov(int dir, SDL_Surface* area)
{
    switch (dir)
    {
        case 0: //Arriba
            if (y > 0)
            {
                y -= 10;
                //printf("x: %d\ty: %d\n", x, y);
            }
            break;
        case 1: //Derecha
            if (x <= area->w)
            {
                x += 10;
                //printf("x: %d\ty: %d\n", x, y);
            }
            break;
        case 2: //Abajo
            if (y <= area->h)
            {
                y += 10;
                //printf("x: %d\ty: %d\n", x, y);
            }
            break;
        case 3: //Izquierda
            if (x > 0)
            {
                x -= 10;
                //printf("x: %d\ty: %d\n", x, y);
            }
            break;
    }
}

//Regresa la coordenada X Platy
int Platy::DameX(void)
{
    return x;
}

//Regresa la coordenada Y de platy
int Platy::DameY(void)
{
    return y;
}

//Regresa el angulo de giro del sprite de Platy
float Platy::DameAnguloGiro(int X, int Y, int X1, int Y1)
{
    float Dy = 0.0f, Dx = 0.0f, A = 0.0f, xTemp = 0.0f, yTemp = 0.0f;

    Dy = (float)Y - (float)Y1;
    Dx = (float)X - (float)X1;

    Dy = Dy * -1.0f;

    A = atan(Dy/Dx);

    if (Dy == 0 && Dx == 0)
        A = 0.0f;

    if (Dx < 0)
        A = A + 135 * -1.0f;

    return A;
}
