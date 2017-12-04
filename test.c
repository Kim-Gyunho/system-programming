#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<time.h>

void ErrorMsg();
void DirHelp();
int Is_File_In(char *, char *);
void DirMove(char *, char *, char *, int);
void DirRemove(char *, char *, int);
void DirCopy(char *, char *, char *, int);
void DirFind(char *, char *, char *, int);

int main(int argc, char *argv[]) 
{
	int n;
	extern char *optarg;
	extern int optind;

	if(argc == -1) 
	{
		perror("argc");
		exit(1);
	}

	if(argc < 2 || argc > 5)
	{
		printf("인자가 잘못되어 있습니다.\n");
		ErrorMsg();
		exit(1);
	}
	
	while((n = getopt(argc, argv, "mrcfh")) != -1)
	{
		switch(n) {
			case 'm':
				DirMove(argv[1], argv[2], argv[3], argc);
				break;
			case 'r':
				DirRemove(argv[1], argv[2], argc);
				break;
			case 'c':
				DirCopy(argv[1], argv[2], argv[3], argc);
				break;
			case 'f':
				DirFind(argv[2], argv[3], argv[4], argc);
				break;
			case 'h':
				DirHelp();
				break;
			default:
				printf("%s 는 잘못된 옵션입니다.\n", argv[1]);
				ErrorMsg();
				break;
		}
		break;
	}
	return 0;
}

void ErrorMsg()
{
	printf("더 많은 정보를 보려면 'mydir -h'하십시오.\n");
	exit(0);
}

void DirHelp()
{
	printf("--- mydir 사용법 ---\n");
	printf("--- mv : move 옵션 ---\n");
	printf("mydir -mv [원본의 경로] [이동할 경로]\n");
	printf("원본의 경로에서 이동할 경로로 대상의 위치를 이동한다.\n");
	printf("대상의 위치를 이동할 때 이동할 경로에 대상 파일명이 없다면 이름이 수정되어 이동한다.\n");
	printf("--- --- ---\n");
	printf("--- r : 디렉토리 remove 옵션 ---\n");
	printf("mydir -r [삭제할 디렉토리명]\n");
	printf("해당 대상이 빈디렉토리라면 삭제한다.\n");
	printf("빈 디렉토리가 아니라면 비워주어야 한다.\n");
	printf("--- --- ---\n");
	printf("--- rm : 파일 remove 옵션 ---\n");
	printf("mydir -rm [삭제할 파일명]\n");
	printf("해당 대상을 삭제한다.\n");
	printf("--- --- ---\n");
	printf("--- cp : copy 옵션 ---\n");
	printf("mydir -cp [원본의 경로] [복사할 경로]\n");
	printf("원본의 경로에서 복사할 경로로 대상을 복사한다.\n");
	printf("--- --- ---\n");
	printf("--- f : find 옵션 ---\n");
	printf("mydir -f [검색할 경로] -name [검색할 키워드]\n");
	printf("해당 경로에서 키워드 일치하는 이름을 가진 파일을 출력한다..\n");
	printf("mydir -f [검색할 경로] -alike [검색할 키워드]\n");
	printf("해당 경로에서 키워드를 포한하는 이름을 가진 파일을 출력한다..\n");
	printf("mydir -f [검색할 경로] -size [찾으려는 파일의 대략적인 크기]\n");
	printf("해당 경로에서 키워드이상의 크기를 가진 파일을 출력한다.\n");
	printf("mydir -f [검색할 경로] -dir\n");
	printf("해당 경로에서 디렉토리만 출력한다.\n");
	printf("mydir -f [검색할 경로] -time [마지막 수정으로부터 지난 일수]\n");
	printf("해당 경로에서 마지막 수정에서 키워드 이상만큼의 날짜가 지난 파일을 출력한다.\n");
	printf("--- --- ---\n");
}

int Is_File_In(char *path, char *filename)
{
	DIR *dp;
	struct dirent *dent;
	int count;
	
	if((dp = opendir(path)) == NULL)
	{
		perror("잘못된 경로입니다.\n");
		ErrorMsg();
		exit(1);
	}

	while((dent = readdir(dp)))
	{
		if(dent->d_name[0] != '.') 
		{	
			if(!strcmp(dent->d_name, filename))
			{
				printf("같은 이름의 파일이 이미 존재합니다.\n");
				return 1;
			}
		}
	}
	closedir(dp);
	return 0;
}

