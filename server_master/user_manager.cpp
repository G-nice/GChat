#include "user_manager.h"

#include <string>
//#include <algorithm>
//#include <ios>

using std::ifstream;
using std::ofstream;
using std::string;
using std::cout;
using std::endl;
using std::stoul;
using std::ios;
//using std::;
//using std::;
//using std::;

// 默认构造函数  读取文件并初始化map
userManager::userManager():userAmount(0)
{
//printf("~initialize\n");

    userTable.clear();
    worker_user_pair.clear();
    userAmount = 0;

    ifstream readdata;
    if ( access(USERDATA, F_OK) == 0)  // 文件存在  读取数据
    {
        readdata.open(USERDATA);
        if (!readdata.is_open())
        {
            std::cout << "open file " << USERDATA << " error" << std::endl;
            exit(EXIT_FAILURE);
        }

        string read_tmp;
        userItem item_temp;

//        readdata >> userAmount;  // 读取总人数
//        cout << userAmount << endl;

        int n = 1;
        while(getline(readdata, read_tmp))
        {
            if (read_tmp.empty())
                continue;

            switch (n % 4)
            {
            case 1:
//                cout << read_tmp << endl;
                item_temp.userID = stoul(read_tmp);  // C++ 11
                userAmount = (item_temp.userID > userAmount) ? item_temp.userID : userAmount;
//                cout << "CHECK: " << item_temp.userID << endl;
                break;
            case 2:
//                cout << read_tmp << endl;
                strncpy( item_temp.userName, read_tmp.c_str(), sizeof(item_temp.userName) );
//                cout << "CHECK: " << item_temp.userName << endl;
                break;
            case 3:
//                cout << read_tmp << endl;
                strncpy( item_temp.password, read_tmp.c_str(), sizeof(item_temp.password) );
//                cout << "CHECK: " << item_temp.password << endl;
                break;
            case 0:
//                cout << read_tmp << endl;
                item_temp.MsgID_record =  stoul(read_tmp);
//                cout << "CHECK: " << item_temp.MsgID_record << endl;
                userTable[string(item_temp.userName)] = item_temp;
//            cout << "userTable MAP.size " << userTable.size() << endl;

                break;
            }

            ++n;
        }
        readdata.close();
    }
    else   // 文件不存在  创建文件
    {
    FILE* fp = fopen(USERDATA, "w");
    fclose(fp);
    }
    cout << "User data initialize success." << endl;
}

// 加载数据  重新载入用户数据
void userManager::loaddata(char pathfilename[])
{
    userTable.clear();
    worker_user_pair.clear();
    userAmount = 0;

    ifstream readdata;
    if ( access(pathfilename, F_OK) == 0)  // 文件存在  读取数据
    {
        readdata.open(pathfilename);
        if (!readdata.is_open())
        {
            std::cout << "open file " << pathfilename << " error" << std::endl;
            exit(EXIT_FAILURE);
        }

        string read_tmp;
        userItem item_temp;

        int n = 1;
        while(getline(readdata, read_tmp))
        {
            if (read_tmp.empty())
                continue;

            switch (n % 4)
            {
            case 1:
                item_temp.userID = stoul(read_tmp);  // C++ 11
                break;
            case 2:
                strncpy( item_temp.userName, read_tmp.c_str(), sizeof(item_temp.userName) );
                break;
            case 3:
                strncpy( item_temp.password, read_tmp.c_str(), sizeof(item_temp.password) );
                break;
            case 0:
                item_temp.MsgID_record =  stoul(read_tmp);
                userTable[string(item_temp.userName)] = item_temp;
                break;
            }

            ++n;
        }
        userAmount = item_temp.userID;
        readdata.close();
    }
    else   // 文件不存在  创建文件
    {
    FILE* fp = fopen(pathfilename, "w");
    fclose(fp);
    }
    cout << "reload user data success." << endl;
    return;
}

// 将用户数据记入文件
void userManager::savedata(char pathfilename[])
{
    ofstream userdata;
    userdata.open(pathfilename, ios::out|ios::trunc);
    if (!userdata.is_open())
        std::cout << "open file " << pathfilename << " error" << std::endl;

    map<string, userItem>::const_iterator iter;

    for(iter = userTable.begin(); iter != userTable.end(); iter++)
    {
        userdata << iter->second.userID << endl;
        userdata << iter->second.userName << endl;
        userdata << iter->second.password << endl;
        userdata << iter->second.MsgID_record << endl;
        userdata << endl;
    }
    userdata.close();
    cout << "save user data success" << endl;
    return;
}

