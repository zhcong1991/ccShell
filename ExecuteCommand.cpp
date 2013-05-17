#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#include "ExecuteCommandDef.h"

using namespace std;

extern _COMMAND command_list[];

int cd_command(char * command, char ** arg)
{
    char * current_dir = (char *)malloc(1024);

    memset(current_dir, '\0', 1024);

    if(NULL == arg[1] || !strcmp(arg[1], "~"))
    {
        arg[1] = get_userhome();
    }
    if(!strcmp(arg[1], "-"))
    {
        strcpy(current_dir, last_dir);
        arg[1] = current_dir;
    }

    memset(last_dir, '\0', 1024);
    getcwd(last_dir,1024);

    if(chdir(arg[1]))
    {
        cout<<"ccShell: cd "<<arg[0]<<": No such file or directory!"<<endl;
        return -1;
    }

    return 0;
}

int pwd_command(char * command, char ** arg)
{
	char * path = (char *) malloc(1024);

	memset(path, '\0', 1024);

	if(getcwd(path,1024) == NULL)
	{
        cout<<"ccShell: pwd :Get path failed!"<<endl;
        return -1;
	}
	else
	{
        cout<<path<<endl;
        return 0;
	}
}

int echo_command(char * command, char ** arg)
{
    int i = 1;
    while(arg[i])
    {
        cout<<arg[i];
        i++;
    }
    cout<<endl;
    return 0;
}

int unset_command(char * command, char ** arg)
{
    int i = 0;

    for(; i < variable_count; i++)
    {
        if(0 == strcmp(variable[i].variable_name, arg[1]))
        {
            delete variable[i].variable_name;
            delete variable[i].variable_value;
            variable[i].variable_name = NULL;
            variable[i].variable_value = NULL;
            variable_count--;
            return 0;
        }
    }
    return 0;
}

int help_command(char * command, char ** arg)
{
    int i = 0;
    cout<<"----------------------The Builtin Command Help Information----------------------"<<endl;
    cout<<"   Command                             Document    "<<endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
    while(command_list[i].commandName)
    {
        cout<<"     "<<command_list[i].commandName<<"                    "<<command_list[i].commandDoc<<"     "<<endl;
        i++;
    }
    return 0;
}

int history_command(char * command, char ** arg)
{
    int i = 0;
    HIST_ENTRY** historylist = NULL;

    historylist = history_list();
    if(NULL != historylist)
    {
        i = 0;
        while(historylist[i])
        {
            cout<<i<<": "<<historylist[i]->line<<endl;
            i++;
        }
    }
    return 0;
}

bool assignment(char ** arg)
{
    int i = 0, j = 0;

    while(arg[i])
    {
        if((0 == strcmp(arg[i], "=")) && arg[i-1] && arg[i+1])
        {
            for(j = 0; j < variable_count; j++)
            {
                if(0 == strcmp(variable[j].variable_name, arg[i-1]))
                {
                    delete variable[j].variable_value;
                    variable[j].variable_value = (char *)malloc(sizeof(arg[i+1]));
                    strcpy(variable[j].variable_value, arg[i+1]);
                    return 1;
                }

            }
            if(variable_count < MAX_VAR_NUM)
            {
                variable[variable_count].variable_name = (char *)malloc(sizeof(arg[i-1]));
                variable[variable_count].variable_value = (char *)malloc(sizeof(arg[i+1]));
                strcpy(variable[variable_count].variable_name, arg[i-1]);
                strcpy(variable[variable_count].variable_value, arg[i+1]);
                variable_count++;
            }
            else
            {
                cout<<"ccShell :Too many variable,please reset some variable with unset command!"<<endl;
            }
            return 1;
        }
        i++;
    }
    return 0;
}

char * get_variable_value(char * variable_name)
{
    int i = 0;

    for(i = 0; i < variable_count; i++)
    {
        if(0 == strcmp(variable[i].variable_name, variable_name))
        {
            return variable[i].variable_value;
        }
        else
        {
            return NULL;
        }
    }
    return NULL;
}

bool search_command_file_path(const char * command_name, char * command_file_path)
{
	int i = 0;
	const char* position = NULL;

	position = getenv("PATH");
	while(*position != 0)
	{
		if(*position != ':')
		{
			command_file_path[i++] = *position;
		}
		else
		{
			command_file_path[i++] = '/';
			command_file_path[i] = '\0';
			strcat(command_file_path, command_name);
			if(access(command_file_path, F_OK) == 0)
			{
				return true;
			}
			i = 0;
		}
		position++;
	}
	return false;
}
