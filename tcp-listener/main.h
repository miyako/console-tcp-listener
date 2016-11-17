#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#endif  /* WIN32 */

#ifdef WIN32
#define __close closesocket
#else
#define __close close
#endif

#define OPT_PORT				'p'
#define OPT_TIMEOUT			't'
#define OPT_STDIN				'-'
#define OPT_HELP				'h'
#define OPT_ALL					'a'
#define OPT_LIST				"p:t:-ha"
#define BUFLEN					1024
#define RESPONSELEN			32768

#ifdef WIN32
#define OPTARG_T wchar_t*
#define main wmain
#define NULL	0
#define EOF	(-1)
#define ERR(s, c)	if(opterr){\
char errbuf[2];\
errbuf[0] = c; errbuf[1] = '\n';\
fputws(argv[0], stderr);\
fputws(s, stderr);\
fputwc(c, stderr);}
#else
#define OPTARG_T char*
#endif  /* WIN32 */

#ifdef __GNUC__
#include <getopt.h>
#endif

#ifndef __GNUC__
#ifndef _WINGETOPT_H_
#define _WINGETOPT_H_
#ifdef __cplusplus
extern "C" {
#endif
    extern int opterr;
    extern int optind;
    extern int optopt;
    extern OPTARG_T optarg;
    extern int getopt(int argc, OPTARG_T *argv, OPTARG_T opts);
#ifdef __cplusplus
}
#endif
#endif  /* _WINGETOPT_H_ */
#endif  /* !__GNUC__ */

#endif  /* __MAIN_H__ */