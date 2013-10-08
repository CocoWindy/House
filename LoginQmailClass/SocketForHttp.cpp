#include "SocketForHttp.h"


SocketForHttp::SocketForHttp(void)
{
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	int err = WSAStartup(versionRequired,&wsaData);
#ifdef DEBUG
	switch (err)
	{
	case 0:
		break;
	case WSASYSNOTREADY:
		errorInformation += "Fuction WSAStartup Error : The underlying network subsystem is not ready for network communication.\n";
	case WSAVERNOTSUPPORTED:
		errorInformation += "Fuction WSAStartup Error : The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.\n";
	case WSAEINPROGRESS:
		errorInformation += "Fuction WSAStartup Error : A blocking Windows Sockets 1.1 operation is in progress.\n";
	case WSAEPROCLIM:
		errorInformation += "Fuction WSAStartup Error : A limit on the number of tasks supported by the Windows Sockets implementation has been reached.\n";
	case WSAEFAULT:
		errorInformation += "Fuction WSAStartup Error : The lpWSAData parameter is not a valid pointer.\n";
	}
#endif


	SetOvertime(5000,10);
}


SocketForHttp::~SocketForHttp(void)
{
	closesocket(clientSocket);
	WSACleanup();
}


int SocketForHttp::GetRequest(HttpResponse * pHR, HttpHeadForGet* HHFG)
{
	clientSocket = socket(AF_INET,SOCK_STREAM,0);
	std::string req;
	SetRequestForGet(req,HHFG);

	LPHOSTENT lphostent=gethostbyname(HHFG->hostUrl.data());
	if(lphostent==NULL)
	{
#ifdef DEBUG
		int err = WSAGetLastError();
		switch (err)
		{
		case WSANOTINITIALISED:
			errorInformation += "Function gethostbyname Error : A successful WSAStartup call must occur before using this function.\n";
			break;
		case WSAENETDOWN:
			errorInformation += "Function gethostbyname Error : The network subsystem has failed.\n";
			break;
		case WSAHOST_NOT_FOUND:
			errorInformation += "Function gethostbyname Error : Authoritative answer host not found.\n";
			break;
		case WSATRY_AGAIN:
			errorInformation += "Function gethostbyname Error : Nonauthoritative host not found, or server failure.\n";
			break;
		case WSANO_RECOVERY:
			errorInformation += "Function gethostbyname Error : A nonrecoverable error occurred.\n";
			break;
		case WSANO_DATA:
			errorInformation += "Function gethostbyname Error : The requested name is valid, but no data of the requested type was found."
				"This error is also returned if the name parameter contains a string representation of an IPv6 address or an illegal IPv4 address."
				"This error should not be interpreted to mean that the name parameter contains a name string that has been validated for a particular protocol"
				"(an IP hostname, for example). Since Winsock supports multiple name service providers, a name may potentially be valid for one provider and not accepted by another provider.\n";
			break;
		case WSAEINPROGRESS:
			errorInformation += "Function gethostbyname Error : A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n";
			break;
		case WSAEFAULT:
			errorInformation += "Function gethostbyname Error : The name parameter is not a valid part of the user address space.\n";
			break;
		case WSAEINTR:
			errorInformation += "Function gethostbyname Error : A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n";
			break;
		}
#endif
		return false;
	}
	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr=*((LPIN_ADDR)*(lphostent->h_addr_list));
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(80);

	int nRet = connect(clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));
	if(nRet == SOCKET_ERROR)
	{
#ifdef DEBUG
		int err =  WSAGetLastError();
		errorInformation += "Function connect Error : Error Code";
		errorInformation += IntToString(err);
		errorInformation += "\n";
#endif
		closesocket(clientSocket);
		return 0;
	}
	send(clientSocket,req.data(),req.size(),0);
	char receiveBuf[bufLength];
	Sleep(10);
	std::string str;
	while(1)
	{
		int re = recv(clientSocket,(LPSTR)receiveBuf,sizeof(receiveBuf)-1,0);
		if(re > 0)
		{
			receiveBuf[re] = '\0';
			str += receiveBuf;
			RestartOvertime();
		}
		else if(re == SOCKET_ERROR)
		{
#ifdef DEBUG
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
				errorInformation += "Function recv Error : Error Code";
				errorInformation += IntToString(err);
				errorInformation += "\n";
			}
#endif
			if(CheckOvertime())
				continue;
			else
			{
#ifdef DEBUG
				errorInformation += "Function recv Error : Overtime.\n";
#endif
				return MYHTTP_RECV_OVERTIME;
			}
		}
		else if(re == 0)
			break;
	}
	Analysis(pHR,str);
	closesocket(clientSocket);
	return 0;
}


