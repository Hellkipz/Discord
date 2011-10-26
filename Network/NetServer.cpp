#include "NetServer.h"

#include "tcputil.h"

NetServer g_server;

NetServer::NetServer()
{
    m_iPort = 7171;
    m_iNumClients = 0;
    m_bServerStarted = false;
}

void NetServer::Init()
{
    /* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		std::cout << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
		SDL_Quit();
		exit(2);
	}

    /* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&m_IP,NULL,m_iPort)==-1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(3);
	}

	/* perform a byte endianess correction for the next printf */
	Uint32 ipaddr=SDL_SwapBE32(m_IP.host);

	/* output the IP address nicely */
	std::cout << "Server IP Address : "<< (ipaddr>>24) << "." <<  ((ipaddr>>16)&0xff) << "." << ((ipaddr>>8)&0xff) << "." << (ipaddr&0xff) << std::endl;

	/* resolve the hostname for the IPaddress */
	const char *host=NULL;
	host = SDLNet_ResolveIP(&m_IP);
	/* print out the hostname we got */
	if(host)
		std::cout << "Hostname   : " << host << std::endl;
	else
		std::cout << "Hostname   : N/A" << std::endl;

	/* output the port number */
	std::cout << "Port       : " << m_iPort << std::endl ;

	/* open the server socket */
	m_server=SDLNet_TCP_Open(&m_IP);
	if(!m_server)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(4);
	}



	std::cout << "Server started" << std::endl;
	m_bServerStarted = true;

}


bool checkChar(char c)
{
    if (std::isalnum(c) != 0)
        return false;
    else
        return true;
}

bool validChar(char c)
{
    if (std::isprint(c) != 0)
        return false;
    else
        return true;
}

void  cleanName (std::string *s)
{
    //http://en.cppreference.com/w/cpp/string/narrow
    s->erase(std::remove_if(s->begin(), s->end(), checkChar), s->end());
}
void fixString (std::string *s)
{
    //http://en.cppreference.com/w/cpp/string/narrow
    s->erase(std::remove_if(s->begin(), s->end(), validChar), s->end());
}

/* add a client into our array of clients */
int NetServer::AddClient(TCPsocket sock, char *_name)
{

	std::string name = _name;
	cleanName(&name);

	//we check if the strlen is 0
	if(!name.length())
	{
		putMsg(sock,"Invalid Nickname...bye bye!");
		SDLNet_TCP_Close(sock);
		return(-1);
	}

	//check for unique names
	if(!IsUniqueUserName(name))
	{
		putMsg(sock,"Duplicate Nickname...bye bye!");
		SDLNet_TCP_Close(sock);
		return(-1);
	}


    userData t {0,0,0,name};
    netClient temp {sock,t};
    m_clients[m_iNumClients] = temp;
	m_iNumClients++;

	/* server side info */
	std::cout << "User Accepted --> " << name << std::endl;
    std::stringstream msg;
    msg << "Server >> User '" << name <<  "' connected";
	SendAllClients(msg.str().c_str());
	msg.clear();

    //we return the position
	return(m_iNumClients-1);
}

/* create a socket set that has the server socket and all the client sockets */
SDLNet_SocketSet NetServer::CreateSocketSet()
{
	static SDLNet_SocketSet set=NULL;

    //if the set is started we clean it
	if(set)
		SDLNet_FreeSocketSet(set);

    //we create a socket that has the client size + the server (1)
	set=SDLNet_AllocSocketSet(m_clients.size()+1);

    //if there was an error creating the set, quit
	if(!set) {
		std::cout << "SDLNet_AllocSocketSet: " << SDLNet_GetError() << std::endl;
		exit(1);
	}

    //we add the server socket at the start of the set
	SDLNet_TCP_AddSocket(set,m_server);

    //after that we add all the clients
	for( std::map<int, netClient>::iterator ii=m_clients.begin(); ii!=m_clients.end(); ++ii)
	{
        SDLNet_TCP_AddSocket(set,(*ii).second.m_sock);
	}

    //we return all the sockets
	return(set);
}

