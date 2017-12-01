#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>

void not();
void myhelp();
void mymove(char *, char *);
void myremove(char *, int);
void mycopy(char *, char *);
void myfind(char *, char *);

int optionp = 0;

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

	while((n = getopt(argc, argv, "pmrcfh")) != -1) {
		switch(n) {
			case 'p':
				optionp = 1;
				break;
			case 'm':
				mymove(argv[2], argv[3]);
				break;
			case 'r':
				myremove(argv[2], argc);
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
	printf("�� ���� ������ ������ 'mydir -h'�Ͻʽÿ�.\n");
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

void myremove(char *path, int arnum) {
	pid_t pid;
	DIR *dp;
	struct dirent *dent;
	int count;

	printf("myremove\n");

	if(arnum != 3) {
		printf("r�ɼ��� ���ڴ� 1���Դϴ�.");
		not();
	}

	count = 0;

	if((dp = opendir(path)) == NULL) {
		perror("oprndir");
		exit(1);
	}
	
	if(optionp == 0) { 
		while(dent = readdir(dp)) {
			count++;
		}

		closedir(dp);

		if(count == 2) {
			rmdir(path);
		}
		else {
			printf("Dir %s�� ���ο� ������ �����մϴ�.\n", path);
			printf("���͸� ���θ� ���ô��� -pr�ɼ��� ����ϼ���\n");
		}
	}
	else {
		switch(pid = fork()) {
			case -1:
				perror("remove-fork");
				exit(1);
				break;
			case 0:
				if(execlp("rm", "rm", "-r", path, (char *)NULL) == -1) {
					perror("remove-exec");
					exit(1);
				}
				break;
			default:
				break;
		}
	}
	printf("%s�� �����Ǿ����ϴ�.\n", path);
}

void mycopy(char *path1, char *path2) {
	printf("mycopy\n");

}

void myfind(char *path, char *filename) {
	printf("myfind\n");
}