void SocketForHttp::SetRequestForGet(std::string& req,HttpHeadForGet* HHFG)
{
	if(HHFG->requestData != "")
	{
		if(HHFG->requestData[HHFG->requestData.size()-1] != '/')
			HHFG->requestData += '/';
	}
	req = "GET /";
	req += HHFG->requestData;
	req +=" HTTP/1.1\r\nAccept:";
	req += HHFG->accept;
	req += "\r\nAccept-Language: ";
	req += HHFG->acceptLanguage;
	req += "\r\n"
		"Connection: close\r\n"
		"Host: ";
	req += HHFG->hostUrl;
	req += "\r\n";
	if(HHFG->length > 0)
	{
		req += "Cookie:";
		for(int i=0;i<HHFG->length;++i)
		{
			req += ' ';
			req += HHFG->pCONK[i].name;
			req += '=';
			req += HHFG->pCONK[i].key;
			req += ';';
		}
		req += "\r\n";
	}
	req += "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)\r\n\r\n";
}


std::string  SocketForHttp::IntToString(int a)
{
	std::stringstream ss;
	ss<<a;
	std::string Ts;
	ss>>Ts;
	return Ts;
}


void SocketForHttp::SetOvertime(DWORD time,DWORD start)
{
	overtime.allTime = start;
	overtime.currentTime = start;
	overtime.limitedTime = time;
	overtime.startTime = start;
}


void SocketForHttp::RestartOvertime(void)
{
	overtime.allTime = overtime.startTime;
	overtime.currentTime = overtime.startTime;
}


bool SocketForHttp::CheckOvertime(void)
{
	Sleep(overtime.currentTime);
	overtime.allTime += overtime.currentTime;
	overtime.currentTime *= 2;

	if(overtime.allTime > overtime.limitedTime)
	{
		RestartOvertime();
		return false;
	}
	else
		return true;
}


void SocketForHttp::Analysis(HttpResponse * pHR, std::string& str)
{
	TextAnalysis tA(&str);
	int s = tA.Analysis("\n:=;");
	pHR->body = str.data() + s;
	tA.InitializeGet();
	std::string str1,str2;
	TA_Node_Data *pTAND = tA.GetNextData();
	pHR->status = pTAND->text;
	for(;pTAND = tA.GetNextData();)
	{
		str1 = pTAND->text;
		if(str1 == "Date" && pTAND->sign == ':')
		{
			pTAND = tA.GetNextData();
			pHR->date = pTAND->text;
		}
		else if(str1 == "Set-Cookie" && pTAND->sign == ':')
		{
			pTAND = tA.GetNextData();
			CookieData cD;
			cD.name = pTAND->text;
			while(1)
			{
				pTAND = tA.GetNextData();
				if(pTAND->sign == ';')
				{
					cD.data += pTAND->text;
					break;
				}
				cD.data += pTAND->text;
				cD.data += pTAND->sign;
			}
			while((pTAND = tA.GetNextData()) && (pTAND->sign != '\n'))
			{
				std::string cDStr = pTAND->text;
				if(cDStr == "path" && pTAND->sign == '=')
				{
					if(!(pTAND = tA.GetNextData()))
						break;
					cD.path = pTAND->text;
					while(pTAND->sign != ';')
					{
						cD.path += pTAND->sign;
						pTAND = tA.GetNextData();
						cD.path += pTAND->text;
					}
				}
				else if(cDStr == "domain")
				{
					if(!(pTAND = tA.GetNextData()))
						break;
					pTAND = tA.GetNextData();
					cD.domain = pTAND->text;
					while(pTAND->sign != ';')
					{
						cD.domain += pTAND->sign;
						pTAND = tA.GetNextData();
						cD.domain += pTAND->text;
					}
				}
			}
			pHR->cookies.addCookieData(cD);
		}
		else if(str1 == "charset" && pTAND->sign == '=')
		{
			pTAND = tA.GetNextData();
			pHR->contentType.charset = pTAND->text;
		}
	}
}


int SocketForHttp::AnalysisHeadByLine(std::string* data,std::string& str,int n)
{
	int i = 0,cur = 0;
	for(;1;++n)
	{
		if(n>=str.size())
		{
#ifdef DEBUG
			errorInformation += "Fuction AnalysisHeadByLine Error : Memory overflow!\n";
#endif
			return MYHTTP_ANALYSISHEADBYLINE_OVERFLOW;
		}
		switch(str[n])
		{
		case '\n':
			return n+1;
			break;
		case ':':
			cur = 1;
			++n;
			break;
		}
		data[cur] += str[n];
	}
	return MYHTTP_ANALYSISHEADBYLINE_UNKNOWERROR;
}


int SocketForHttp::StringToInt(std::string str)
{
	int a;
	std::stringstream ss;
	ss<<str;
	ss>>a;
	return a;
}


