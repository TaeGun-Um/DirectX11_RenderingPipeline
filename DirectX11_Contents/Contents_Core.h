#pragma once

class Contents_Core
{
public:
	// constrcuter destructer
	Contents_Core();
	~Contents_Core();

	// delete Function
	Contents_Core(const Contents_Core& _Other) = delete;
	Contents_Core(Contents_Core&& _Other) noexcept = delete;
	Contents_Core& operator=(const Contents_Core& _Other) = delete;
	Contents_Core& operator=(Contents_Core&& _Other) noexcept = delete;

	static void Start();
	static void End();

protected:
	
private:
	
};