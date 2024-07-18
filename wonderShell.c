#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <limits.h>

//#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>


#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"



char deviceName[100], currentWD[PATH_MAX];
char *userName;
char args[PATH_MAX];

int childpid;
char *childProcName;

time_t timSt;
double waitFor;
char msg[500];
int timeVLD = 0;
int loopflg = 1;
int parsePath(char* list[50],int len)
{
	int i;
	for(i=1;i<len;i++)
	{
		if(list[i][0]!='-' && list[i][0]!='&')
		{
			return i;
		}
	}
	return -1;
}
int chkFlgOrPath(char *list[50], int len)
{
	if (len == 1)
		return 0;
	int ret = 0;
	int i;
	for (i = 1; i < len; i++)
	{
		if (strcmp(list[i], "-l") == 0)
			ret += 2;
		else if (strcmp(list[i], "-a") == 0)
			ret += 1;
		else if (strcmp(list[i], "-la") == 0)
			return 3;
	}
	return ret;
}

char *replaceWord(const char *s, const char *oldW, const char *newW)
{
	char *result;
	int i, cnt = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);

	for (i = 0; s[i] != '\0'; i++)
	{
		if (strstr(&s[i], oldW) == &s[i])
		{
			cnt++;
			i += oldWlen - 1;
		}
	}
	result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);

	i = 0;
	while (*s)
	{
		if (strstr(s, oldW) == s)
		{
			strcpy(&result[i], newW);
			i += newWlen;
			s += oldWlen;
		}
		else
			result[i++] = *s++;
	}

	result[i] = '\0';
	return result;
}


extern void checkChild()
{
	int st;
	int *status;
	pid_t a;
	status = &st;
	a = waitpid(-1, status, WNOHANG);
	if (a > 0)
	{
		fprintf(stderr, "pid %d exicted Normally\n", a);
	}
}

void chkAlarm()
{
    if (timeVLD == 0)
    {
        return;
    }
    else if(timeVLD == 1)
    {
        time_t timEd = time(NULL);
        double diff = difftime(timEd, timSt);
        if (diff >= waitFor)
        {
            timeVLD = 0;
            (msg, "");
        }
        else if(diff < 0)
        {
            printf("Reaminder failed , printing the message\n");
            timeVLD = 0;
            strcpy(msg, "");
        }
    }
    return;
}

