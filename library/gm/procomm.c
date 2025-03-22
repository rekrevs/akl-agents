/* #include <stdlib.h> */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>

#if sun || ultrix || hpux
# define FORK vfork
#else
# define FORK fork
#endif

#define OOPS(msg) {perror(msg);*pidp = -1;*sockp = -1;return;}
#define OOPS_EXIT(msg) {perror(msg);_exit(errno);}
#define TMPDIR "/tmp/"
#if DBG
#define DBGPRINT0(F) {char buf[256]; sprintf(buf,F); write(2, buf, strlen(buf));}
#define DBGPRINT1(F,A) {char buf[256]; sprintf(buf,F,A); write(2, buf, strlen(buf));}
#else
#define DBGPRINT0(F) ;
#define DBGPRINT1(F,A) ;
#endif
extern int errno;

struct sockaddr_un saddr;
struct sockaddr_in saddrin;

static int counter = 1;
static char *gmpath = "ispgm";

static long getl(s)
     int s;
{
  char sbuf[BUFSIZ], *p = sbuf;

  do {
    read(s, p, 1);
  } while (*p++);

  return atol(sbuf);
}

void startsocket(host, user, pidp, sockp)
     char *host, *user;
     int *pidp, *sockp;
{
  int pid, sock, errcount = 5;

  if (!strcmp(host,"local"))
    {
      char *tmp;		/* UNIX DOMAIN */

      if (!(tmp = (char*)getenv("TMPDIR")))
	tmp = TMPDIR;


      /* Create socket file name */

      sprintf(saddr.sun_path, "%s/%d-%d", tmp, getpid(), counter++);


      /* Create socket */

      if ((sock = socket(AF_UNIX,SOCK_STREAM,0)) == -1)
	OOPS("socket");


      /* Create subprocess */

      pid = FORK();

      if (pid == 0)
	{
	  /* CHILD */

	  /* We will never return from a execlp call */
	  /* If vfork() this is when really forking */

	  if (execlp(gmpath, gmpath, saddr.sun_path, (char*)0) == -1)
	    OOPS_EXIT("execlp");
	}
      else
	{
	  /* PARENT */

	  if (pid == -1)
	    OOPS("fork");


	  /* If GM alive, kill it */	      

	  if (kill(pid, 0) != 0)
	    OOPS("kill");


	  saddr.sun_family = AF_UNIX;

	  while (connect(sock,&saddr,sizeof(saddr)) == -1)
	    if (--errcount == 0) /* No connection, give up */
	      OOPS("connect")
	    else
	      sleep(2);		/* No connection, wait a little */

	  *sockp = sock;
	  *pidp = pid;
	}
    }
  else
    {
      char hostname[80];	/* INTERNET DOMAIN */

      /* Get the host name, "dunlop" or "dunlop.sics.se" */

      if (gethostname(hostname,sizeof(hostname)) == -1)
	OOPS("gethostname");


      /* Create socket */

      if ((sock = socket(AF_INET,SOCK_STREAM,0)) == -1) 
	OOPS("socket");


      /* Bind a name to socket, but let the system choose the name */

      saddrin.sin_family = AF_INET;
      saddrin.sin_addr.s_addr = INADDR_ANY;
      saddrin.sin_port = 0;

      if (bind(sock,&saddrin,sizeof(saddrin)) == -1)
	OOPS("bind");


      /* Get the name choosen */
      
      {
	int length = sizeof(saddrin);

	if (getsockname(sock,&saddrin,&length) == -1)
	  OOPS("getsockname");
      }


      /* Only serve one client */

      listen(sock, 1);


      /* Create subprocess */

      pid = FORK();


      if (pid == 0)
	{
	  char t[256];		/* CHILD */
	  char *argv[10];
	  char *display, *p;
	  int i = 0;

	  DBGPRINT0("In child\n");

	  if(!strchr(hostname,'.'))
	    {
	      int l = strlen(hostname);

	      hostname[l++] = '.';
	      getdomainname(hostname+l, sizeof(hostname)-l);
	    }

	  display = (char*)getenv("DISPLAY");

	  if (display == NULL)
	    {
	      fprintf(stderr,"DISPLAY shell variable is not set\n");
	      _exit(1);
	    }

	  argv[i++] = "rsh";
	  argv[i++] = host;
	  argv[i++] = "-n";
	  if (strcmp(user,""))
	    {
	      argv[i++] = "-l";
	      argv[i++] = user;
	    }
	  sprintf(t, "setenv DISPLAY %s; exec %s %s %d",
		display, gmpath, hostname, ntohs(saddrin.sin_port));
	  argv[i++] = t;
	  argv[i++] = (char *)0;

	  /* We will never return from a execvp call */
	  /* If vfork() this is when really forking */

	  DBGPRINT1("DISPLAY=%s\n",display);
	  DBGPRINT1("host=%s\n",host);
	  DBGPRINT1("hostname=%s\n",hostname);
	  DBGPRINT1("port=%d\n",ntohs(saddrin.sin_port));

	  if (execvp("rsh", argv) == -1)
	    OOPS_EXIT("execlp");
	}
      else
	{
	  int port, newsock;	/* PARENT */
	  struct hostent* h;
	  
	  DBGPRINT0("Forked but in parent\n");
	  fflush(stderr);

	  if (pid == -1)
	    OOPS("GM: fork");


	  /* Wait for connection */

	  if ((newsock = accept(sock,NULL,NULL)) == -1)
	    OOPS("accept");

	  close(sock);		/* Close old socket */

	  DBGPRINT0("Accepted\n");


	  /* Get server process id */

	  port = getl(newsock);

	  close(newsock);	/* Close new socket */

	  DBGPRINT0("Closed\n");

	  if ((sock = socket(AF_INET,SOCK_STREAM,0)) == -1)
	    OOPS("socket");

	  DBGPRINT0("Socket opened\n");

	  h = gethostbyname(host);

	  bzero((char*)&saddrin, sizeof(saddrin));
	  saddrin.sin_family = AF_INET;
	  saddrin.sin_port = htons(port);
	  bcopy((char*)h->h_addr, (char*)&saddrin.sin_addr, h->h_length);

	  while (connect(sock,(struct sockaddr*)&saddrin,sizeof(saddrin))== -1)
	    if (--errcount == 0) /* No connection, give up */
	      OOPS("connect")
	    else
	      sleep(2);		/* No connection, wait a little */


	  *sockp = sock;
	  *pidp = pid;
	  DBGPRINT0("Connected\n");
	}
    }
}

void endsocket(pid,s)
     int pid, s;
{
  int status;
  close(s);
  kill(pid,SIGUSR1);
  while (wait(&status) != pid) ;
  if (saddr.sun_path[0] != '\0')
    unlink(saddr.sun_path);   /* Remove socket */
}
