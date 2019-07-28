#include "ConfigHandler.h"

void ConfigHandler::removeComment(std::string &line) const
{
	if (line.find('#') != line.npos)
		line.erase(line.find('#'));
}

bool ConfigHandler::onlyWhitespace(const std::string &line) const
{
	return (line.find_first_not_of(' ') == line.npos);
}

bool ConfigHandler::isValidLine(const std::string &line) const
{
	std::string temp = line;
	temp.erase(0, temp.find_first_not_of("\t "));
	if (temp[0] == '=')
	{
		return false;
	}

	for (size_t i = temp.find('=') + 1; i < temp.length(); i++)
	{
		if (temp[i] != ' ')
		{
			return true;
		}
	}
	return false;
}

void ConfigHandler::extractKey(std::string &key, size_t const &sepPos, const std::string &line) const
{
	key = line.substr(0, sepPos);
	if (key.find('\t') != line.npos || key.find(' ') != line.npos)
	{
		key.erase(key.find_first_of("\t "));
	}
}

void ConfigHandler::extractValue(std::string &value, size_t const &sepPos, const std::string &line) const
{
	value = line.substr(sepPos + 1);
	value.erase(0, value.find_first_not_of("\t "));
	value.erase(value.find_last_not_of("\t ") + 1);
}

void ConfigHandler::extractContents(const std::string &line)
{
	std::string temp = line;
	//Erase leading whitespace from the line
	temp.erase(0, temp.find_first_not_of("\t "));
	size_t sepPos = temp.find("=");

	std::string key, value;
	extractKey(key, sepPos, line);
	extractValue(value, sepPos, line);

	if (!keyExists(key))
		contents.insert(std::pair<std::string, std::string>(key, value));
	else
		exitWithError("CFG: Can only have unique key names!\n");
}

void ConfigHandler::parseLine(const std::string &line, size_t const lineNo)
{
	if(line.find("=") == line.npos)
		exitWithError("CFG: Couldn't find separator on line: " + StringConvert::T_To_String<size_t>(lineNo) + "\n");
	if (!isValidLine(line))
	{
		std::cout << line << std::endl;
		exitWithError("CFG: Bad format for line: " + StringConvert::T_To_String<size_t>(lineNo) + "\n");
	}
	extractContents(line);
}

void ConfigHandler::extractKeys()
{
	std::ifstream file;
	file.open(fileName.c_str());
	if (!file)
		exitWithError("CFG: File " + fileName + " couldn't be found!\n");
	
	std::string line;
	size_t lineNo = 0;
	while (std::getline(file, line))
	{
		lineNo++;
		std::string temp = line;

		if (temp.empty())
			continue;

		removeComment(temp);
		if (onlyWhitespace(temp))
			continue;

		parseLine(line, lineNo);
	}
	file.close();
}

bool ConfigHandler::keyExists(const std::string &key) const
{
	return (contents.find(key) != contents.end());
}

void ConfigHandler::writeFile(const std::vector<std::string> &keys)
{
	std::ofstream file;
	file.open(fileName.c_str(), std::ios::trunc);
	file << "#configfile of " << fileName << "\n\n" ;
	for (size_t i = 0; i < keys.size(); i++)
	{
		if (keys[i][0] == '#')
			file << keys[i] << "\n";
		else if (keys[i] == ";")
			file << "\n";
		else
			file << keys[i] << " = NULL\n";
	}
	file.close();
}

ConfigHandler::ConfigHandler(std::string name)
	:fileName{ name }
{
}

void ConfigHandler::read()
{
	contents.clear();
	extractKeys();
}

void ConfigHandler::write(const std::vector<std::string> &keys)
{
	if (exists())
		exitWithError(fileName + " Already exists");
	writeFile(keys);
}

bool ConfigHandler::exists()
{
	std::ifstream file(fileName);
	return (bool)file;
}
