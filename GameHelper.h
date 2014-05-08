#ifndef GAMEHELPER_H
#define GAMEHELPER_H

#include "qstring.h"
#include <string>

class GameHelper {

public:
	GameHelper() {}
	static inline std::string toString(const QString& str) { return str.toStdString(); }

};
//extern GameHelper* gameHelper = NULL;

#endif