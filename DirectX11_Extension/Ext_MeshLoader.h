#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Ext_MeshLoader
{
public:
	// constrcuter destructer
	Ext_MeshLoader() {}
	~Ext_MeshLoader() {}

	// delete Function
	Ext_MeshLoader(const Ext_MeshLoader& _Other) = delete;
	Ext_MeshLoader(Ext_MeshLoader&& _Other) noexcept = delete;
	Ext_MeshLoader& operator=(const Ext_MeshLoader& _Other) = delete;
	Ext_MeshLoader& operator=(Ext_MeshLoader&& _Other) noexcept = delete;

	// 메시 로드하기
	static bool LoadMeshFromFile(const std::string& _FilePath, std::vector<class Ext_DirectXVertexData>& _OutVertices, std::vector<uint32_t>& _OutIndices);

protected:
	
private:
	
};