#include<iostream>
#include <fcntl.h>
#include <fstream>
#include <string>
#include "history.h"
using namespace std;
string history_file="maintain_history.txt";
// int no_of_lines;
int num_lines()
{
    ifstream check_lines;
    string buff;
    int cnt=0;
    check_lines.open(history_file);
    while(getline(check_lines,buff))
    {
        cnt++;
    }
    check_lines.close();
    return cnt;
}
void read_shell_history(int num)
{
    // cout<<num;
    ifstream file(history_file);
    string temp;
    deque<string> array;
    vector<string> display;
    int cnt=0;
    if(num>20)
    {
        cout<<"Number more than 20\n";
        return;
    }
    else if(num<0)
    {
        cout<<"Negative number given\n";
        return;
    }

    while(getline(file,temp))
    {
        array.push_back(temp);
    }
    if(array.empty())
    {
        cout<<"No history\n";
        return;
    }

    while(cnt<num && !array.empty())
    {
        display.push_back(array.back());
        array.pop_back();
        cnt++;
    }
    reverse(display.begin(),display.end());
    for(int i=0;i<display.size();i++)
        cout<<display[i]<<'\n';
    file.close();
}
void write_shell_history(string command)
{
    ofstream file;
    int no_lines=num_lines();
    // cout<<command;
    if(no_lines<20)
    {
        file.open(history_file,ios::app);
        if(!file) 
        {
            cout<<"Error opening history file"<<endl;
            return;
        }
        file<<command<<'\n';
        file.close();
    }
    else
    {
        ifstream file(history_file);
        string temp;
        queue<string> cmd;
        while(getline(file,temp))
        {
            // cout<<temp;
            cmd.push(temp);
        }
        cmd.pop();
        cmd.push(command);
        file.close();

        ofstream file_op(history_file,ios::trunc);
        while(!cmd.empty())
        {
            // cout<<cmd.front()<<'\n';
            file_op<<cmd.front()<<'\n';
            cmd.pop();
        }
        file_op.close();
    }
}