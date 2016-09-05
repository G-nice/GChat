#ifndef GUARD_USER_MANAGER_H
#define GUARD_USER_MANAGER_H

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>

#include "protocol.h"

#define USERDATA "userinfo.dat"


using std::string;
using std::map;
using std::vector;

//using std::;

typedef struct
{
    unsigned int userID; // userID 从 1 开始
    char userName[24];
    char password[20];
    unsigned int MsgID_record;    // groupchat
} userItem;

class userManager
{
public:
    // 默认构造函数  读取文件并初始化map
    userManager();

    // 析构函数  保存数据
    ~userManager();

    // 加载数据  重新载入用户数据
    void loaddata(char pathfilename[] = USERDATA);

    // 将用户数据记入文件
    void savedata(char pathfilename[] = USERDATA);

    // 用户注册
    bool register_user(const register_package& pak, int worker_fd);

    // 判断用户密码是否正确
    bool logincheck(const login_package& pak, int worker_fd);

    // 登出
    void logout(const logout_package& pak);

    // 用户修改密码
    bool modify_password(const modPwd_package& pak);

    // 返回用户群聊记录
    unsigned int latest_grp_MsgID(const backdata_package& pak);

    // 返回在线用户列表 online_user_list
    bool online_user(const backdata_package& pack, vector<string>& online_user_list);

    // 返回私聊用户的套接字
    int user_socketfd(const privateMsg_package& pack);


    void test()
    {
        string str = "nbjsulbjv shuar";
        char chas[32] = "nbjsulbjv shuar";
        if (str == chas)
            exit(EXIT_SUCCESS);
    }



private:
    unsigned int userAmount;  // userID 从 1 开始
    map<string, userItem> userTable;
    map<string, int>worker_user_pair;


};


#endif // GUARD_USER_MANAGER_H
