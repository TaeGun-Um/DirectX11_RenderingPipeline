#include "PrecompileHeader.h"
#include "Base_String.h"
#include "Base_Debug.h"

// 모두 대문자로 변환
std::string Base_String::ToUpper(std::string_view _Str)
{
	std::string Text = _Str.data();

	for (size_t i = 0; i < Text.size(); i++)
	{
		Text[i] = toupper(Text[i]);
	}

	return Text;
}

// Ansi를 UniCode로 변환, string에 담겨있는 한글을 wstring으로 옮겨줄 때 사용
std::wstring Base_String::AnsiToUniCode(std::string_view _Text)
{
	int Size = MultiByteToWideChar(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), nullptr, 0);

	if (0 == Size)
	{
		MsgAssert("문자열 변환에 실패했습니다");
		return L"";
	}

	std::wstring Result;
	Result.resize(Size);

	Size = MultiByteToWideChar(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), &Result[0], Size);

	if (0 == Size)
	{
		MsgAssert("문자열 변환에 실패했습니다");
		return L"";
	}

	return Result;
}

// UniCode를 Ansi로 변환
std::string Base_String::UniCodeToAnsi(std::wstring_view _Text)
{
	int Size = WideCharToMultiByte(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), nullptr, 0, nullptr, nullptr);

	if (0 == Size)
	{
		MsgAssert("문자열 변환에 실패했습니다");
		return "";
	}

	std::string Result;
	Result.resize(Size);

	Size = WideCharToMultiByte(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), &Result[0], Size, nullptr, nullptr);

	if (0 == Size)
	{
		MsgAssert("문자열 변환에 실패했습니다");
		return "";
	}

	return Result;
}