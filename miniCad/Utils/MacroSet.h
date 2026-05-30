//
// Created by ZQD on 2026/5/30.
//

#pragma once

///单例宏
#define DECLARE_SINGLETON(className)\
private:\
className();\
className(className&) = delete;\
className& operator=(const className&) = delete;\
public:\
static className& Instance();

#define IMPLEMENT_SINGLETON(className)\
className& 	className::Instance()\
{\
static className _instance;\
return _instance;\
}
