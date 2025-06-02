#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

// assimp 라이브러리 기능을 활용한 Mesh 파일 데이터 불러오는 클래스
class Ext_MeshLoader
{
public:
    Ext_MeshLoader();
    ~Ext_MeshLoader() {}

    Ext_MeshLoader(const Ext_MeshLoader&) = delete;
    Ext_MeshLoader(Ext_MeshLoader&&) noexcept = delete;
    Ext_MeshLoader& operator=(const Ext_MeshLoader&) = delete;
    Ext_MeshLoader& operator=(Ext_MeshLoader&&) noexcept = delete;

    
    
    bool LoadAnimation(const std::string& _FilePath); // DynamicMesh에 사용할 Animation 가져오기

protected:

private:
    const aiScene* AIScene; // 

};