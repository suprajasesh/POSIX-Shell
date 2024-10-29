#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include "cd.h"
#include "ls.h"
#include "pinfo.h"
#include "history.h"
#include "ioredirect.h"
#include "mypipe.h"
#include "others.h"

#define SIGCUSTOM SIGUSR1
using namespace std;

string terminal_prompt;
vector<pid_t>bg_pids;
bool append;
pid_t child_pid=-1;
termios term;
string fetch_prompt()
{
    char host_name[1024];
    char user_name[1024];
    int res=gethostname(host_name, 1024);
    if (res)
    {
        cout<<"Error fetching hostname"<<endl;
        exit;
    }
    // cout<<host_name;
    res=getlogin_r(user_name,1024);
    if (res)
    {
        cout<<"Error fetching username"<<endl;
        exit;
    }
    string prompt=user_name;
    prompt+='@';
    prompt+=host_name;
    prompt+=':';
    dir_to_str();
    prompt+=curr_dir;
    return prompt;
}

void other_commands(vector<string> cmds,bool background)
{    
    char* args[cmds.size()+1];
    args[cmds.size()]=NULL;
    for(int i=0;i<cmds.size();i++)
    {
        args[i]=new char[cmds[i].size()+1];
        strcpy(args[i],cmds[i].c_str());
    }
    pid_t pid=fork();
    // child_pid=pid;
    if(pid==0)
    {
        setpgid(0,0);
        int status=execvp(cmds[0].c_str(),args);
        perror(strerror(errno));
        _exit(EXIT_FAILURE);
    }
    else if(pid>0)
    {
        sleep(1);
        if(background)
        {
            cout<<pid<<endl;
            bg_pids.push_back(pid);
        }
        else
        {
            child_pid=pid;
            int st;
            // while(wait(&st)!=pid);
            waitpid(pid,&st,WUNTRACED);
            if (WIFSTOPPED(st)==0) 
            {
                int exit_status = WEXITSTATUS(st);
                if(exit_status)
                {
                    cout<<"Exit Status:"<<exit_status<<endl;
                }
            } 
            else if(WIFSIGNALED(st))
            {
                cout<<WTERMSIG(st);
            }
            else 
            {
                cout<<"Not terminated"<<endl;
            }
            child_pid=-1;
        }
        // kill(pid,SIGTERM);
    }
    for (size_t i=0;i<cmds.size();i++) 
    {
        delete[] args[i];
    }
}

void signal_handler(int signal)
{
    // cout<<signal;
    if(signal==SIGINT)
    {
        // cout<<"Ctrl+C"<<endl;
        if(child_pid>0)
        {
            kill(child_pid,SIGINT);
            return;
        }
        else if(child_pid==-1)
        {
            // cout<<"No foreground process to interrupt\n";
            return;
        }
    }
    else if(signal==SIGTSTP)
    {
        // cout<<"Ctrl+Z"<<endl;
        // cout<<child_pid;
        if(child_pid>0)
        {
            kill(child_pid,SIGTSTP);
            return;
            // exit(0);
        }
        else if(child_pid==-1)
        {
            // cout<<"No foreground process to suspend\n";
            return;
        }
    }
    else if(signal==SIGCUSTOM)
    {
        cout<<"Ctrl+D"<<endl;
        tcsetattr(STDIN_FILENO,TCSAFLUSH,&term);
        exit(0); 
    }
}

