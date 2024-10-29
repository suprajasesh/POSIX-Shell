#include<iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include "mypipe.h"
#include "cd.h"
#include "ioredirect.h"
#include "others.h"
using namespace std;

void check_pipe(string command)
{
    char*cmds[100];
    char* args[100];
    int num_cmds=0;
    
    char* tkn=strtok((char*)command.c_str(),"|");
    while(tkn!=NULL) 
    {
        cmds[num_cmds]=tkn;
        tkn=strtok(NULL,"|");
        num_cmds++;
    }
    cmds[num_cmds]=NULL;
    // for(int i=0;i<num_cmds;i++)
    //     cout<<cmds[i]<<"[[]]";
    int pipeline[2];
    int input_fd=STDIN_FILENO; 

    for(int i=0;i<num_cmds;i++)
    {
        if(pipe(pipeline)==-1) 
        {
            perror("Error creating pipe");
            exit(EXIT_FAILURE);
        }

        int pid=fork();
        if(pid==-1)
        {
            perror("Error creating fork");
            exit(EXIT_FAILURE);
        }
        else if(pid==0) 
        {
            dup2(input_fd,STDIN_FILENO);       
            if(i<num_cmds-1) 
            {
                dup2(pipeline[1],STDOUT_FILENO);
            }
            close(pipeline[0]);             

            char* cmd=cmds[i];
            int j=0;
            args[j]=strtok(cmd," \t");
            while (args[j]!=NULL)
            {
                j++;
                args[j]=strtok(NULL," \t");
            }

            execvp(args[0],args); 
            perror("Error in execvp"); 
            exit(EXIT_FAILURE);
        } 
        else 
        {
            // sleep(1);
            waitpid(pid,NULL,0);
            close(pipeline[1]); 
            input_fd=pipeline[0];
        }
    }
}


void redirect_and_pipe(string command)
{
    int pipeline[2];
    int num_cmds=0;
    char* cmds[100];
    char* tkn=strtok((char*)command.c_str(),"|");
    while(tkn!=NULL) 
    {
        cmds[num_cmds]=tkn;
        tkn=strtok(NULL,"|");
        num_cmds++;
    }
    cmds[num_cmds]=NULL;
    int input_fd=STDIN_FILENO;
    for(int i=0;i<num_cmds;i++)
    {
        // cout<<cmds[i]<<"\n";
        if(pipe(pipeline)==-1)
        {
            perror("Error creating pipe\n");
            exit(EXIT_FAILURE);
        }
        int pid=fork();
        if(pid==-1)
        {
            perror("Error creating fork\n");
            exit(EXIT_FAILURE);
        }
        else if(pid==0) 
        {
            
            int j=0; 
            
            dup2(input_fd,STDIN_FILENO);

            if(i<num_cmds-1)
            {
                dup2(pipeline[1],STDOUT_FILENO);
                close(pipeline[1]);
            }
            close(pipeline[0]);
            char delim[]=" \t";
            vector<string> args=get_command(cmds[i],delim);
            // for(int i=0;i<args.size();i++)
            //     cout<<args[i]<<"\\";

            vector<string> ret=check_redirect(args); 
            // cout<<"Ret 4:"<<ret[4];
            if(ret[4]=="true")
            {
                // cout<<"Ret3"<<ret[3];
                if(ret[3]=="true")
                    redirection(ret[0],ret[1],ret[2],true);
                else
                    redirection(ret[0],ret[1],ret[2],false);
                // return 0;
                // continue;
                exit(0);
            }
            char* chr_args[100];
            for(int i=0;i<args.size();i++)
                chr_args[i]=strdup(args[i].c_str());
            chr_args[args.size()]=NULL;
            execvp(args[0].c_str(),chr_args);
        } 
        else 
        {
            // sleep(1);
            if(i>0)
            {
                close(input_fd);
            }
            if(i<num_cmds-1)
            {
                close(pipeline[1]);
                input_fd=pipeline[0];
            }
            waitpid(pid,NULL,0);
        }
    }
}