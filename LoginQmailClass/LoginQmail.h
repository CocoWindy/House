#pragma once
#include "SocketForHttp.h"
#include "JsManage.h"
#include "JsFunction.h"
#include <iostream>
#include <fstream>

#pragma comment(lib,"v8_base.ia32.lib")
#pragma comment(lib,"v8_nosnapshot.ia32.lib")
#pragma comment(lib,"v8_snapshot.lib")
#pragma comment(lib,"icui18n.lib")
#pragma comment(lib,"icuuc.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"WSock32.lib")

class LoginQmail
{
public:
	LoginQmail(std::string u,std::string pwd);
	~LoginQmail(void);
	void login();
private:
	Cookie_OnlyNK* pCONK;
	std::string username,password;

	void setQTSVALUE(std::string& body);
	void readFile(std::ifstream* pof,std::string& str);
	void setPostName_Key(PostName_Key* p,const std::string& username,const std::string& key,const std::string& aliastype);
	void setRUrl(std::string& str,std::string& url);
};

