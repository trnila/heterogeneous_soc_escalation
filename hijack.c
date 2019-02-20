#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


int main() {
	int d = open("/dev/test", O_RDWR);
	if(d < 0) {
		perror("/dev/test");
		return 1;
	}

	for(;;) {
		if(open("/secured", O_RDWR) >= 0) {
			printf("access granted - opened");
			system("bash");
			return 1;
		}
		printf("%d\n", getuid());
		perror("/secured: ");
		usleep(100 * 1000);
	}
}
