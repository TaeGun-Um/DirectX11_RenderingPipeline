#pragma once

class DirectXResourceLoader
{
public:
	// constrcuter destructer
	DirectXResourceLoader();
	~DirectXResourceLoader();

	// delete Function
	DirectXResourceLoader(const DirectXResourceLoader& _Other) = delete;
	DirectXResourceLoader(DirectXResourceLoader&& _Other) noexcept = delete;
	DirectXResourceLoader& operator=(const DirectXResourceLoader& _Other) = delete;
	DirectXResourceLoader& operator=(DirectXResourceLoader&& _Other) noexcept = delete;

protected:
	
private:
	
};