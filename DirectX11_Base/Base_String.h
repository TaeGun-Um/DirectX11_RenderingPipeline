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

	static std::string ToUpper(std::string_view _Str);

protected:
	
private:
	// constrcuter destructer
	Base_String() {}
	~Base_String() {}
	
};