void DirMove(char *opt, char *path1, char *path2, int argc) 
{ 
	struct stat dirbuf;
	struct dirent *dent;
	FILE *rfp;
	FILE *wfp;
	DIR *dp;
	pid_t pid;
	char buf[BUFSIZ];
	char tmp[256];
	int isfl;

	if(argc != 4) 
	{
		printf("m옵션의 추가 인자는 2개입니다.\n");
		ErrorMsg();
	}
	
	if(!strcmp(opt, "-mv"))
	{
		isfl = Is_File_In(path2, path1);
	
		if(isfl == 0) 
		{
			stat(path1, &dirbuf);
	
			if(!S_ISDIR(dirbuf.st_mode))
			{
				if((rfp = fopen(path1, "r")) == NULL) 
				{
					perror("move-fopen");
					exit(1);
				}

				sprintf(tmp, "%s/%s", path2, path1);
			
				if((wfp = fopen(tmp, "w")) == NULL)
				{
					perror("move-fopen");
					exit(1);
				}

				while(fgets(buf, BUFSIZ, rfp) != NULL) 
				{
					fputs(buf, wfp);
				}
				fclose(rfp);
				fclose(wfp);
				
				unlink(path1);
			}

			else
			{
				switch(pid = fork())
				{
					case -1:
						perror("move-fork");
						exit(1);
						break;
					case 0:
						if(execlp("mv", "mv", path1, path2, (char *)NULL) == -1)
						{
							perror("move-exec");
							exit(1);
						}
						exit(0);
					default:
						break;
				}
			}
		}

		else 
		{
			printf("중복되는 이름의 파일이 존재하면 이동할 수 없습니다.\n");
			ErrorMsg();
			exit(1);
		}
	}

	else
	{
		printf("잘못된 옵션입니다.\n");
		ErrorMsg();
		exit(1);
	}
	printf("%s를 %s로 이동했습니다.\n");
}

void DirRemove(char *opt, char *path, int argc) 
{
	pid_t pid;
	DIR *dp;
	struct dirent *dent;
	int count = 0;

	if(argc != 3) 
	{
		printf("r옵션의 추가 인자는 1개입니다.\n");
		ErrorMsg();
		exit(1);
	}
	
	if(!strcmp(opt, "-rm"))
	{ 
		switch(pid = fork())
		{
			case -1:
				perror("remove-fork");
				exit(1);
				break;
			case 0:
				if(execlp("rm", "rm", "-r", path, (char *)NULL) == -1)
				{
					perror("remove-exec");
					exit(1);
				}
				exit(0);
				break;
			default:
				break;
		}
	}

	else if(!strcmp(opt, "-r"))
	{
		if((dp = opendir(path)) == NULL)
		{
				printf("경로가 잘못되었거나 디렉터리가 아닙니다.\n");
				ErrorMsg();
				exit(1);
		}

		while((dent = readdir(dp))) 
		{
			count++;
		}

		closedir(dp);

		if(count == 2) 
		{
			rmdir(path);
		}
		else
		{
			printf("%s의 내부에 파일이 존재합니다.\n", path);
			printf("디렉터리 내부를 비우시던가 -rm옵션을 사용하세요\n");
			ErrorMsg();
			exit(1);
		}
	}
	else {
		printf("잘못된 옵션입니다.\n");
		ErrorMsg();
		exit(1);
	}
	printf("%s가 삭제되었습니다.\n", path);
}

void DirCopy(char *opt, char *path1, char *path2, int argc)
{
	struct stat dirbuf;
	FILE *rfp;
	FILE *wfp;
	pid_t pid;
	char buf[BUFSIZ];
	char tmp[256];

	if(argc != 4)
	{
		printf("c옵션의 추가 인자는 2개입니다.\n");
		ErrorMsg();
	}
	
	if(!strcmp(opt, "-cp"))
	{
		stat(path1, &dirbuf);
	
		if(!S_ISDIR(dirbuf.st_mode)) 
		{
			if((rfp = fopen(path1, "r")) == NULL)
			{
				perror("move-fopen");
				exit(1);
			}

			sprintf(tmp, "%s/%s", path2, path1);
		
			if((wfp = fopen(tmp, "w")) == NULL) 
			{
				perror("move-fopen");
				exit(1);
			}

			while(fgets(buf, BUFSIZ, rfp) != NULL) 
			{
				fputs(buf, wfp);
			}
			fclose(rfp);
			fclose(wfp);
		}
		else 
		{
			switch(pid = fork())
			{
				case -1:
					perror("cop-fork");
					exit(1);
					break;
				case 0:
					if(execlp("cp", "cp", path1, path2, (char *)NULL) == NULL)
					{
						perror("copy=exec");
						exit(1);
					}
					exit(0);
					break;
				default:
					break;
			}
		}
	}

	else
	{
		printf("잘못된 옵션입니다.\n");
		ErrorMsg();
		exit(1);
	}
	printf("%s를 %s로 복사 하였습니다.\n", path1, path2);
}

