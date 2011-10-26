#include "malos.h"

#include <cmath>
#include <cstdlib>

//Generador de numeros aleatoriois en un rango establecido
int random(int lowest_number, int highest_number)
{
    int range = highest_number - lowest_number + 1;
    int temp;
    temp = (rand() % range) + lowest_number;
    while (temp == 0);

    return temp;
}

MalosBasico::MalosBasico(int width, int height)
{
    x = random(0, width);
    y = random(0, height);
}

MalosBasico::~MalosBasico()
{

}

//Seguir al malo, se manda posicion x e y del jugador
void MalosBasico::SeguirJugador(int PlayerX, int PlayerY)
{
    float AnguloPLayer = atan2(PlayerY-y, PlayerX-x);

    float VelocidadX = cos(AnguloPLayer)*10;
    float VelocidadY = sin(AnguloPLayer)*10;

    VelocidadX=LimiteMovimiento(VelocidadX, 8, -8);
    VelocidadY=LimiteMovimiento(VelocidadY, 8, -8);

    x += VelocidadX;
    y += VelocidadY;
}

//Función que evita que se mueva demaciado deprisa para este enemigo
int MalosBasico::LimiteMovimiento(int Valor,int Max, int Min)
{
    if (Valor>Max) return Max;
    if (Valor<Min) return Min;
}

//Reajustar la psocion de los enemigos para evitar sobre encimarse todos
void MalosBasico::ReajustarPosicion(int tmpx, int tmpy, int Radio)
{
    int tmpdis=sqrt(pow(tmpx-x,2)+pow(tmpy-y,2));

    if (tmpdis<Radio)
    {
        if (tmpx>x)
            x=tmpx-Radio;
        else
            x=tmpx+Radio;

        if (tmpy>y)
            y=tmpy-Radio;
        else
            y=tmpy+Radio;
    }
}

int MalosBasico::MaloX(void)
{
    return x;
}

int MalosBasico::MaloY(void)
{
    return y;
}
