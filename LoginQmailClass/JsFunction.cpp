#include "JsFunction.h"


void QPASSWORDGetter(v8::Local<v8::String> key,const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::New(QPASSWORD.c_str(),QPASSWORD.size()));
}


void QTSVALUEGetter(v8::Local<v8::String> key,const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::New(QTSVALUE.c_str(),QTSVALUE.length()));
}


void XSetter(v8::Local<v8::String> key, v8::Local<v8::Value> value,const v8::PropertyCallbackInfo<void>& info)
{
}