#include "SocketForHttp.h"
#include <fstream>
#include <iostream>

int main()
{
	SocketForHttp s;
	/*HttpResponse HR;
	PostName_Key PNK[3];
	PNK[0].name = "id";
	PNK[0].key = "123";
	PNK[1].name = "key";
	PNK[1].key = "321";
	PNK[2].name = "logonsubmit";
	PNK[2].key = "logon";
	HttpHeadForPost HHFP("posttest/test2.php",PNK,3,"5.bbttest.sinaapp.com");
	s.PostRequest(&HR,&HHFP);
	std::ofstream of("test.html");
	of<<HR.body;*/
	Cookie_OnlyNK CONK[2];
	CONK[1].name = "xiao";
	CONK[1].key = "hehe";
	CONK[0].name = "xiao0";
	CONK[0].key = "hehe0";
	HttpHeadForGet HHFG("w.mail.qq.com",CONK,2);
	HttpResponse HR;
	s.GetRequest(&HR,&HHFG);

}