void NetServer::NetLoop()
{
    if (m_bServerStarted == false)
        return;

    int numready;
    TCPsocket sock;

    SDLNet_SocketSet set;
    set=CreateSocketSet();

    //this checks for the sockets that has activity
    numready=SDLNet_CheckSockets(set, 0);
    //there was an error when checking sockets
    if(numready==-1)
    {
        std::cout << "SDLNet_CheckSockets: " << SDLNet_GetError() << std::endl;
        exit(3);
    }
    //there are no sockets with activity so do nothing
    if(!numready)
    {
        return;
    }
    //we check if the socket that had activity is the server
    if(SDLNet_SocketReady(m_server))
    {
        //we remove the count of the sockets with activity by 1
        numready--;
        //printf("Connection...\n");
        //accept the connection
        sock=SDLNet_TCP_Accept(m_server);
        //check the connection
        if(sock)
        {
            char *name=NULL;
            if(getMsg(sock, &name))
            {
                //std::cout << "Requested name: " << name << std::endl;
                int clientID = AddClient(sock,name);
                //if(clientID != -1) //the client was added
                    //do_command("WHO",clientID); //TODO: add commands
            }
            else
            {
                SDLNet_TCP_Close(sock);
            }
        }
    }

    char *message=NULL;
    for( std::map<int, netClient>::iterator ii=m_clients.begin(); ii!=m_clients.end(); ++ii)
    {
        if(SDLNet_SocketReady((*ii).second.m_sock))
        {
            if(getMsg((*ii).second.m_sock, &message))
            {

                numready--;
                printf("<%s> %s\n",(*ii).second.user.m_sUserName.c_str(),message);
                /* interpret commands */
                if(message[0]=='/' && strlen(message)>1)
                {
                    //do_command(message+1,&m_clients[i]);//TODO: add commands
                }
                else /* it's a regular message */
                {
                    // forward message to ALL clients...
                    std::stringstream msg;
                    msg << "<" << (*ii).second.user.m_sUserName << ">" << message;
                    SendAllClients(msg.str().c_str());
                    msg.clear();
                }
                free(message);
                message=NULL;
            }
            else
            {
                RemoveClient((*ii).first); //if there was an error sending the message remove the client
                --ii;
                if (m_clients.size() == 0)
                    return;
            }
        }
    }

}

/* send a buffer to all clients */
void NetServer::SendAllClients(const char *buf)
{
    //we check if the buffer exists and if we have clients
	if(!buf || !m_clients.size())
		return;

    //we loop on all the clients
   for( std::map<int, netClient>::iterator ii=m_clients.begin(); ii!=m_clients.end(); ++ii)
   {
        if(!putMsg((*ii).second.m_sock,buf))
        {
            RemoveClient((*ii).first); //if there was an error sending the message remove the client
            --ii;
            if (m_clients.size() == 0)
                return;
        }
   }
   return;
}

void NetServer::CloseAllConnections()
{
   for( std::map<int, netClient>::iterator ii=m_clients.begin(); ii!=m_clients.end(); ++ii)
   {
        SDLNet_TCP_Close((*ii).second.m_sock);
   }
   m_clients.clear();
}

void NetServer::RemoveClient(int ID)
{
    //m_iNumClients--;
    SDLNet_TCP_Close(m_clients[ID].m_sock);
    std::stringstream msg;
    msg << "<Server>" << m_clients[ID].user.m_sUserName << " left the chat";
    std::cout << msg;

    m_clients.erase(ID);

    SendAllClients(msg.str().c_str());
    msg.clear();
}

//returns true if the username is unique
bool NetServer::IsUniqueUserName(std::string newname)
{
    //we loop on all the clients
   for( std::map<int, netClient>::iterator ii=m_clients.begin(); ii!=m_clients.end(); ++ii)
   {
        //std::cout << (*ii).first << ": " << (*ii).second.m_sUserName << std::endl;
        if((*ii).second.user.m_sUserName == newname)
            return false;
   }
   return true;
}

int NetServer::GetClientCount()
{
    return m_clients.size();
}

NetServer::~NetServer()
{
    //dtor
}
