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

	// ��� ��θ� ���� �ҽ� ���� �������� �������
	static std::string MakePath(std::string_view RelativePath, std::string_view ThisFile = __FILE__)
	{
		std::filesystem::path CurrentPath(ThisFile.data()); // ���� cpp ������ ��ü ���
		std::filesystem::path BaseDir = CurrentPath.parent_path(); // ���� ������ ���͸�

		std::filesystem::path TargetPath = BaseDir / RelativePath.data();
		std::string Path = TargetPath.lexically_normal().string();

		if (!std::filesystem::exists(Path))
		{
			MsgAssert(Path += "�������� �ʴ� ��η� �̵��Ϸ��� �߽��ϴ�.");
			return " ";
		}

		return Path;
	}

protected:
	
private:
	
};