#include <iostream>
#include <fcntl.h>
#include <sys/types.h> 
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/wait.h> 
#include "ioredirect.h"
using namespace std;

vector<string> check_redirect(vector<string> cmds)
{
    string command;
    string input_file,output_file;
    bool app=false;
    bool redirect=false;
    vector<string> ret;

    for(int i=0;i<cmds.size();) 
    {
        // int pos;
        if(cmds[i]==">") 
        {
            // pos=find(cmds.begin(),cmds.end(),">>")-cmds.begin();
            if(i<cmds.size()-1)
            {
                output_file=cmds[i+1];
                app=false;
                redirect=true;
                cmds.erase(cmds.begin()+i,cmds.begin()+i+2);
            }
            continue;
        } 
        else if(cmds[i]==">>") 
        {
            // pos=find(cmds.begin(),cmds.end(),">>")-cmds.begin();
            if (i<cmds.size()-1) 
            {
                output_file=cmds[i+1];
                app=true;
                cmds.erase(cmds.begin()+i,cmds.begin()+i+2);
                redirect=true;
            }
            continue;
        } 
        else if(cmds[i]=="<") 
        {
            // pos=find(cmds.begin(),cmds.end(),"<")-cmds.begin();
            if (i<cmds.size()-1)
            {
                input_file=cmds[i+1];
                cmds.erase(cmds.begin()+i,cmds.begin()+i+2);
            }
            redirect=true;
            continue;
        } 
        else 
        {
            if(command.empty())
                command=cmds[i];
            else
                command+=" "+cmds[i];
        }
        i++;
    }
    ret.push_back(command);
    ret.push_back(input_file);
    ret.push_back(output_file);
    if(app)
        ret.push_back("true");
    else    
        ret.push_back("false");
    if(redirect)
        ret.push_back("true");
    else    
        ret.push_back("false");
    return ret;
}

void redirection(string command, string input_file, string output_file, bool app) {
    int pid=fork();
    if(pid<0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } 
    else if(pid==0) 
    {
        if(!input_file.empty())
        {
            int fd=open(input_file.c_str(),O_RDONLY);       //read from file
            if (fd<0) 
            {
                if(errno==ENONET)
                {
                    perror("Input File does not exist");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    perror("Error opening input file");
                    exit(EXIT_FAILURE);
                }
            }
            dup2(fd,STDIN_FILENO);                          //duplicate from stdin into file
            close(fd);
        }
        if(!output_file.empty()) 
        {
            int flag;
            if(app)
            {
                flag=O_APPEND|O_WRONLY|O_CREAT;         //set flags if you want to append
            }
            else
            {
                flag=O_TRUNC|O_WRONLY|O_CREAT;          //don't set O_APPEND flag 
            }

            int fd=open(output_file.c_str(),flag,0644);     //create output file with permissions
            if(fd<0) 
            {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd,STDOUT_FILENO);                     //dupliacte the std output into file 
            close(fd);
        }

        vector<char*>args;
        int beg=0,end;
        while(command.find(' ', beg)!=string::npos)     // find next space
        {
            end=command.find(' ',beg);                  // mark it as end of command
            string temp=command.substr(beg,end-beg);    // get that substring
            args.push_back(strdup(temp.c_str()));       // duplicate and convert it to char*
            beg=end+1;                                  // increment to find next space
        }
        args.push_back(strdup(command.substr(beg).c_str()));    // push last command
        args.push_back(NULL);    

        execvp(args[0],args.data());                   //execute
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } 
    else 
    {
        int status;
        waitpid(pid,&status,0);
    }
}
