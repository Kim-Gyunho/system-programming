#include<stdlib.h>
#include<stdio.h>

void not();
void myhelp();
void mymove(char *, char *);
void myremove(char *, char *);
void mycopy(char *, char *);
void myfind(char *, char *);

int main(int argc, char *argv[]) {
	int n;
	extern char *optarg;
	extern int optind;

	if(argc == -1) {
		perror("argc");
		exit(1);
	}

	if(argc == 1) {
		printf("no option");
		not();
	}

	while((n = getopt(argc, argv, "mrcfh")) != -1) {
		switch(n) {
			case 'm':
				mymove(argv[2], argv[3]);
				break;
			case 'r':
				myremove(argv[2], argv[3]);
				break;
			case 'c':
				mycopy(argv[2], argv[3]);
				break;
			case 'f':
				myfind(argv[2], argv[3]);
				break;
			case 'h':
				myhelp();
				break;
			default:
				break;
		}
	}
	return 0;
}

void not() {
	printf("더 많은 정보를 보려면 'mtdir -h'하십시오.\n");
	exit(0);
}

void myhelp() {
	printf("-m : mv option\n");
	printf("-r : rm option\n");
	printf("-c : cp option\n");
	printf("-f : find option\n");
}

void mymove(char *path1, char *path2) {
	printf("mymove\n");
	printf("path1 : %s, path2 : %s\n", path1, path2);
}

void myremove(char *path1, char *path2) {
	printf("myremove\n");
}

void mycopy(char *path1, char *path2) {
	printf("mycopy\n");
}

void myfind(char *path, char *filename) {
	printf("myfind\n");
}
