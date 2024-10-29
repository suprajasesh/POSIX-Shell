#include <iostream>
#include <unistd.h> 
#include "pinfo.h"
#include "cd.h"
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

bool check_fg_bg(pid_t pid)
{
    bool background;
    pid_t console_pid=tcgetpgrp(STDIN_FILENO);
    pid_t my_pid=getpgid(pid);
    if(console_pid==my_pid)
        background=false;
    else    
        background=true;
    return background;
}
void pinfo_fun(vector<string> cmd)
{
    bool background=false;
    pid_t pid;
    if(cmd.size()==1)
    {
        pid=getpid();
        // cout<<pid;
    }
    else if(cmd.size()==2)
    {   
        pid=stoi(cmd[1]);
    }
    if(kill(pid,0)==-1)
    {
        cout<<"Invalid PID\n";
        return;
    }
    background=check_fg_bg(pid);

    cout<<"PID -- "<<pid<<'\n';
    string proc_file="/proc/"+to_string(pid);
    proc_file+="/status";
    FILE* fp=fopen(proc_file.c_str(),"r");
    if(fp==NULL)
    {
        cout<<"Error opening file\n";
    }
    char buff[1024];
    while(fgets(buff,1024,fp))
    {
        char *line=buff;
        char *end=NULL;
        while(end=strchr(line,'\n'))
        {
            string line_str(line);
            // cout<<line_str;
            if(line_str.find("State")!=string::npos)
            {
                cout<<"Process Status -- ";
                if(!background)
                    cout<<"+";
                cout<<line_str.substr(line_str.find("State")+7);
            }
            else if(line_str.find("VmSize")!=string::npos)
            {
                cout<<"Memory -- "<<line_str.substr(line_str.find("VmSize")+10);
            }
            line=end+1;
        }
    }
    fclose(fp);
    proc_file=proc_file.substr(0,proc_file.find("/status"));
    proc_file+="/exe";
    fp=fopen(proc_file.c_str(),"r");
    if(fp==NULL)
    {
        cout<<"Error opening file\n";
    }
    int len=readlink(proc_file.c_str(),buff,1024);
    buff[len]='\0';
    // while()
    // {
    string str(buff);
    cout<<"Executable Path -- "<<str<<'\n';
}

bool search(string name, string dest_path)
{
    // string path=home_dir+curr_dir;
    bool found=false;
    struct dirent* each_record;
    DIR* dir=opendir(name.c_str());
    while(each_record=readdir(dir))
    {
        string rec_name=each_record->d_name;
        if(rec_name==dest_path)
            return true;
        if(each_record->d_type == DT_DIR && rec_name!="." && rec_name!="..")
        {
            // cout<<rec_name<<endl;
            if(search(name+'/'+rec_name,dest_path))
            {
                found=true;
                break;
            }
        }
    }
    closedir(dir);
    return found;
}