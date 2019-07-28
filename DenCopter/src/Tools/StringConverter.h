#ifndef __STRINGCONVERTER_H__
#define __STRINGCONVERTER_H__


#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>

/*
*	A class that specializes conversion from string to a type and the reverse.
*/

//Helperfunction
void exitWithError(const std::string &error);

class StringConvert
{
public:
	template<typename T>
	inline static std::string T_To_String(T const &val)
	{
			std::ostringstream ostr;
			ostr << val;
			return ostr.str();
	}

	template<typename T>
	inline static T String_To_T(std::string const &val)
	{
		std::istringstream istr(val);
		T outputVal;
		if (!(istr >> outputVal))
		{
			std::cout << val << std::endl;
			exitWithError("CFG: Not a valid " + (std::string)typeid(T).name() + " received!\n");
		}
		return outputVal;
	}
};


template<>
inline  std::string StringConvert::String_To_T<std::string>(std::string const &val)
{
	return val;
}

#endif // !__STRINGCONVERTER_H__