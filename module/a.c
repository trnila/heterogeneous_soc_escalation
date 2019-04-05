#include <unistd.h>
#include <fcntl.h>

int main() {
	int f = open("/sys/module/test2/parameters/su", O_RDONLY);
	char c;
	read(f, &c, 1);
	execl("/bin/bash", "bash", NULL);
}
