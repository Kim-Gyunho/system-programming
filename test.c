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
		printf("���ڰ� �߸��Ǿ� �ֽ��ϴ�.\n");
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
				printf("%s �� �߸��� �ɼ��Դϴ�.\n", argv[1]);
				ErrorMsg();
				break;
		}
		break;
	}
	return 0;
}

void ErrorMsg()
{
	printf("�� ���� ������ ������ 'mydir -h'�Ͻʽÿ�.\n");
	exit(0);
}

void DirHelp()
{
	printf("--- mydir ���� ---\n");
	printf("--- mv : move �ɼ� ---\n");
	printf("mydir -mv [������ ���] [�̵��� ���]\n");
	printf("������ ��ο��� �̵��� ��η� ����� ��ġ�� �̵��Ѵ�.\n");
	printf("����� ��ġ�� �̵��� �� �̵��� ��ο� ��� ���ϸ��� ���ٸ� �̸��� �����Ǿ� �̵��Ѵ�.\n");
	printf("--- --- ---\n");
	printf("--- r : ���丮 remove �ɼ� ---\n");
	printf("mydir -r [������ ���丮��]\n");
	printf("�ش� ����� ����丮��� �����Ѵ�.\n");
	printf("�� ���丮�� �ƴ϶�� ����־�� �Ѵ�.\n");
	printf("--- --- ---\n");
	printf("--- rm : ���� remove �ɼ� ---\n");
	printf("mydir -rm [������ ���ϸ�]\n");
	printf("�ش� ����� �����Ѵ�.\n");
	printf("--- --- ---\n");
	printf("--- cp : copy �ɼ� ---\n");
	printf("mydir -cp [������ ���] [������ ���]\n");
	printf("������ ��ο��� ������ ��η� ����� �����Ѵ�.\n");
	printf("--- --- ---\n");
	printf("--- f : find �ɼ� ---\n");
	printf("mydir -f [�˻��� ���] -name [�˻��� Ű����]\n");
	printf("�ش� ��ο��� Ű���� ��ġ�ϴ� �̸��� ���� ������ ����Ѵ�..\n");
	printf("mydir -f [�˻��� ���] -alike [�˻��� Ű����]\n");
	printf("�ش� ��ο��� Ű���带 �����ϴ� �̸��� ���� ������ ����Ѵ�..\n");
	printf("mydir -f [�˻��� ���] -size [ã������ ������ �뷫���� ũ��]\n");
	printf("�ش� ��ο��� Ű�����̻��� ũ�⸦ ���� ������ ����Ѵ�.\n");
	printf("mydir -f [�˻��� ���] -dir\n");
	printf("�ش� ��ο��� ���丮�� ����Ѵ�.\n");
	printf("mydir -f [�˻��� ���] -time [������ �������κ��� ���� �ϼ�]\n");
	printf("�ش� ��ο��� ������ �������� Ű���� �̻�ŭ�� ��¥�� ���� ������ ����Ѵ�.\n");
	printf("--- --- ---\n");
}

int Is_File_In(char *path, char *filename)
{
	DIR *dp;
	struct dirent *dent;
	int count;
	
	if((dp = opendir(path)) == NULL)
	{
		perror("�߸��� ����Դϴ�.\n");
		ErrorMsg();
		exit(1);
	}

	while((dent = readdir(dp)))
	{
		if(dent->d_name[0] != '.') 
		{	
			if(!strcmp(dent->d_name, filename))
			{
				printf("���� �̸��� ������ �̹� �����մϴ�.\n");
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
		printf("m�ɼ��� �߰� ���ڴ� 2���Դϴ�.\n");
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
			printf("�ߺ��Ǵ� �̸��� ������ �����ϸ� �̵��� �� �����ϴ�.\n");
			ErrorMsg();
			exit(1);
		}
	}

	else
	{
		printf("�߸��� �ɼ��Դϴ�.\n");
		ErrorMsg();
		exit(1);
	}
	printf("%s�� %s�� �̵��߽��ϴ�.\n");
}

void DirRemove(char *opt, char *path, int argc) 
{
	pid_t pid;
	DIR *dp;
	struct dirent *dent;
	int count = 0;

	if(argc != 3) 
	{
		printf("r�ɼ��� �߰� ���ڴ� 1���Դϴ�.\n");
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
				printf("��ΰ� �߸��Ǿ��ų� ���͸��� �ƴմϴ�.\n");
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
			printf("%s�� ���ο� ������ �����մϴ�.\n", path);
			printf("���͸� ���θ� ���ô��� -rm�ɼ��� ����ϼ���\n");
			ErrorMsg();
			exit(1);
		}
	}
	else {
		printf("�߸��� �ɼ��Դϴ�.\n");
		ErrorMsg();
		exit(1);
	}
	printf("%s�� �����Ǿ����ϴ�.\n", path);
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
		printf("c�ɼ��� �߰� ���ڴ� 2���Դϴ�.\n");
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
		printf("�߸��� �ɼ��Դϴ�.\n");
		ErrorMsg();
		exit(1);
	}
	printf("%s�� %s�� ���� �Ͽ����ϴ�.\n", path1, path2);
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
			printf("f�ɼ��� ������ ������ �߸��Ǿ� �ֽ��ϴ�..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL) 
		{
			printf("�߸��� ����Դϴ�.\n");
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
			printf("f�ɼ��� ������ ������ �߸��Ǿ� �ֽ��ϴ�..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL) 
		{
			printf("�߸��� ����Դϴ�.\n");
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
			printf("f�ɼ��� ������ ������ �߸��Ǿ� �ֽ��ϴ�..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL)
		{
			printf("�߸��� ����Դϴ�.\n");
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
			printf("f�ɼ��� ������ ������ �߸��Ǿ� �ֽ��ϴ�..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL)
		{
			printf("�߸��� ����Դϴ�.\n");
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
			printf("f�ɼ��� ������ ������ �߸��Ǿ� �ֽ��ϴ�..\n");
			ErrorMsg();
			exit(1);
		}

		if((dp = opendir(cwd)) == NULL)
		{
			printf("�߸��� ����Դϴ�.\n");
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
		printf("�߸��� �ɼ��Դϴ�.\n");
		ErrorMsg();
		exit(1);
	}
}