int SocketForHttp::PostRequest(HttpResponse* pHR, HttpHeadForPost* HHFP)
{
	clientSocket = socket(AF_INET,SOCK_STREAM,0);
	std::string req;
	SetRequestForPost(req,HHFP);

	LPHOSTENT lphostent=gethostbyname(HHFP->hostUrl.data());
	if(lphostent==NULL)
	{
#ifdef DEBUG
		int err = WSAGetLastError();
		switch (err)
		{
		case WSANOTINITIALISED:
			errorInformation += "Function gethostbyname Error : A successful WSAStartup call must occur before using this function.\n";
			break;
		case WSAENETDOWN:
			errorInformation += "Function gethostbyname Error : The network subsystem has failed.\n";
			break;
		case WSAHOST_NOT_FOUND:
			errorInformation += "Function gethostbyname Error : Authoritative answer host not found.\n";
			break;
		case WSATRY_AGAIN:
			errorInformation += "Function gethostbyname Error : Nonauthoritative host not found, or server failure.\n";
			break;
		case WSANO_RECOVERY:
			errorInformation += "Function gethostbyname Error : A nonrecoverable error occurred.\n";
			break;
		case WSANO_DATA:
			errorInformation += "Function gethostbyname Error : The requested name is valid, but no data of the requested type was found."
				"This error is also returned if the name parameter contains a string representation of an IPv6 address or an illegal IPv4 address."
				"This error should not be interpreted to mean that the name parameter contains a name string that has been validated for a particular protocol"
				"(an IP hostname, for example). Since Winsock supports multiple name service providers, a name may potentially be valid for one provider and not accepted by another provider.\n";
			break;
		case WSAEINPROGRESS:
			errorInformation += "Function gethostbyname Error : A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n";
			break;
		case WSAEFAULT:
			errorInformation += "Function gethostbyname Error : The name parameter is not a valid part of the user address space.\n";
			break;
		case WSAEINTR:
			errorInformation += "Function gethostbyname Error : A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n";
			break;
		}
#endif
		return false;
	}
	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr=*((LPIN_ADDR)*(lphostent->h_addr_list));
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(80);

	int nRet = connect(clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));
	if(nRet == SOCKET_ERROR)
	{
#ifdef DEBUG
		int err =  WSAGetLastError();
		errorInformation += "Function connect Error : Error Code";
		errorInformation += IntToString(err);
		errorInformation += "\n";
#endif
		closesocket(clientSocket);
		return 0;
	}
	send(clientSocket,req.data(),req.size(),0);
	char receiveBuf[bufLength];
	Sleep(10);
	std::string str;
	while(1)
	{
		int re = recv(clientSocket,(LPSTR)receiveBuf,sizeof(receiveBuf)-1,0);
		if(re > 0)
		{
			receiveBuf[re] = '\0';
			str += receiveBuf;
			RestartOvertime();
		}
		else if(re == SOCKET_ERROR)
		{
#ifdef DEBUG
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
				errorInformation += "Function recv Error : Error Code";
				errorInformation += IntToString(err);
				errorInformation += "\n";
			}
#endif
			if(CheckOvertime())
				continue;
			else
			{
#ifdef DEBUG
				errorInformation += "Function recv Error : Overtime.\n";
#endif
				return MYHTTP_RECV_OVERTIME;
			}
		}
		else if(re == 0)
			break;
	}
	Analysis(pHR,str);
	closesocket(clientSocket);
	return 0;
}


std::string SocketForHttp::UrlEncording(const char* str)
{
	std::stringstream ss;
	for(int i=0;str[i];++i)
	{
		if((str[i]<= 'z' && str[i]>= 'A') ||(str[i]<= '9' && str[i]>= '0'))
			ss<<str[i];
		else if(str[i]== ' ')
			ss<<'+';
		else
			ss<<'%'<<std::hex<<(int)(str[i] & 255);
	}
	std::string rStr;
	ss>>rStr;
	return rStr;
}


void SocketForHttp::SetRequestForPost(std::string& req, HttpHeadForPost* HHFP)
{
	std::string str;
	if(HHFP->lengthNK >= 1)
	{
		str += this->UrlEncording((HHFP->pPNK[0].name).data());
		str += '=';
		str += this->UrlEncording((HHFP->pPNK[0].key).data());
		for(int i=1;i<HHFP->lengthNK;++i)
		{
			str += '&';
			str += this->UrlEncording((HHFP->pPNK[i].name).data());
			str += '=';
			str += this->UrlEncording((HHFP->pPNK[i].key).data());
		}
	}

	req = "POST /";
	req += HHFP->url;
	req +=" HTTP/1.1\r\nAccept:";
	req += HHFP->accept;
	req += "\r\nAccept-Language: ";
	req += HHFP->acceptLanguage;
	req += "\r\n"
		"Connection: close\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Host: ";
	req += HHFP->hostUrl;
	req += "\r\n";
	req += "Content-Length: ";
	req += this->IntToString(str.size());
	req += "\r\n";
	if(HHFP->cLength > 0)
	{
		req += "Cookie:";
		for(int i=0;i<HHFP->cLength;++i)
		{
			req += ' ';
			req += HHFP->pCONK[i].name;
			req += '=';
			req += HHFP->pCONK[i].key;
			req += ';';
		}
		req += "\r\n";
	}
	req +=	"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)\r\n\r\n";
	req += str;
}
