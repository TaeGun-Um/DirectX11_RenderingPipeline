#pragma once

// string 속성 변환 헬퍼 클래스
class Base_String
{
public:
	// delete Function
	Base_String(const Base_String& _Other) = delete;
	Base_String(Base_String&& _Other) noexcept = delete;
	Base_String& operator=(const Base_String& _Other) = delete;
	Base_String& operator=(Base_String&& _Other) noexcept = delete;

	static std::string ToUpper(std::string_view _Str); // 모두 대문자로 변환
	static std::wstring AnsiToUniCode(std::string_view _Text); // Ansi를 UniCode로 변환
	static std::string UniCodeToAnsi(std::wstring_view _Text); // UniCode를 Ansi로 변환

protected:
	
private:
	// constrcuter destructer
	Base_String() {}
	~Base_String() {}
	
};