int main()
{
    char buf[1024];
    char overall_delim[]=";";
    char within_delim[]=" \t";
    home_dir=getcwd(buf,1024);
    vector<string> cmds,overall_cmds;
    bool background=false;    

    signal(SIGINT,signal_handler);
    signal(SIGTSTP,signal_handler);
    signal(SIGCUSTOM,signal_handler);


    struct termios raw;
    tcgetattr(STDIN_FILENO,&term); 
    raw=term;

    raw.c_iflag&=~(BRKINT | INPCK | ISTRIP | IXON); 
    raw.c_cflag|=CS8; 
    raw.c_lflag&=~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);
    // setvbuf(stdout, (char *)NULL, _IONBF, 0); 

    while(1)
    {     
        terminal_prompt=fetch_prompt();
        cout<<terminal_prompt<<"~> ";
        string command;
        // getline(cin,command);

        char ch;
        while(true)
        {
            ch=getchar();
            if (ch=='\n')
            {
                cout<<endl;
                break;  
            }
            else if(ch==4)
            {
                kill(getpid(),SIGCUSTOM);
            }
            else if(ch==3)
            {
                cout<<endl;
                break;  
                // continue;
            }
            else if(ch==26)
            {
                cout<<endl;
                break;  
            }
            else if (ch=='\t') 
            {
                cout<<"\t";
                if(autocomplete(command))
                    continue;
                command+="\t";
            } 
            else if (ch=='\b'||ch==127) 
            {
                if(!command.empty()) 
                {
                    command.pop_back();
                    cout<<"\b \b"<<flush;
                }
            }
            else 
            {
                command+=ch;
                cout<<ch<<flush;
            }
            // cout<<ch<<"==";
        }

        // cout<<"Cmd:"<<command<<endl;

        write_shell_history(command);
        vector<vector<string>> all_cmds;            //tokenize and store all commands and subcommands
        vector<string> for_echo;                    //to print for echo store as strings itself;
        if(command.find(";")!=string::npos)
        {
            overall_cmds=get_command(command,overall_delim);        //separate commands
            string temp;
            for(int i=0;i<overall_cmds.size();i++)
            {
                cmds=get_command(overall_cmds[i],within_delim);         //separate within commands
                all_cmds.push_back(cmds);
                for_echo.push_back(overall_cmds[i]);                    //directly add string for echo commands;
            }
        }
        // cout<<"hi";


        if(all_cmds.empty())
        {
            // cout<<"all dir 0";
            cmds=get_command(command,within_delim); 
            background=check_bg(cmds);
            
            if(command.find("|")==string::npos && (command.find("<")!=string::npos || command.find(">")!=string::npos))
            {
                // cout<<"Calling redirect alone";
                vector<string> ret=check_redirect(cmds); 
                if(ret[4]=="true")
                {
                    if(ret[3]=="true")
                        redirection(ret[0],ret[1],ret[2],true);
                    else
                        redirection(ret[0],ret[1],ret[2],false);
                    // return 0;
                    continue;
                }
            }
            // cout<<"Ret 4:"<<ret[4]<<'\n';
            
            else if(command.find("|")!=string::npos &&!(command.find("<")!=string::npos || command.find(">")!=string::npos))
            {
                // cout<<"calling pipe alone";
                check_pipe(command);
                continue;
            }

            else if(command.find("|")!=string::npos && (command.find("<")!=string::npos || command.find(">")!=string::npos))
            {
                // cout<<"Calling redirect_and pipe";
                redirect_and_pipe(command);
                continue;
            }

            else if(cmds.size()==0)
                continue;
            else if(cmds[0].find("cd")==0)
            {
                change_dir(cmds);
            }
            else if(cmds[0]=="pwd")
            {
                pwd();
            }
            else if(cmds[0]=="echo")
            {
                echo(command);
            }
            else if(cmds[0]=="ls")
            {
                ls_fun(cmds);
            }
            else if(cmds[0]=="pinfo")
            {
                pinfo_fun(cmds);
            }
            else if(cmds[0]=="search")
            {
                if(cmds.size()==1)
                {    perror("No directory given");
                    continue;
                }
                else if(cmds.size()>2)
                {
                    cout<<"More than one directory given\n";
                    continue;
                }
                else
                    cout<<boolalpha<<search(home_dir+curr_dir,cmds[1])<<endl;
            }
            else if(cmds[0]=="history")
            {
                // cout<<"his";
                if(cmds.size()==1)
                    read_shell_history(10);
                else if(cmds.size()==2)
                    read_shell_history(stoi(cmds[1]));
                else    
                    {
                        perror("Too many arguments");
                        return 0;
                    }
            }
            else if(cmds[0]=="exit")
            {
                tcsetattr(STDIN_FILENO,TCSAFLUSH,&term);
                exit(0);
            }
            else
            {
                // cout<<"bruh";
                other_commands(cmds,background);
            }
        }
        else
        {
            for(int i=0;i<all_cmds.size();i++)                      //for each command perform the corresponding operation
            {
                vector<string> each_cmd;
                each_cmd=all_cmds[i];
                // cout<<"\nSize:"<<each_cmd.size()<<'\n';
                background=check_bg(each_cmd);


                string pipe_str;
                for(string s: each_cmd)
                {
                    pipe_str+=s+' ';
                }
                // cout<<pipe_str;

                if(pipe_str.find("|")==string::npos && (pipe_str.find("<")!=string::npos || pipe_str.find(">")!=string::npos))
                {
                    // cout<<"Calling redirect alone";
                    vector<string> ret=check_redirect(each_cmd); 
                    if(ret[4]=="true")
                    {
                        if(ret[3]=="true")
                            redirection(ret[0],ret[1],ret[2],true);
                        else
                            redirection(ret[0],ret[1],ret[2],false);
                        // return 0;
                        continue;
                    }
                }
                // cout<<"Ret 4:"<<ret[4]<<'\n';
                
                else if(pipe_str.find("|")!=string::npos &&!(pipe_str.find("<")!=string::npos || pipe_str.find(">")!=string::npos))
                {
                    // cout<<"calling pipe alone";
                    check_pipe(pipe_str);
                    continue;
                }

                else if(pipe_str.find("|")!=string::npos && (pipe_str.find("<")!=string::npos || pipe_str.find(">")!=string::npos))
                {
                    // cout<<"Calling redirect_and pipe";
                    redirect_and_pipe(pipe_str);
                    continue;
                }
                else if(each_cmd.size()==0)
                    continue;
                else if(each_cmd[0].find("cd")==0)
                {
                    // cout<<"cd;";
                    change_dir(each_cmd);
                }
                else if(each_cmd[0]=="pwd")
                {
                    pwd();
                }
                else if(each_cmd[0]=="echo")
                {
                    echo(for_echo[i]);
                }
                else if(each_cmd[0]=="ls")
                {
                    ls_fun(each_cmd);
                }
                else if(each_cmd[0]=="pinfo")
                {
                    pinfo_fun(each_cmd);
                }
                else if(each_cmd[0]=="search")
                {
                    if(each_cmd.size()==1)
                    {    perror("No directory given");
                        continue;
                    }
                    else if(cmds.size()>2)
                    {
                        cout<<"More than one directory given\n";
                        continue;
                    }
                    else
                        cout<<search(home_dir+curr_dir,cmds[1])<<endl;
                }
                else if(each_cmd[0]=="history")
                {
                    if(cmds.size()==1)
                        read_shell_history(10);
                    else if(cmds.size()==2)
                        read_shell_history(stoi(cmds[1]));
                    else    
                        {
                            perror("Too many arguments");
                            return 0;
                        }
                }
                else if(cmds[0]=="exit")
                {
                    tcsetattr(STDIN_FILENO,TCSAFLUSH,&term);
                    exit(0);
                }
                else
                {
                    other_commands(each_cmd,background);
                }
            }
        }
        
    }
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&term);
    // getchar();
    return 0;
}