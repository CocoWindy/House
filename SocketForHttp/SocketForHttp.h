#pragma once
#include <WINSOCK2.H>
#include <string>
#include <stdio.h>
#include <sstream>
#include <vector>
#include "TextAnalysis.h"
#pragma comment(lib,"ws2_32.lib")
#define DEBUG
#define MYHTTP_RECV_OVERTIME -1
#define MYHTTP_ANALYSISHEADBYLINE_OVERFLOW -2
#define MYHTTP_ANALYSISHEADBYLINE_UNKNOWERROR -3

struct CookieData
{
	std::string name,data,path,domain,expires,secure;
	_int64 maxAge;
};

class Cookie_OnlyNK
{
public:
	std::string name,key;
	static void CookieDataToCookie_OnlyNK(const CookieData* c,Cookie_OnlyNK* cNK,int Lenth)
	{
		for(int i=0;i<Lenth;++i)
		{
			cNK[i].name = c[i].name;
			cNK[i].key = c[i].data;
		}
	}
};

struct PostName_Key
{
	std::string name,key;
};

class HttpHeadForGet
{
public:
	std::string hostUrl,accept,acceptLanguage,requestData;
	const Cookie_OnlyNK *pCONK;
	int length;
	HttpHeadForGet(std::string MhostUrl,const Cookie_OnlyNK *MpCONK = 0,int Mlength = 0,std::string MrequestData = "",std::string Maccept = "*/*",std::string MacceptLanguage = "zh-cn")
		: hostUrl(MhostUrl),requestData(MrequestData),accept(Maccept),acceptLanguage(MacceptLanguage),pCONK(MpCONK),length(Mlength)
	{
	}
};

class HttpHeadForPost
{
	public:
	std::string url,accept,acceptLanguage,hostUrl;
	PostName_Key *pPNK;
	int lengthNK;
	Cookie_OnlyNK *pCONK;
	int cLength;
	HttpHeadForPost(std::string Murl,PostName_Key *MpPNK,int Mlength,Cookie_OnlyNK *MpCONK = 0,int McLength = 0,std::string hUrl = "",std::string Maccept = "*/*",std::string MacceptLanguage = "zh-cn")
		: url(Murl),accept(Maccept),acceptLanguage(MacceptLanguage),pPNK(MpPNK),lengthNK(Mlength)
	{
		if(hUrl == "")
			hostUrl = url;
		else
			hostUrl = hUrl;
	}
};

class CookieInformation
{
public:
	std::vector<CookieData> cookieData;
	void addCookieData(CookieData &c)
	{
		cookieData.push_back(c);
	}
	int size()
	{
		return cookieData.size();
	}
};

struct ContentType
{
	std::string type,charset;
};

struct HttpResponse
{
	std::string body,status,date;
	ContentType contentType;
	CookieInformation cookies;
	u_int64 length;
};

class SocketForHttp
{
private:
	const static int bufLength = 1024;
	std::string errorInformation;
	SOCKET clientSocket;
	struct Overtime
	{
		DWORD limitedTime,startTime,currentTime,allTime;
	};
	Overtime overtime;
public:
	SocketForHttp(void);
	~SocketForHttp(void);
	int GetRequest(HttpResponse * pHR,HttpHeadForGet* HHFG); //以get的形式获取html(第一个参数是一个返回内容的指针,第二个是选项) ////////////////////用的话,只需要用这个函数
	void SetRequestForGet(std::string& req, HttpHeadForGet* HHFG); //设置get的头
	std::string  IntToString(int a); //int转string
private:
	void RestartOvertime(void);
	void SetOvertime(DWORD time,DWORD start);
	bool CheckOvertime(void);
public:
	void Analysis(HttpResponse * pHR, std::string& str); //分析返回内容的函数
private:
	int AnalysisHeadByLine(std::string* data,std::string& str,int n); //分析返回内容的函数,已废弃
	int StringToInt(std::string str); //string转int
public:
	int PostRequest(HttpResponse* HR, HttpHeadForPost* HHFP); //以post的形式获取html(参数同GetRequest) ////////////////////用的话,只需要用这个函数
	std::string UrlEncording(const char* str);
	void SetRequestForPost(std::string& req, HttpHeadForPost* HHFP); //设置post的头
};
