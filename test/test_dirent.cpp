#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Function to print file properties
void printFileProperties(struct dirent *file)
{
	struct stat fileStat;
	if (stat(file->d_name, &fileStat) < 0)
	{
		perror("stat");
		return;
	}

	printf("Name: %s\n", file->d_name);
	printf("Size: %ld bytes\n", fileStat.st_size);
	printf("Last modified: %s", ctime(&fileStat.st_mtime));
	printf("Chmod: %x\n", fileStat.st_mode);
	printf("print: %o\n", (S_IRUSR | S_IWUSR | S_IXUSR) & fileStat.st_mode);
	printf("--------------------------------\n");
	// S_ISDIR(filesTat.st_mode);
}

int main()
{
	// Open the current directory
	DIR *dir = opendir(".");
	if (dir == NULL)
	{
		perror("Unable to open directory");
		return 1;
	}

	// Read directory contents
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
		{ // Check if it's a regular file
			printFileProperties(entry);
		}
	}

	struct dirent temp;
	temp.d_name[0] = 'h';
	temp.d_name[1] = 'h';
	temp.d_name[2] = '\0';

	struct stat fileStat;
	if (stat("./a.out", &fileStat) < 0)
	{
		perror("temp");
	}
	printf("Name: %s\n", "../Makefile");
	printf("Size: %ld bytes\n", fileStat.st_size);
	printf("Last modified: %s", ctime(&fileStat.st_mtime));
	printf("Chmod: %x\n", fileStat.st_mode);
	printf("print: %o\n", (S_IRUSR | S_IWUSR | S_IXUSR) & fileStat.st_mode);
	// Close the directory
	closedir(dir);

	return 0;
}