#include<iostream>
#include <unistd.h>
#include <string.h>
#include "cd.h"
#include <bits/stdc++.h>
#include <sys/stat.h>
using namespace std;
string home_dir;
string curr_dir;
string prev_dir;

void dir_to_str()
{
    char buf[1024];
    curr_dir.clear();
    string dir=getcwd(buf,1024);
    // cout<<dir<<"\n";
    for(int i=0;i<dir.size();i++)
        if(dir[i]==home_dir[i]);
        else
            curr_dir+=dir[i];
}

bool valid_dir(string dir)
{
    struct stat s;
    if(stat(dir.c_str(),&s)==0 && (s.st_mode & S_IFDIR))
        return true;
    return false;
}

void change_dir(vector<string>cmds)
{
    // cout<<prev_dir;
    if(cmds.size()==1 && cmds[0]=="cd")
    {
        chdir(home_dir.c_str());
        return;
    }
    else if(cmds.size()<2)
    {
        perror("Too few arguments\n");
        return;
    }
    else if(cmds.size()>2)
    {
        perror("many arguments\n"); 
        return;
    }
    if(cmds[1].compare(".")==0);    //stay in current directory
    else if(cmds[1].compare("..")==0)       // go back a directory
    {
        if(curr_dir.empty())                //if already in home dir print the path
            cout<<home_dir<<endl;
        else    
        {
            chdir("..");
        }
    }
    else if(cmds[1].compare("~")==0)
    {
        chdir(home_dir.c_str());            //go back to home directory
    }
    else if(cmds[1].compare("-")==0)        //switch to previous directory
    {
        // cout<<prev_dir;
        if(prev_dir.empty() && curr_dir.empty())
        {
            cout<<"No previous directory\n";
            return;
        }
        if(chdir((home_dir+'/'+prev_dir).c_str())!=0)
            cout<<"Error"<<endl;
        else
        {
            strerror(errno);
        }
    }
    else                                    //change dir
    {
        if(!valid_dir(cmds[1]))             //check if dir exists
        {
            // fflush(stdout);
            cout<<"Directory does not exist\n";
            return;
        }
        int err=chdir(cmds[1].c_str());     //else change dir
        if(err==-1)
        {
            strerror(errno);
        }
    }
    prev_dir=curr_dir;
}

void pwd()
{
    dir_to_str();
    if(home_dir.compare(home_dir+curr_dir)==0)
    {
        cout<<home_dir<<endl;
    }
    else
    {
        cout<<curr_dir<<endl;
    }
}

inline void ltrim(string &s) {
    s.erase(s.begin(),find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
}

void echo(string command)
{
    string content;
    content=command.substr(command.find("echo")+5);
    ltrim(content);
    if(content.empty())
    {
        printf("\n");
        return;
    }
    cout<<content<<endl;
}