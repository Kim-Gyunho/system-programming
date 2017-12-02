#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>

void not();
void myhelp();
void mymove(int, char *, char *);
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
				mymove(argc, argv[2], argv[3]);
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
	printf("더 많은 정보를 보려면 'mydir -h'하십시오.\n");
	exit(0);
}

void myhelp() {
	printf("-m : mv option\n");
	printf("-r : rm option\n");
	printf("-c : cp option\n");
	printf("-f : find option\n");
}

void mymove(int argc, char *path1, char *path2) {
	struct stat buf;
	char path[BUFSIZ];
	char tmp[30];
	int i=0, j=0;
	
	printf("mymove\n");
	printf("path1 : %s, path2 : %s\n", path1, path2);
	
	stat(path2, &buf);
	
	if(argc != 4){
		perror("$mydir -m [dir] [dir]\n");
		exit;
	}
	if(access(path1, F_OK) == -1 && errno == ENOENT){
		perror("moved file");
		exit(1);
	}else if(access(path2, F_OK) == -1 && errno == ENOENT){
		perror("des dir");
		exit(1);
	}
	
	tmp[0] = '\0';
	while(path1[i] != '\0'){
		tmp[j] = path1[i];
		j++;
		if(path1[i] == '/'){
			tmp[0] = '\0';
			j = 0;
		}
		i++;
	}
	tmp[j] = '\0';
	
	if(S_ISDIR(buf.st_mode)){
		sprintf(path, "%s/%s", path2, tmp);
		rename(path1, path);
	}else
		printf("not dirfile");
}
	

void myremove(char *path, int arnum) {
	pid_t pid;
	DIR *dp;
	struct dirent *dent;
	int count;

	printf("myremove\n");

	if(arnum != 3) {
		printf("r옵션의 인자는 1개입니다.");
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
			printf("Dir %s의 내부에 파일이 존재합니다.\n", path);
			printf("디렉터리 내부를 비우시던가 -pr옵션을 사용하세요\n");
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
	printf("%s가 삭제되었습니다.\n", path);
}

void mycopy(char *path1, char *path2) {
	printf("mycopy\n");

}

void myfind(char *path, char *filename) {
	printf("myfind\n");
}
