#ifndef __CONFIGHANDLER_H__
#define __CONFIGHANDLER_H__

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <vector>
#include "StringConverter.h"


/* General class used to read a config file, can read "key = value" paires
*  and parse then to the desired type.
*  Puts all results in a map.
*  Can also write a configfile with given name and keys.
*/


class ConfigHandler
{

public:
	ConfigHandler(std::string name);

	void read();
	void write(const std::vector<std::string> &keys);
	bool exists();

	template <typename T>
	T getValueOfKey(const std::string &key, T const &defaultValue = T()) const
	{
		if (!keyExists(key))
		{
			return defaultValue;
		}
		return StringConvert::String_To_T<T>(contents.find(key)->second);
	}

private:
	std::map<std::string, std::string> contents{};
	std::string fileName;


	//Reading
	void removeComment(std::string &line) const;

	bool onlyWhitespace(const std::string &line) const;

	bool isValidLine(const std::string &line) const;

	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const;

	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const;

	void extractContents(const std::string &line);

	void parseLine(const std::string &line, size_t const lineNo);

	bool keyExists(const std::string &key) const;

	void extractKeys();

	//Writing
	void writeFile(const std::vector<std::string> &keys);
};

#endif
