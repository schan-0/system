#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>    // PATH_MAX

int main() {


	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	printf("%s\n", path);
	return 0;


}

