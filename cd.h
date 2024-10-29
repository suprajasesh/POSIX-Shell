#ifndef cd
#define cd

#include <bits/stdc++.h>
using namespace std;
extern string home_dir;
extern string curr_dir;
extern string prev_dir;

bool valid_dir(string);
void dir_to_str();
void pwd();
void echo(string);
void change_dir(vector<string>);
inline void ltrim(string&);
#endif
