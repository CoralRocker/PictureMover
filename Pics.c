#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "Vector.h"
#include <string.h>

int main(int argc, char *argv[])
{
    	int f_d;
	char src[256], dst[256], *path, o_path[1024];
	getcwd(o_path, 1024);
	struct stat finfo;

	printf("What is the relative (or absolute) position of the directory to send pictures to?: ");
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
			printf("%s: invalid file\n", path);
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

	printf("\nHere is the finalized list of available directories to send the picture(s) to: \n");
	for(int i = 0; i < dirs->size; i++)
	{
		printf("Directory (%3d): %s\n", i, atVector(dirs, i));
	}

	puts(" ");

	printf("Where are the pictures to move to the destination?: ");
	scanf("%s", src);

	folder = opendir(src);

	while((dp = readdir(folder))!=NULL)
	{
		if(dp->d_name[0] == '.')
			continue;
		printf("\nFile: %s\n", dp->d_name);
		printf("Move file?(y/N): ");
		char answer[16];
		scanf("%s", answer);
		if(answer[0] == 'y' || answer[0] == 'Y')
		{
			printf("Move file\nWhere to move file?:\n");		
			for(int i = 0; i < dirs->size; i++)
			{
				printf("Directory (%3d): %s\n", i, atVector(dirs, i));
			}
			printf("NUMBER OF DIRECTORY TO SEND FILE TO: ");
			int whereto;
			scanf("%d", &whereto);
			printf("%d\n", whereto);
			FILE *in = fopen(dp->d_name, "r");
			char* name = malloc(sizeof(char)*1536);
			strcat(name, atVector(dirs, whereto));
			strcat(name, "/");
			strcat(name, dp->d_name);
			printf("File: %s\n", name);
			FILE *out = fopen(name, "w");
			printf("INFILE: %p\nOUTFILE: %p\n", in, out);
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

