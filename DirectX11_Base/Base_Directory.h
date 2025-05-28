#pragma once
#include "Base_Debug.h"

// 경로 생성 클래스
class Base_Directory
{
public:
	// constrcuter destructer
	Base_Directory() {}
	~Base_Directory() {}

	// delete Function
	Base_Directory(const Base_Directory& _Other) = delete;
	Base_Directory(Base_Directory&& _Other) noexcept = delete;
	Base_Directory& operator=(const Base_Directory& _Other) = delete;
	Base_Directory& operator=(Base_Directory&& _Other) noexcept = delete;

	std::string MakePath(std::string_view _RelativePath, std::string_view _ThisFile = __FILE__); // 경로 생성
	std::vector<std::string> GetAllFile(std::initializer_list<std::string> _Extensions); // 디렉터리 내에 파일들 검사 후 반환
	std::string GetFileName(); // 파일명 가져오기
	std::string GetExtension(); // 파일 확장자명 가져오기
	std::string FindEntryPoint(std::string_view _Path); // 파일을 열어 특정 문자 검사

	void SetPath(std::string_view _Path) { Path = _Path; }
	const std::string& GetPath() { return Path; }

protected:
	
private:
	std::string Path;
	
};