//
// oled test program
// Written by Larry Bank
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include "oled96.h"

int GetIPAddress(char *interface, char *address)
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) 
    {
        return -1;
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;  

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if((strcmp(ifa->ifa_name,"wlan0")==0 || strcmp(ifa->ifa_name,"eth0")==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return -1;
            }
            strcpy(interface,ifa->ifa_name);
            strcpy(address, host);
        }
    }

    freeifaddrs(ifaddr);
    return 0;
} /* GetIPAddress() */

void GetCurrentTime(char *time, char *date)
{
struct timeval tv;
struct tm *tmp;

	gettimeofday(&tv, NULL);
	tmp = localtime(&tv.tv_sec);
//	sprintf(time, "%02d:%02d%s", tmp->tm_hour, tmp->tm_min, tmp->tm_zone);
	sprintf(time, "%02d:%02d:%02d", tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	sprintf(date, "%02d/%02d/%04d", tmp->tm_mon+1, tmp->tm_mday, tmp->tm_year+1900);

} /* GetCurrentTime() */

int kbhit(void)
{
struct timeval tv;
fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);
	select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
} /* kbhit() */

int main(int argc, char *argv[])
{
int i;
char interface[16];
char address[16];
char szTime[16], szDate[16];

	GetIPAddress(interface, address);
	GetCurrentTime(szTime, szDate);

	i=oledInit(0x3c);
	if (i == 0)
	{
		oledFill(0); // fill with black
		oledWriteString(0,0,interface,0);
		oledWriteString(0,1,address,0);
		oledWriteString(0,3,szTime,1);
		oledWriteString(0,6,szDate,0);
		for (i=0; i<128; i++)
		{
			oledSetPixel(i, 60+abs((i&7)-4), 1);
//			oledSetPixel(127-i, 16+i, 1);
		}
		while (!kbhit())
		{
			GetCurrentTime(szTime, szDate);
			oledWriteString(0,3,szTime,1);
			usleep(1000000);
			i++;
			oledSetContrast((i&1)? 64: 192);
		}
		oledShutdown();
	}
   return 0;
} /* main() */
