#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Ext_MeshLoader
{
public:
	// constrcuter destructer
	Ext_MeshLoader();
	~Ext_MeshLoader();

	// delete Function
	Ext_MeshLoader(const Ext_MeshLoader& _Other) = delete;
	Ext_MeshLoader(Ext_MeshLoader&& _Other) noexcept = delete;
	Ext_MeshLoader& operator=(const Ext_MeshLoader& _Other) = delete;
	Ext_MeshLoader& operator=(Ext_MeshLoader&& _Other) noexcept = delete;

    static void LoadModel(const std::string& FilePath)
    {
        Assimp::Importer Importer;

        const aiScene* Scene = Importer.ReadFile(
            FilePath,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices
        );

        if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
        {
            std::cout << "Assimp Error: " << Importer.GetErrorString() << std::endl;
            return;
        }

        std::cout << "Model Loaded: " << FilePath << std::endl;
    }

protected:
	
private:
	
};