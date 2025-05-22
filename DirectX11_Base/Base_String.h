#pragma once

// string �Ӽ� ��ȯ ���� Ŭ����
class Base_String
{
public:
	// delete Function
	Base_String(const Base_String& _Other) = delete;
	Base_String(Base_String&& _Other) noexcept = delete;
	Base_String& operator=(const Base_String& _Other) = delete;
	Base_String& operator=(Base_String&& _Other) noexcept = delete;

	static std::string ToUpper(std::string_view _Str);
	static std::wstring StringToWString(const std::string& str) { return std::wstring(str.begin(), str.end()); }

protected:
	
private:
	// constrcuter destructer
	Base_String() {}
	~Base_String() {}
	
};