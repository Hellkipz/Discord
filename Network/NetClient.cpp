#include "NetClient.h"

extern char *getMsg(TCPsocket sock, char **buf);
extern int putMsg(TCPsocket sock, const char *buf);

NetClient g_netclient;

NetClient::NetClient()
{
    m_bServerStarted = false;
}

void NetClient::Init()
{
    IPaddress ip;
	Uint16 port = 7171;
	char *name;

	name="test";

	/* initialize SDL */
	if(SDL_Init(0)==-1)
	{
		printf("SDL_Init: %s\n",SDL_GetError());
		exit(1);
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		printf("SDLNet_Init: %s\n",SDLNet_GetError());
		SDL_Quit();
		exit(3);
	}

	set=SDLNet_AllocSocketSet(1);
	if(!set)
	{
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		exit(4); /*most of the time this is a major error, but do what you want. */
	}

	/* Resolve the argument into an IPaddress type */
	printf("Connecting to %s port %d\n","127.0.0.1",port);
	if(SDLNet_ResolveHost(&ip,"127.0.0.1",port)==-1)
	{
		printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		exit(5);
	}

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		exit(6);
	}

	if(SDLNet_TCP_AddSocket(set,sock)==-1)
	{
		printf("SDLNet_TCP_AddSocket: %s\n",SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		exit(7);
	}

	/* login with a name */
	if(!putMsg(sock,name))
	{
		SDLNet_TCP_Close(sock);
		SDLNet_Quit();
		SDL_Quit();
		exit(8);
	}

	printf("Trying to login as %s\n",name);

	m_bServerStarted = true;
}

void NetClient::NetLoop()
{
    if (m_bServerStarted == false)
        return;
    int numready;

    char *str = NULL; //todo: use string instead, also on netserver, you pass a string pointer to tcputil and it writes the output into it ;) then we clear it

    /* wait on the socket for data */

    numready=SDLNet_CheckSockets(set, 0);
    if(numready==-1)
    {
        printf("SDLNet_CheckSockets: %s\n",SDLNet_GetError());
        exit(2);
    }


    /* check to see if the server sent us data */
    if(numready && SDLNet_SocketReady(sock))
    {
        /* getMsg is in tcputil.h, it gets a string from the socket */
        /* with a bunch of error handling */
        if(!getMsg(sock,&str))
            return;

        /* post it to the screen */
        printf("%s\n",str);
    }
}
