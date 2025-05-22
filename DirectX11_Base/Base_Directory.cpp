#include "PrecompileHeader.h"
#include "Base_Directory.h"
#include "Base_Debug.h"

// 경로 생성 및 내부 저장
std::string Base_Directory::MakePath(std::string_view _RelativePath, std::string_view _ThisFile /*= __FILE__*/)
{
	std::filesystem::path CurrentPath(_ThisFile);
	std::filesystem::path BaseDir = CurrentPath.parent_path();
	std::filesystem::path TargetPath = BaseDir / _RelativePath.data();

	Path = TargetPath.lexically_normal().string();

	if (!std::filesystem::exists(Path))
	{
		MsgAssert(Path + " 경로가 존재하지 않습니다.");
		Path.clear();
	}

	return Path;
}

// 여러 확장자 인자를 받아 해당 디렉터리의 파일 목록 반환
std::vector<std::string> Base_Directory::GetAllFile(std::initializer_list<std::string> _Extensions)
{
	std::vector<std::string> Result;

	if (Path.empty())
	{
		return Result;
	}

	for (const auto& Entry : std::filesystem::directory_iterator(Path))
	{
		if (!Entry.is_regular_file())
		{
			continue;
		}

		std::string Ext = Entry.path().extension().string();
		Ext.erase(0, 1);

		for (const std::string& ValidExt : _Extensions)
		{
			if (Ext == ValidExt)
			{
				Result.push_back(Entry.path().string());
				break;
			}
		}
	}

	return Result;
}