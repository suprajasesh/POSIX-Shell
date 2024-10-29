#include <iostream>
#include <string.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>
#include "cd.h"
using namespace std;
vector<string> get_command(string command,char* delim)
{
    vector<string> cmd;
    if(command.empty())
        return cmd;
    char* tkn=strtok((char*) command.c_str(),delim);
    while(tkn)
    {  
        cmd.push_back(tkn);
        
        tkn=strtok(NULL,delim);
    }
    return cmd;
}

bool check_bg(vector<string>& cmds)
{
    for(int i=0;i<cmds.size();i++)
    {
        if(cmds[i]=="&")
        {
            cmds.erase(cmds.begin()+i);
            return true;
        }
        else if(cmds[i].find("&")!=string::npos)
        {
            int pos=cmds[i].find("&");
            cmds[i].erase(cmds[i].begin()+pos);
            return true;
        }
    }
    return false;
}

bool autocomplete(string command)
{
    struct dirent* each_record;
    string path=home_dir+'/'+curr_dir;
    DIR* dir=opendir(path.c_str());
    int fl=1;
    while(each_record=readdir(dir))
    {
        string rec_name=each_record->d_name;
        if(rec_name.find(command)==0)
        {
            cout<<rec_name<<" ";
            fl=0;
        }
    }
    closedir(dir);
    if(fl==0)
        return true;
    return false;
}