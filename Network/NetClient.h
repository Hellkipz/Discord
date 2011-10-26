#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <SDL.h>
#include <SDL_net.h>
#include <string>


#define MAXLEN (10*1024) /* 10 KB - adequate for text! */

class NetClient
{
    public:
        NetClient();
        void Init();
        void NetLoop();


    private:
        SDLNet_SocketSet set;
        TCPsocket sock;
        bool m_bServerStarted;

};

extern NetClient g_netclient;

#endif // NETCLIENT_H
