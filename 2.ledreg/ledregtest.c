#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./ledtest /dev/100ask_led0 on
 * ./ledtest /dev/100ask_led0 off
 */
int main(int argc, char *argv[])
{
	int fd;
	char status;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: ./ledregtest <on | off>\n");
		return -1;
	}

	/* 2. 打开文件 */
	fd = open("/dev/qztxledreg", O_RDWR);
	if (fd == -1)
	{
		printf("can not open file /dev/qztxledreg \n");
		return -1;
	}

    printf("argv[1] = %s\r\n",argv[1]);
	/* 3. 写文件 */
	if (0 == strcmp(argv[1], "on"))
	{
		status = 1;
		write(fd, &status, 1);
	}
	else
	{
		status = 0;
		write(fd, &status, 1);
	}
	
	close(fd);
	
	return 0;
}


