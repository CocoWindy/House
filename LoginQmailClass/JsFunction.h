#pragma once
#include <v8\v8.h>
#include <string>

extern std::string QPASSWORD;
extern std::string QTSVALUE;

void QPASSWORDGetter(v8::Local<v8::String> key,const v8::PropertyCallbackInfo<v8::Value>& info);

void QTSVALUEGetter(v8::Local<v8::String> key,const v8::PropertyCallbackInfo<v8::Value>& info);

void XSetter(v8::Local<v8::String> key, v8::Local<v8::Value> value,const v8::PropertyCallbackInfo<void>& info);