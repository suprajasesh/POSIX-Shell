#include <iostream>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include "cd.h"
using namespace std;

void ls_long(string,bool);
void normal_ls(string dir_name, bool full, bool append)
{
    if(full)
    {
        ls_long(dir_name,append);
        return;
    }
    struct dirent* each_record;
    DIR* dir=opendir(dir_name.c_str());
    while(each_record=readdir(dir))
    {
        if(each_record->d_name[0]=='.' & !append)
            continue;
        cout<<each_record->d_name<<"\n";
    }
}


void ls_l_record(string dir_name,struct dirent* record)
{
    string rec_name=record->d_name;
    // cout<<dir_name+'/'+rec_name<<"\n";
    struct stat dir_fp;
    if(stat((dir_name+'/'+rec_name).c_str(),&dir_fp)!=0)
    {
        cout<<strerror(errno);
        cout<<"Error in fetching record details\n";
        return;
    }
    printf(dir_fp.st_mode & S_IFDIR ? "d" : "-");
    printf(dir_fp.st_mode & S_IRUSR ? "r" : "-");
    printf(dir_fp.st_mode & S_IWUSR ? "w" : "-");
    printf(dir_fp.st_mode & S_IXUSR ? "x" : "-");

    printf(dir_fp.st_mode & S_IRGRP ? "r" : "-");
    printf(dir_fp.st_mode & S_IWGRP ? "w" : "-");
    printf(dir_fp.st_mode & S_IXGRP ? "x" : "-");

    printf(dir_fp.st_mode & S_IROTH ? "r" : "-");
    printf(dir_fp.st_mode & S_IWOTH ? "w" : "-");
    printf(dir_fp.st_mode & S_IXOTH ? "x  " : "-  ");

    printf("%ld  ",dir_fp.st_nlink);

    struct passwd* owner=getpwuid(dir_fp.st_uid);
    struct group* grp=getgrgid(dir_fp.st_gid);
    printf("%s  ",owner->pw_name);
    printf("%s  ",grp->gr_name);

    printf("%5ld  ",dir_fp.st_size);
    // time;
    char buffer[1024];
    struct tm *time=localtime(&dir_fp.st_mtime);
    strftime(buffer,1024,"%b %2d %H:%M", time);
    printf("%s  ",buffer);
    // printf("%ld  ",dir_fp.st_mtime);
    printf("%s\n",rec_name.c_str());
}

void ls_long(string dir_name, bool app)
{
    // string dir_name=home_dir+curr_dir;
    struct dirent* each_record;
    DIR* dir=opendir(dir_name.c_str());
    while(each_record=readdir(dir))
    {
        if(each_record->d_name[0]=='.' && !app)
            continue;
        ls_l_record(dir_name,each_record);
    }
}

void dir_handling(string dir_name,bool full, bool append)
{
    if(dir_name.empty())
    {
        normal_ls(home_dir+curr_dir,full,append);
        // cout<<"ls ";
    }
    else if(dir_name==".")
    {
        normal_ls(home_dir+curr_dir,full,append);
        // cout<<"ls .";
        return;
    }
    else if(dir_name=="..")
    {
        if(curr_dir.empty())
        {
            cout<<"Root directory cannot go to previous one\n";
            return;
        }
        string full_path=home_dir+curr_dir;
        size_t pos= full_path.find_last_of('/');
        if(pos==string::npos)
        {
            return;
        }
        string p_dir=full_path.substr(0,pos);
        normal_ls(p_dir,full,append);
        // cout<<"ls ..";
        return;   
    }
    else if(dir_name=="~")
    {
        normal_ls(home_dir,full,append);
        // cout<<"ls ~";
    }
    else if(!dir_name.empty())
    {
        // cout<<"ls dir?";
        if(valid_dir(home_dir+curr_dir+'/'+dir_name))
            normal_ls(home_dir+curr_dir+'/'+dir_name,full,append);
        else    
            cout<<"Directory does not exist\n";
    }
}

void ls_fun(vector<string>cmds)
{
    if(cmds.size()==1 && cmds[0]=="ls")             //ls
    {
        normal_ls(home_dir+curr_dir,false,false);
        cout<<"ls\n";
        return;
    }
    string dir_name="";
    bool append=false,full=false;
    int cnt=0;
    vector<string> all_dirs;
    for(auto cmd:cmds)
    {
        if(cmd=="ls")
            continue;
        else if(cmd=="-a")
            append=true;
        else if(cmd=="-l")
            full=true;
        else if(cmd=="-la" || cmd=="-al")
        {
            full=true;
            append=true;
        }
        else
            all_dirs.push_back(cmd);
    }
    if(all_dirs.empty())
        all_dirs.push_back("");
    for(int i=0;i<all_dirs.size();i++)
    {
        if(all_dirs.size()>1)
            cout<<all_dirs[i]<<":\n";
        dir_handling(all_dirs[i],full,append);
    }    
}
