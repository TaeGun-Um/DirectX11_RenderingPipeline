#include "PrecompileHeader.h"
#include "Base_String.h"
#include "Base_Debug.h"

// ��� �빮�ڷ� ��ȯ
std::string Base_String::ToUpper(std::string_view _Str)
{
	std::string Text = _Str.data();

	for (size_t i = 0; i < Text.size(); i++)
	{
		Text[i] = toupper(Text[i]);
	}

	return Text;
}

// Ansi�� UniCode�� ��ȯ, string�� ����ִ� �ѱ��� wstring���� �Ű��� �� ���
std::wstring Base_String::AnsiToUniCode(std::string_view _Text)
{
	int Size = MultiByteToWideChar(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), nullptr, 0);

	if (0 == Size)
	{
		MsgAssert("���ڿ� ��ȯ�� �����߽��ϴ�");
		return L"";
	}

	std::wstring Result;
	Result.resize(Size);

	Size = MultiByteToWideChar(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), &Result[0], Size);

	if (0 == Size)
	{
		MsgAssert("���ڿ� ��ȯ�� �����߽��ϴ�");
		return L"";
	}

	return Result;
}

// UniCode�� Ansi�� ��ȯ
std::string Base_String::UniCodeToAnsi(std::wstring_view _Text)
{
	int Size = WideCharToMultiByte(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), nullptr, 0, nullptr, nullptr);

	if (0 == Size)
	{
		MsgAssert("���ڿ� ��ȯ�� �����߽��ϴ�");
		return "";
	}

	std::string Result;
	Result.resize(Size);

	Size = WideCharToMultiByte(CP_ACP, 0, _Text.data(), static_cast<int>(_Text.size()), &Result[0], Size, nullptr, nullptr);

	if (0 == Size)
	{
		MsgAssert("���ڿ� ��ȯ�� �����߽��ϴ�");
		return "";
	}

	return Result;
}