void execute(char *list[50], int len)
{
    checkChild();
    if (len == 0)
    {
        return;
    }
    if (strcmp("clear", list[0]) == 0)
    {
         printf("\033[H\033[J");
    }

    else if (strcmp("ls", list[0]) == 0)
	{
		struct dirent *de;
		DIR *dr = NULL;
		int flg = 0;
		for (int i = 1; i < len; i++)
		{
			if (list[i][0] != '-' && list[i][0] != '&')
			{
				flg = 1;
				dr = opendir(list[i]);
				break;
			}
		}
		if (flg == 0)
			dr = opendir(".");

		char *file[5000];
		if (dr == NULL)
		{
			printf("Could not open file directory");
			return;
		}
		int noFile = 0;
		char *space = "  ";
		long int blksize = 0;
		int infoLvl = chkFlgOrPath(list, len);
		while ((de = readdir(dr)) != NULL)
		{
			file[noFile++] = de->d_name;
			if (infoLvl == 0)
			{
				if (strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0)
					printf("%s%s", file[noFile - 1], space);
			}
			else if (infoLvl == 1)
				printf("%s%s", file[noFile - 1], space);

			else if ((infoLvl == 2 && strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0) || infoLvl == 3)
			{
				struct stat fileStat;
				struct passwd *pwd;
				char date[12];
				int addrsI = parsePath(list, len);
				if (addrsI > 0)
				{
					char path1[100];
					sprintf(path1, "%s%s", list[addrsI], file[noFile - 1]);
					if (stat(path1, &fileStat) < 0)
						return;
				}
				else
				{
					if (stat(file[noFile - 1], &fileStat) < 0)
						return;
				}

				printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
				printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
				printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
				printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
				printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
				printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
				printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
				printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
				printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
				printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
				printf(" %ld ", fileStat.st_nlink);

				if ((pwd = getpwuid(fileStat.st_uid)) != NULL)
					printf(" %-8.8s", pwd->pw_name);
				else
					printf(" %-8d", fileStat.st_uid);

				if ((pwd = getpwuid(fileStat.st_gid)) != NULL)
					printf(" %-8.8s", pwd->pw_name);
				else
					printf(" %-8d", fileStat.st_gid);

				printf(" %8ld ", fileStat.st_size);
				strftime(date, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
				printf(" %s ", date);
				blksize += fileStat.st_blocks;
				printf("%s\n", file[noFile - 1]);
			}
		}
		if (infoLvl >= 2)
		{
			blksize /= 2;
			printf("total: %ld\n", blksize);
			blksize = 0;
			printf("\n");
		}
		printf("\n");
		closedir(dr);
	}

    else if (strcmp("cd", list[0]) == 0)
    {
        if (len < 1)
        {
            printf("Enter the path after cd ,Usgae -> cd <pathname>\n");
        }
        char *replaceTilda = list[1];
        if (list[1][0] = '~')
        {
            replaceTilda = replaceWord(list[1], "~", strcat(currentWD,"/"));
        }
        if (chdir(replaceTilda) == -1)
        {
            printf("No such dir exists.\n");
            printf("%s ",replaceTilda);
        }
    }
    else if (strcmp("pwd", list[0]) == 0)
    {
        char pwd[PATH_MAX];
        getcwd(pwd, sizeof(pwd));
        printf("%s\n", pwd);
    }
    // else if (strcmp("remindme",list[0])==0)
    // {
    //     setAlarm(list,len);
    // }
    else if (strcmp("clock", list[0]) == 0)
    {
        // clkCmd(list, len);
    }
    else if (strcmp("echo", list[0]) == 0)
    {
        int i = 1;
        for (int i = 1; i < len; i++)
        {
            printf("%s ", list[i]);
        }
        printf("\n");
    }
    else{
        printf("Wrong command");
    }

    // else if (strcmp("ls", list[0]) == 0)
    {
        struct direct *de;
        DIR *dr = NULL;
        int flg = 0;
        for (int i = 1; i < len; i++)
        {
            if (list[i][0] != '-' && list[i][0] != '&')
            {
                flg=1;
                dr=opendir(list[1]);
                break;
            }
        }
        
    }
}

char *get_command()
{
    ssize_t bufferS = 0;
    char *userInput = NULL;
    if (getline(&userInput, &bufferS, stdin) == -1)
    {
        printf("Some error reading the line \n");
        return get_command();
    }
    return userInput;
}

void splitCommands(char str[PATH_MAX])
{
    char *d111 = " \n";
    char *d222 = ";";
    int looper1 = 0;
    char *Pointer1;
    char *Pointer2;
    char *token;
    char *token1;
    token1 = strtok_r(str, d222, &Pointer2);
    while (token1 != NULL)
    {
        char *argumentList[50];
        int looper = 0;
        token = strtok_r(token1, d111, &Pointer1);
        while (token != NULL)
        {
            argumentList[looper++] = token;
            token = strtok_r(NULL, d111, &Pointer1);
        }
        token1 = strtok_r(NULL, d222, &Pointer2);
        execute(argumentList, looper);
    }
}

int main()
{
    char shellsWD[200];
    getcwd(currentWD, sizeof(currentWD));

    gethostname(deviceName, sizeof(deviceName));

    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        userName = pw->pw_name;
    }
    userName = "";
    char *args;
    while (2 > 1)
    {
        getcwd(shellsWD, sizeof(shellsWD));
        char *cwd1 = shellsWD;
        if (strncmp(currentWD,shellsWD,strlen(currentWD)==0))
        {
            cwd1 = replaceWord(shellsWD, currentWD, "");
        }
        printf("<" YEL "%s@%s" RESET ":" BLU "~%s" RESET ">\n", userName, deviceName, cwd1);
        args=get_command();
        chkAlarm();
        if (strcmp("exit\n",args)==0)
        {
            printf("Thank you for using the shell");
            free(args);
			return 1;
        }
        splitCommands(args);
        free(args);
    }
}
