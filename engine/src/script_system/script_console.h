#pragma once
#include <iostream>
#include <base.h>
#include "glm/glm.hpp"


namespace Enik {
class __attribute__((visibility("default"))) Console {
public:

// ? we can't extern "C" templates so ...

static void Trace(const std::string& msg);
static void Trace(const std::string& msg, void* ptr);
static void Trace(const std::string& msg, const std::string& txt);

static void Trace(const std::string& msg, float x);
static void Trace(const std::string& msg, float x, float y);
static void Trace(const std::string& msg, float x, float y, float z);

static void Info (const std::string& msg);
static void Info (const std::string& msg, const std::string& txt);

static void Warn (const std::string& msg);
static void Warn (const std::string& msg, const std::string& txt);

static void Error(const std::string& msg);
static void Error(const std::string& msg, const std::string& txt);
};

}