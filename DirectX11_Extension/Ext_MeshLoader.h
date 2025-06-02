#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

// assimp ���̺귯�� ����� Ȱ���� Mesh ���� ������ �ҷ����� Ŭ����
class Ext_MeshLoader
{
public:
    Ext_MeshLoader();
    ~Ext_MeshLoader() {}

    Ext_MeshLoader(const Ext_MeshLoader&) = delete;
    Ext_MeshLoader(Ext_MeshLoader&&) noexcept = delete;
    Ext_MeshLoader& operator=(const Ext_MeshLoader&) = delete;
    Ext_MeshLoader& operator=(Ext_MeshLoader&&) noexcept = delete;

    
    
    bool LoadAnimation(const std::string& _FilePath); // DynamicMesh�� ����� Animation ��������

protected:

private:
    const aiScene* AIScene; // 

};