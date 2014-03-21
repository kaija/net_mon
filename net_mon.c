#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/queue.h>
#include <errno.h>
#define DEBUG 0
#define IF_NAME_LEN	8
#define IP_LEN		16
#define MAX_IF_NUM	8

struct if_list{
	char name[IF_NAME_LEN];
	char ip[IP_LEN];
};
static struct if_list plist[MAX_IF_NUM];
static int if_count = 0;

int interface_list()
{
	int status = 0;
    int fd;
    struct ifconf ifc;
    int i;
    struct ifreq ifr[MAX_IF_NUM];
    int interface;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    ifc.ifc_buf = (char *)ifr;
    ifc.ifc_len = sizeof(ifr);
    if(ioctl(fd, SIOCGIFCONF, &ifc) == -1){
        printf("ioctl error\n");
        close(fd);
        return -1;
    }
    interface = ifc.ifc_len/sizeof(ifr[0]);
    if(DEBUG) printf("IF(%d)\tIP\n", interface);
	if_count = interface;
    for(i = 0; i < interface; i++){
        char ip[IP_LEN] = {'\0'};
        struct sockaddr_in *address = (struct sockaddr_in *)&ifr[i].ifr_addr;
        if(!inet_ntop(AF_INET, &address->sin_addr, ip, sizeof(ip))){
            printf("inet_ntop %d %s\n", errno, strerror(errno));
        }
        strcpy(plist[i].name, ifr[i].ifr_name);
		if(strlen(plist[i].ip) == 0) {
        	strcpy(plist[i].ip, ip);
		}else{
			if(strcmp(plist[i].ip, ip)!=0){
        		strcpy(plist[i].ip, ip);
				//ip changed\n
				status = 1;
			}
		}
        if(DEBUG) printf("%s\t%s\n", ifr[i].ifr_name, ip);
    }
    close(fd);
	return status;
}
int main()
{
	memset(&plist, 0, sizeof(struct if_list));
	while(1){
		if(interface_list() == 1){
			printf("IP changed\n");
		}
		sleep(1);
	}
	return 0;
}
