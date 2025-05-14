#include "PrecompileHeader.h"
#include "Base_String.h"

std::string Base_String::ToUpper(std::string_view _Str)
{
	std::string Text = _Str.data();

	for (size_t i = 0; i < Text.size(); i++)
	{
		Text[i] = toupper(Text[i]);
	}

	return Text;
}