userManager::~userManager()
{
//    cout << endl << "quit" << endl;
    savedata();
}

bool userManager::register_user(const register_package& pak, int worker_fd)
{
    string username(pak.user);
//    register_package pack_buff;
    if (userTable.find(username) == userTable.end())
    {
        userTable[username].userID = ++userAmount;
        strncpy(userTable[username].userName, pak.user, sizeof(pak.user));
        strncpy(userTable[username].password, pak.password, sizeof(pak.password));
//cout << userTable.size() << endl;
        worker_user_pair[username] = worker_fd;  // 注册成功默认进行登陆


        ofstream userdata;
        userdata.open(USERDATA, ios::out|ios::app);
        if (!userdata.is_open())
        {
            std::cout << "open file " << USERDATA << " error" << std::endl;
            return false;
        }
        userdata << endl;
        userdata << userTable[username].userID << endl;
        userdata << userTable[username].userName << endl;
        userdata << userTable[username].password << endl;
        userdata << userTable[username].MsgID_record << endl;
        userdata.close();

//        cout << "register new user success." << endl;
        return true;
    }
    else
    {
//        cout << "register new user success." << endl;
        return false;
    }
}

// 判断用户密码是否正确
bool userManager::logincheck(const login_package& pak, int worker_fd)
{
    string username(pak.user);
//    cout << pak.user << endl;
//    cout << pak.password <<endl;
//
//    cout << userTable[username].userName << endl;
//    cout << userTable[username].password << endl;
    if (userTable.find(username) == userTable.end())  // 用户名错误
    {
//        cout << "login failed." << endl;
        return false;
    }


    if ( string(pak.password) == userTable[username].password)
    {
        worker_user_pair[username] = worker_fd;
//        cout << (int)userTable[username].MsgID_record << endl;
//cout << "login success." << endl;
        return true;
    }
    else
    {
//        cout << "login failed." << endl;
        return false;
    }
}

// 登出
void userManager::logout(const logout_package& pak)
{
    string username(pak.user);
    worker_user_pair.erase(username);
    if (pak.MsgID_record == 0)
        return;  // 在菜单2中中途退出

//cout << "logout save MsgID_record: " << pak.MsgID_record << endl;
    userTable[username].MsgID_record = pak.MsgID_record;    // 记录最后阅读群聊记录
//    cout << "logout success." << endl;
    return;
}


// 用户修改密码
bool userManager::modify_password(const modPwd_package& pak)
{
    string username(pak.user);
    if (string(pak.previous_password) == userTable[username].password)
    {
        strncpy(userTable[username].password, pak.new_password, sizeof(pak.previous_password));
//        savedata();    // 移至析构函数中
//        cout << "modify password successs." << endl;
        return true;
    }
    else
    {
//        cout << "modify password success." << endl;
        return false;
    }

}


// 返回用户群聊记录
unsigned int userManager::latest_grp_MsgID(const backdata_package& pak)
{
//cout << "synchronization group chat record MsgID" << endl;
    return userTable[string(pak.msg_1)].MsgID_record;
}

// 返回在线用户列表 online_user_list
bool userManager::online_user(const backdata_package& pack, vector<string>& online_user_list)
{
    if (worker_user_pair.size() <= 1)  // 只有唯一用户
    {
//        cout << "only or less than one user" << endl;
        return false;
    }
    else
    {
        map<string, int>::const_iterator iter;
        for(iter = worker_user_pair.begin(); iter != worker_user_pair.end(); iter++)
        {
            if ((iter->first) != pack.msg_1 )
            {
                online_user_list.push_back(iter->first);
//                cout << iter->first << endl;
            }
        }
        return true;
    }
}

// 返回私聊用户的套接字
int userManager::user_socketfd(const privateMsg_package& pack)
{
    string username(pack.destination);
    if (worker_user_pair.find(username) != worker_user_pair.end())
        return worker_user_pair[username];
    else
        return -1;
}

