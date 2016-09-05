# **即聊 · GChat**

----------
即聊是一款Linux平台下简单的即时聊天工具。支持群聊聊天室以及一对一私聊。群聊聊天室中每位用户的聊天内容都公开于所有用户。支持用户云端群聊消息记录，未读消息自动同步，登陆群聊后自动展现。私聊暂不支持聊天记录保存。


# 构建
通过命令`make`进行项目构建。构建后会在当前目录生成 `GChatServer_master` 、`GChatServer_worker` 和 `GChat_client`三个可执行文件。

```bash
	make
```
# 使用方法
服务器中需要启动`GChatServer_maste`以及`GChatServer_worker`两个程序。`GChatServer_worker`需要在启动参数中指定服务器对外公开的IP地址以及端口号，客户端`GChat_client`运行时同样需要在参数中给定服务器的IP地址以及端口号。

客户端启动后出现功能选项：
    1. Register Account
    2. Login
    3. Modify Password
    4. Exit


1. 选项1为注册用户，需要填写用户名称以及用户密码，若用户名与已注册用户重名，则注册失败，需要重新注册，注册成功后默认为登录状态；
2. 选项2为用户登录，用户输入合法用户名以及用户密码，进行登录，登录后选择进行群聊还是私聊，进入群聊首先接收未读消息，然后开始群聊；进入私聊则要选择在线用户进行消息发送；
3. 选项3为用户修改密码，需要输入合法注册的用户名，输入旧登录密码，再输入新的登录密码，然后提交。若旧密码错误或用户名无效则修改密码失败，修改密码成功需要重新登陆；
4. 选项4为退出程序。


## 参数格式:
* `reflect_server <IP> <port>`
* `reflect_client <IP> <port>`

例如:
```bash
	./GChatServer_master
	./GChatServer_worker 127.0.0.1 8888
	./GChat_client 127.0.0.1 8888
```
