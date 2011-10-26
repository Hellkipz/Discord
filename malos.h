#ifndef malos_h
#define malos_h

#include <ctime>

class MalosBasico
{
    public:
        MalosBasico(int width, int height);
        ~MalosBasico();
        void SeguirJugador(int PlayerX, int PlayerY);
        int LimiteMovimiento(int Valor,int Max,int Min);
        void ReajustarPosicion(int tmpx, int tmpy, int Radio);
        int MaloX (void);
        int MaloY (void);

    private:
        int x;
        int y;
};

#endif
