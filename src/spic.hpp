#pragma once

#include <string>

namespace Spic::Impl {

void throwSystemExceptionIf(bool condition);
void throwNodeExceptionIf(bool condition, const std::string& msg);
bool failed(int returnCode);
bool fileDoesNotExistError();

}