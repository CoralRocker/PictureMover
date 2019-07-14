#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "C-Vector-Generic/Vector.h"
#include <string.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

int main(int argc, char *argv[])
{
    	int f_d;
	char src[256], dst[256], *path, o_path[1024];
	getcwd(o_path, 1024);
	struct stat finfo;

	printf("What is the relative (or absolute) position of the directory to send file(s) to?: ");
	scanf("%s", dst);

	DIR *folder = opendir(dst);
	struct dirent *dp;

	vector *dirs = initVector();

	while((dp = readdir(folder))!=NULL)
	{
		if(dp->d_name[0] == '.')
			continue;
		path = malloc(sizeof(char)*1024);
		chdir(dst);
		getcwd(path, 1024);
		strcat(path, "/");
		strcat(path, dp->d_name);
		chdir(o_path);
		f_d = open(path, O_RDONLY);
		if(f_d < 0)
		{
			printf("%s%s: invalid file\n%s",KRED, path, KRED);
			break;
		}
		fstat(f_d, &finfo);
		close(f_d);
		if(S_ISDIR(finfo.st_mode))	
		{
			pushBackVector(dirs,NULL);
			assignVector(dirs, dirs->size-1, malloc(sizeof(char)*1024));
			memcpy(backVector(dirs), path, sizeof(char)*1024);
		}
		free(path);
	
	}
	chdir(o_path);
	closedir(folder);

	printf("\nHere is the finalized list of available directories to send the file(s) to: \n");
	for(int i = 0; i < dirs->size; i++)
	{
		printf("%sDirectory (%3d): %s\n%s", KGRN, i, atVector(dirs, i), KNRM);
	}

	puts(" ");

	printf("Where are the files to move to the destination?: ");
	scanf("%s", src);

	folder = opendir(src);

	while((dp = readdir(folder))!=NULL)
	{
		if(dp->d_name[0] == '.')
			continue;
		printf("\nFile: %s\n", dp->d_name);
		printf("%sMove file?(y/N): %s", KCYN, KNRM);
		char answer[16];
		scanf("%s", answer);
		if(answer[0] == 'y' || answer[0] == 'Y')
		{
			printf("%sMove file\nWhere to move file?:\n%s", KCYN, KNRM);		
			for(int i = 0; i < dirs->size; i++)
			{
				printf("Directory (%s%3d%s): %s\n", KGRN, i, KNRM, atVector(dirs, i));
			}
			printf("%sNUMBER OF DIRECTORY TO SEND FILE TO: %s", KCYN, KNRM);
			int whereto;
			scanf("%d", &whereto);
			printf("%d\n", whereto);
			FILE *in = fopen(dp->d_name, "r");
			char* name = malloc(sizeof(char)*1536);
			strcat(name, atVector(dirs, whereto));
			strcat(name, "/");
			strcat(name, dp->d_name);
			printf("%sFile: %s\n%s", KGRN, name, KNRM);
			FILE *out = fopen(name, "w");
			printf("%sINFILE: %p\nOUTFILE: %p\n%s", KGRN, in, out, KNRM);
			char c;
			while((c = fgetc(in))!=EOF)
					putc(c, out);
			free(name);
			fclose(in);
			fclose(out);
		}
		puts(" ");
	}
	closedir(folder);	
	rfreeVector(dirs);
	return 0;
}

