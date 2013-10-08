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
	p[8].key = " ��¼ ";

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
	HttpHeadForGet HHFG("w.mail.qq.com");//����http��ص���Ϣ
	SKFH.GetRequest(&HR,&HHFG);//���w.mail.qq.com��html,Ϊ��ȡ��ts��ֵ

	setQTSVALUE(HR.body);//ȡ��html�ļ����ts��ֵ,������QTSVALUE
	QPASSWORD = password;
	//////////////////////////////////////////////////////////////v8
	v8::Isolate *isolate = v8::Isolate::GetCurrent();//ȡ��isolate(����ÿ��v8�������Ҫ����)(�������������������Ϊ�˶��߳�)
	v8::HandleScope handle_scope(isolate);//(��������)
	//���÷��ʻص�����
	v8::Handle<v8::ObjectTemplate> globalTemplate = v8::ObjectTemplate::New();
	globalTemplate->SetAccessor(v8::String::New("QPASSWORD"),QPASSWORDGetter);
	globalTemplate->SetAccessor(v8::String::New("QTSVALUE"),QTSVALUEGetter);
	std::ifstream ifs("JS/QJS.SNTJS");//��ȡjs�ļ�
	std::string JSscript;
	std::string res;
	readFile(&ifs,JSscript);//�ҵĶ�ȡ�����ļ��ĺ���
	JsManage::RunScriptRstr(JSscript.c_str(),globalTemplate,res);//����js,��ȡ��ֵ(����res��)
	///////////////////////////////////////////////////////////////
	HttpResponse HR2;
	PostName_Key pNK[11];
	setPostName_Key(pNK,username,res,"@qq.com");//����post������
	HttpHeadForPost HHFP("cgi-bin/login?sid=",pNK,11,0,0,"w.mail.qq.com");//postͷ��һ��������url����(����������),�ڶ�����post����,��������post���ݵĸ���,��4,��5��cookie������������
	SKFH.PostRequest(&HR2,&HHFP);//post
	std::ofstream ofs("qmailrequest.html");
	ofs<<HR2.body;
	//��ȡ�����js���Զ���ת��ҳ��
	std::string rUrl;
	setRUrl(HR2.body,rUrl);//ȡ��url

	Sleep(500);//��ͣ500����
	pCONK = new Cookie_OnlyNK[HR2.cookies.size()];
	Cookie_OnlyNK::CookieDataToCookie_OnlyNK(HR2.cookies.cookieData.data(),pCONK,HR2.cookies.size());//��cookieDataת����Cookie_OnlyNK��
	HttpHeadForGet HHFG2("w.mail.qq.com",pCONK,HR2.cookies.size(),rUrl);//����ͷ
	HttpResponse HR3;
	SKFH.GetRequest(&HR3,&HHFG2);//ȡ����Ϣ
	std::ofstream ofs2("loginok.html");
	ofs2<<HR3.body;
}