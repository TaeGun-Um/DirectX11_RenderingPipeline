#pragma once

class Base_Path
{
public:
	// constrcuter destructer
	Base_Path();
	~Base_Path();

	// delete Function
	Base_Path(const Base_Path& _Other) = delete;
	Base_Path(Base_Path&& _Other) noexcept = delete;
	Base_Path& operator=(const Base_Path& _Other) = delete;
	Base_Path& operator=(Base_Path&& _Other) noexcept = delete;

protected:
	
private:
	
};