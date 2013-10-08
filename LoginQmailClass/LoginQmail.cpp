#include "LoginQmail.h"

std::string QPASSWORD;
std::string QTSVALUE;

LoginQmail::LoginQmail(std::string u,std::string pwd)
	: username(u),password(pwd)
{
}


LoginQmail::~LoginQmail(void)
{
}


void LoginQmail::setQTSVALUE(std::string& body)
{
	std::string strVa = "value=\"";
	bool fError = false;
	int i=body.find("id=\"ts\"");
	int j,k;
	for(j = i-1;;--j)
	{
		if(body[j]=='>')
		{
			fError = true;
			break;
		}
		else if(body[j]=='<')
			break;
	}
	for(k = i+1;;++k)
	{
		if(body[k]=='<')
		{
			fError = true;
			break;
		}
		else if(body[k]=='>')
			break;
	}
	if(!fError)
	{
		std::string strValue = "value=\"";
		fError = true;
		for(int m = j,n=0;m<k;++m)
		{
			if(n<strValue.size())
			{
				if(body[m] == strValue[n])
				{
					++n;
					continue;
				}
			}
			else
			{
				fError = false;
				while(1)
				{
					if(body[m] == '"')
						return;
					char hehe = body[m];
					QTSVALUE += body[m];
					++m;
				}
			}
		}
	}
}


void LoginQmail::readFile(std::ifstream* pifs,std::string& str)
{
	char c;
	while((c = pifs->get())!=EOF)
		str += c;
}


void LoginQmail::setPostName_Key(PostName_Key* p,const std::string& username,const std::string& key,const std::string& aliastype)
{
	p[0].name = "device";

	p[1].name = "f";
	p[1].key = "xhtml";

	p[2].name = "delegate_url";

	p[3].name = "action";

	p[4].name = "tfcont";

	p[5].name = "uin";
	p[5].key = username;

	p[6].name = "aliastype";
	p[6].key = aliastype;

	p[7].name = "pwd";

	p[8].name = "btlogin";
	p[8].key = " 登录 ";

	p[9].name = "ts";
	p[9].key = QTSVALUE;

	p[10].name = "p";
	p[10].key = key;
}


void LoginQmail::setRUrl(std::string& str,std::string& url)
{
	std::string strLocal = "location.href=\"http://w.mail.qq.com/";
	bool error = true;
	int i=0,j=0;
	while(i<str.size())
	{
		if(j>=strLocal.size())
		{
			error = false;
			break;
		}
		if(str[i] == strLocal[j])
			++j;
		else
			j = 0;
		++i;
	}
	if(!error)
	{
		for(;str[i]!='\"' && i<str.size();++i)
			url += str[i];
	}
}


void LoginQmail::login()
{
	SocketForHttp SKFH;
	HttpResponse HR;
	HttpHeadForGet HHFG("w.mail.qq.com");//设置http相关的信息
	SKFH.GetRequest(&HR,&HHFG);//获得w.mail.qq.com的html,为了取得ts的值

	setQTSVALUE(HR.body);//取得html文件里的ts的值,并存入QTSVALUE
	QPASSWORD = password;
	//////////////////////////////////////////////////////////////v8
	v8::Isolate *isolate = v8::Isolate::GetCurrent();//取得isolate(就是每个v8程序必须要做的)(设置这个函数的意义是为了多线程)
	v8::HandleScope handle_scope(isolate);//(设置名域)
	//设置访问回调函数
	v8::Handle<v8::ObjectTemplate> globalTemplate = v8::ObjectTemplate::New();
	globalTemplate->SetAccessor(v8::String::New("QPASSWORD"),QPASSWORDGetter);
	globalTemplate->SetAccessor(v8::String::New("QTSVALUE"),QTSVALUEGetter);
	std::ifstream ifs("JS/QJS.SNTJS");//读取js文件
	std::string JSscript;
	std::string res;
	readFile(&ifs,JSscript);//我的读取整个文件的函数
	JsManage::RunScriptRstr(JSscript.c_str(),globalTemplate,res);//运行js,并取得值(存在res里)
	///////////////////////////////////////////////////////////////
	HttpResponse HR2;
	PostName_Key pNK[11];
	setPostName_Key(pNK,username,res,"@qq.com");//设置post的内容
	HttpHeadForPost HHFP("cgi-bin/login?sid=",pNK,11,0,0,"w.mail.qq.com");//post头第一个参数是url域名(不包括主机),第二个是post内容,第三个是post内容的个数,第4,第5是cookie第六是主机名
	SKFH.PostRequest(&HR2,&HHFP);//post
	std::ofstream ofs("qmailrequest.html");
	ofs<<HR2.body;
	//获取里面的js的自动跳转的页面
	std::string rUrl;
	setRUrl(HR2.body,rUrl);//取得url

	Sleep(500);//暂停500毫秒
	pCONK = new Cookie_OnlyNK[HR2.cookies.size()];
	Cookie_OnlyNK::CookieDataToCookie_OnlyNK(HR2.cookies.cookieData.data(),pCONK,HR2.cookies.size());//把cookieData转换成Cookie_OnlyNK型
	HttpHeadForGet HHFG2("w.mail.qq.com",pCONK,HR2.cookies.size(),rUrl);//设置头
	HttpResponse HR3;
	SKFH.GetRequest(&HR3,&HHFG2);//取得信息
	std::ofstream ofs2("loginok.html");
	ofs2<<HR3.body;
}