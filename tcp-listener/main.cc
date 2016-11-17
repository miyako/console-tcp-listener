#include "main.h"

static void usage(void)
{
	fprintf(stderr, "Usage: tcp-listener -p port -t timeout -\n\n");
	fprintf(stderr, "One-shot TCP listener\n\n");
	fprintf(stderr, " -%c port: %s\n", OPT_PORT , "tcp port number to listen (default=8080)");
	fprintf(stderr, " -%c timeout: %s\n", OPT_TIMEOUT , "time to keep listening in secounds (default=30)");
	fprintf(stderr, " -%c : %s\n", OPT_STDIN , "use stdin as tcp response");
	fprintf(stderr, " -%c : %s\n", OPT_ALL , "accept connection from any address (default=loopback only)");
	
	exit(1);
}

extern OPTARG_T optarg;
extern int optind, opterr, optopt;

#ifdef WIN32
optarg = 0;
opterr = 1;
optind = 1;
optopt = 0;
int getopt(int argc, OPTARG_T *argv, OPTARG_T opts) {
	static int sp = 1;
	register int c;
	register OPTARG_T cp;
	
	if(sp == 1)
		if(optind >= argc ||
			 argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(wcscmp(argv[optind], L"--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=wcschr(opts, c)) == NULL) {
		ERR(L": illegal option -- ", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR(L": option requires an argument -- ", c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}
#endif

int main(int argc, char *argv[])
{
	//http request buffer
	char buf[BUFLEN];
	memset(buf, '\0', BUFLEN);

	//http response buffer
	size_t size = BUFLEN;
	char *response = (char *)calloc(size, sizeof(char));
	if(!response) {
		exit(1);
	}

	//arguments
	int port = 8080;
	int timeout = 30;
	int ch;
	int any = 0;
	
	while ((ch = getopt(argc, argv, OPT_LIST)) != -1){
		switch (ch){
			case OPT_PORT:
				port = atoi(optarg);
				break;
			case OPT_TIMEOUT:
				timeout = atoi(optarg);
				break;
			case OPT_ALL:
				any = 1;
				break;
			case OPT_STDIN:
			{
				//http response buffer
				size_t ret, i = 0;
#ifdef WIN32
				_setmode(_fileno(stdin), _O_BINARY);
				while((ret = _read(_fileno(stdin), &response[i], BUFLEN)) > 0) {
					i += ret;
					size += BUFLEN;
					response = (char *)realloc(response, size);
					if(!response) {
						exit(1);
					}
				}
#else
				freopen(NULL, "rb", stdin);
				while((ret = fread(&response[i], sizeof(char), BUFLEN, stdin)) > 0) {
					i += ret;
					size += BUFLEN;
					response = (char *)realloc(response, size);
					if(!response) {
						exit(1);
					}
				}
#endif
			}
				break;
			case OPT_HELP:
			default:
				usage();
				break;
		}
	}
		
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	
	int one = 1, client_fd;
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);
	
#ifdef WIN32
	WSADATA wsaData;
	SOCKET sock = INVALID_SOCKET;
	if(NO_ERROR == WSAStartup(MAKEWORD(2,2), &wsaData)) {
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	};
#else
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
	
	if (sock < 0) {
		fprintf(stderr, "socket() failed ");
	}else{
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(int));
		svr_addr.sin_family = AF_INET;
		if(any) {
			svr_addr.sin_addr.s_addr = INADDR_ANY;
		}else{
			svr_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		}
		svr_addr.sin_port = htons(port);
		if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
			__close(sock);
			fprintf(stderr, "bind() failed ");
		}else{
			listen(sock, 5);
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			
			if(select((int)(sock+1), &rfds, (fd_set *)0, (fd_set *)0, &tv) > 0) {
				client_fd = accept((int)sock, (struct sockaddr *) &cli_addr, &sin_len);
				if (client_fd != -1) {
					recv(client_fd, buf, BUFLEN, 0);
					printf("%s", buf);
					send(client_fd, response, (int)size, 0);
					__close(client_fd);
				}
			}else{
				fprintf(stderr, "timeout");
			}
			__close(sock);
		}
	}
	
	free(response);
	
#ifdef WIN32
	WSACleanup();
#endif
	
	return 0;
}