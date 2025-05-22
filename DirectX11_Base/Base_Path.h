#pragma once
#include "Base_Debug.h"

class Base_Path
{
public:
	// constrcuter destructer
	Base_Path() {}
	~Base_Path() {}

	// delete Function
	Base_Path(const Base_Path& _Other) = delete;
	Base_Path(Base_Path&& _Other) noexcept = delete;
	Base_Path& operator=(const Base_Path& _Other) = delete;
	Base_Path& operator=(Base_Path&& _Other) noexcept = delete;

	// 상대 경로를 현재 소스 파일 기준으로 만들어줌
	static std::string MakePath(std::string_view RelativePath, std::string_view ThisFile = __FILE__)
	{
		std::filesystem::path CurrentPath(ThisFile.data()); // 현재 cpp 파일의 전체 경로
		std::filesystem::path BaseDir = CurrentPath.parent_path(); // 현재 파일의 디렉터리

		std::filesystem::path TargetPath = BaseDir / RelativePath.data();
		std::string Path = TargetPath.lexically_normal().string();

		if (!std::filesystem::exists(Path))
		{
			MsgAssert(Path += "존재하지 않는 경로로 이동하려고 했습니다.");
			return " ";
		}

		return Path;
	}

protected:
	
private:
	
};