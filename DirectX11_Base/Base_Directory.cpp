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

std::string Base_Directory::FindEntryPoint(std::string_view _FilePath)
{
	std::string FileName = std::filesystem::path(_FilePath).stem().string();

	std::ifstream File(_FilePath.data());
	if (!File.is_open())
	{
		MsgAssert(std::string("���� ���� ���� : ") + _FilePath.data());
		return "";
	}

	std::string Line;
	while (std::getline(File, Line))
	{
		// ���� ����
		Line.erase(remove_if(Line.begin(), Line.end(), isspace), Line.end());

		// "Basic_VS(" Ȥ�� "Basic_VS(" ���·� �Լ� ���� ã��
		std::string Signature = FileName + "(";

		if (Line.find(Signature) != std::string::npos)
		{
			return FileName;
		}
	}

	// �� ã���� �⺻��
	return "";
}
