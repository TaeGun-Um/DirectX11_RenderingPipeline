﻿#include "PrecompileHeader.h"
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

// 디렉터리 내에 파일들 검사 후 반환, 인자로 전달받은 이름으로 검색하여 vector에 담아 반환
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

// 파일명 가져오기
std::string Base_Directory::GetFileName()
{
	std::filesystem::path CurrentPath(Path);
	return CurrentPath.stem().string();
}

// Path 인자에서 파일명 가져오기
std::string Base_Directory::GetFileName(std::string_view _Path)
{
	std::filesystem::path CurrentPath(_Path);
	return CurrentPath.stem().string();
}

// 확장자명 가져오기
std::string Base_Directory::GetExtension()
{
	std::filesystem::path CurrentPath(Path);
	return CurrentPath.extension().string();
}

// Path 인자에서 파일 확장자명 가져오기
std::string Base_Directory::GetExtension(std::string_view _Path)
{
	std::filesystem::path CurrentPath(_Path);
	return CurrentPath.extension().string();
}

// 파일을 열어 특정 문자 검사, 라인별로 확인함(한줄씩)
std::string Base_Directory::FindEntryPoint(std::string_view _FilePath)
{
	std::string FileName = std::filesystem::path(_FilePath).stem().string();

	std::ifstream File(_FilePath.data());
	if (!File.is_open())
	{
		MsgAssert(std::string("파일 열기 실패 : ") + _FilePath.data());
		return "";
	}

	std::string Line;
	while (std::getline(File, Line))
	{
		// 공백 제거 (안전하게 처리)
		Line.erase(remove_if(Line.begin(), Line.end(),
			[](char c) { return std::isspace(static_cast<unsigned char>(c)); }),
			Line.end());

		std::string Signature = FileName + "(";
		if (Line.find(Signature) != std::string::npos)
		{
			return FileName;
		}
	}

	// 못 찾으면 기본값
	return "";
}

