#pragma once
#include "Base_Debug.h"

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

	std::string MakePath(std::string_view _RelativePath, std::string_view _ThisFile = __FILE__);
	std::vector<std::string> GetAllFile(std::initializer_list<std::string> _Extensions);
	std::string FindEntryPoint(std::string_view _Path);

	const std::string& GetPath() { return Path; }

protected:
	
private:
	std::string Path;
	
};