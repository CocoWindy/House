#include "JsManage.h"


JsManage::JsManage(void)
{
}


JsManage::~JsManage(void)
{
}

void JsManage::RunScriptRInt(const char * JSscript,v8::Local<v8::ObjectTemplate> globalTemplate,_int64& intRes)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	v8::Handle<v8::Context> context = v8::Context::New(isolate,0,globalTemplate);
	v8::Context::Scope context_scope(context);
	v8::Handle<v8::String> source = v8::String::New(JSscript);
	v8::String * str = *source;
	v8::Handle<v8::Script> script = v8::Script::Compile(source);
	v8::Handle<v8::Value> result = script->Run();
	intRes = result->IntegerValue();
}


void JsManage::RunScriptRstr(const char * JSscript,v8::Local<v8::ObjectTemplate> globalTemplate,std::string& strRes)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	v8::Handle<v8::Context> context = v8::Context::New(isolate,0,globalTemplate);
	v8::Context::Scope context_scope(context);
	v8::Handle<v8::String> source = v8::String::New(JSscript);
	v8::String * str = *source;
	v8::Handle<v8::Script> script = v8::Script::Compile(source);
	v8::Handle<v8::Value> result = script->Run();
	v8::String::AsciiValue ascii(result);
	strRes = *ascii; 
}