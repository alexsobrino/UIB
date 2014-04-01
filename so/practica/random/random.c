#include <stdlib.h>

int main () {
	srand(getpid());
	int i,m;
	for (i=1;i<=100;i++) {
		m = rand()%100;
		printf("%d\n",m);
	}
	return 0;
}