void DirFind(char *path, char *opt, char *filename, int argc) 
{
	pid_t pid;
	time_t tt;
	DIR *dp;
	struct dirent *dent;
	struct stat buf;
	char *cwd;
	char *result;
	int filesize = 0;
	int itime = 0;
	int mtime = 0;
	
	chdir(path);
	cwd = getcwd(NULL, BUFSIZ);

	if(!strcmp(opt, "-name")) 
	{
		if(argc != 5)
		{
			printf("f옵션의 인자의 갯수가 잘못되어 있습니다..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL) 
		{
			printf("잘못된 경로입니다.\n");
			ErrorMsg();
			exit(1);
		}

		while((dent = readdir(dp)))
		{
			if(dent->d_name[0] != '.') 
			{
				result = strstr(dent->d_name, filename);

				if(!strcmp(dent->d_name, filename)) 
				{
					printf("%s/%s\n", cwd, dent->d_name);
				}
			}
		}
		closedir(dp);
	}

	else if(!strcmp(opt, "-alike")) 
	{
		if(argc != 5)
		{
			printf("f옵션의 인자의 갯수가 잘못되어 있습니다..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL) 
		{
			printf("잘못된 경로입니다.\n");
			ErrorMsg();
			exit(1);
		}

		while((dent = readdir(dp)))
		{
			if(dent->d_name[0] != '.') 
			{
				result = strstr(dent->d_name, filename);

				if(result != NULL) 
				{
					printf("%s/%s\n", cwd, dent->d_name);
				}
			}
		}
		closedir(dp);
	}

	else if(!strcmp(opt, "-size"))
	{
		if(argc != 5)
		{
			printf("f옵션의 인자의 갯수가 잘못되어 있습니다..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL)
		{
			printf("잘못된 경로입니다.\n");
			ErrorMsg();
			exit(1);
		}

		while((dent = readdir(dp)))
		{
			if(dent->d_name[0] != '.')
			{
				stat(dent->d_name, &buf);
				filesize = atoi(filename);

				if(((int)buf.st_size) >= filesize)
				{
					printf("%s/%s  size : %d\n", cwd, dent->d_name, (int)buf.st_size);
				}
			}
		}
	}

	else if(!strcmp(opt, "-dir"))
	{
		if(argc != 4)
		{
			printf("f옵션의 인자의 갯수가 잘못되어 있습니다..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL)
		{
			printf("잘못된 경로입니다.\n");
			ErrorMsg();
			exit(1);
		}

		while((dent = readdir(dp)))
		{
			if(dent->d_name[0] != '.')
			{
				stat(dent->d_name, &buf);

				if(S_ISDIR(buf.st_mode))
				{
					printf("%s/%s\n", cwd, dent->d_name);
				}
			}
		}
	}

	else if(!strcmp(opt, "-time"))
	{
		if(argc != 5)
		{
			printf("f옵션의 인자의 갯수가 잘못되어 있습니다..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL)
		{
			printf("잘못된 경로입니다.\n");
			ErrorMsg();
			exit(1);
		}

		itime = atoi(filename);
		mtime = itime * 3600 * 24;
		time(&tt);
		
		while((dent = readdir(dp)))
		{
			if(dent->d_name[0] != '.')
			{
				stat(dent->d_name, &buf);
				
				if((buf.st_mtime + mtime) <= (int)tt )
				{
					printf("%s/%s\n", cwd, dent->d_name);
				}
			}
		}
	}

	else
	{
		printf("잘못된 옵션입니다.\n");
		ErrorMsg();
		exit(1);
	}
}
