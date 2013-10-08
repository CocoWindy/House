#pragma once
#include <v8\v8.h>
#include <string>
#pragma comment(lib,"v8_base.ia32.lib")
#pragma comment(lib,"v8_nosnapshot.ia32.lib")
#pragma comment(lib,"v8_snapshot.lib")
#pragma comment(lib,"icui18n.lib")
#pragma comment(lib,"icuuc.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"WSock32.lib")
class JsManage
{
public:
	JsManage(void);
	~JsManage(void);
	static void RunScriptRInt(const char * JSscript,v8::Local<v8::ObjectTemplate> globalTemplate,_int64& intRes);
	static void RunScriptRstr(const char * JSscript,v8::Local<v8::ObjectTemplate> globalTemplate,std::string & strRes);
};

