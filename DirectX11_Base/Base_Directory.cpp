#include "PrecompileHeader.h"
#include "Base_Directory.h"
#include "Base_Debug.h"

// ��� ���� �� ���� ����
std::string Base_Directory::MakePath(std::string_view _RelativePath, std::string_view _ThisFile /*= __FILE__*/)
{
	std::filesystem::path CurrentPath(_ThisFile);
	std::filesystem::path BaseDir = CurrentPath.parent_path();
	std::filesystem::path TargetPath = BaseDir / _RelativePath.data();

	Path = TargetPath.lexically_normal().string();

	if (!std::filesystem::exists(Path))
	{
		MsgAssert(Path + " ��ΰ� �������� �ʽ��ϴ�.");
		Path.clear();
	}

	return Path;
}

// ���� Ȯ���� ���ڸ� �޾� �ش� ���͸��� ���� ��� ��ȯ
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