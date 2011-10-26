#ifndef NETSERVER_H
#define NETSERVER_H

#include <SDL.h>
#include <SDL_net.h>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>


typedef struct
{
    int posx;
    int posy;
    int rotation;
    std::string m_sUserName;

} userData;


typedef struct {

	TCPsocket m_sock;
	userData user;
} netClient;

class NetServer
{
    public:
        NetServer();
        virtual ~NetServer();
        void Init();
        void NetLoop();
        void CloseAllConnections();
        int GetClientCount();

        std::map<int,netClient> m_clients;


    private:

        IPaddress m_IP;
        Uint16 m_iPort;
        Uint16 m_iNumClients;
        bool m_bServerStarted;
        TCPsocket m_server;

        //functions
        SDLNet_SocketSet CreateSocketSet();
        void SendAllClients(const char *buf);
        bool IsUniqueUserName(std::string newname);
        int AddClient(TCPsocket sock, char *name);
        void RemoveClient(int ID);
};

extern NetServer g_server;

#endif // NETSERVER_H
