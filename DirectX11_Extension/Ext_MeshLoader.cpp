#include "PrecompileHeader.h"
#include "Ext_MeshLoader.h"

#include "Ext_DirectXVertexData.h"

// 로드 시 버텍스 버퍼 생성용 벡터와 인덱스 버퍼 생성용 버퍼를 전달해주면 값을 입력해줌, 그걸로 Create 실시하면 됨
bool Ext_MeshLoader::LoadMeshFromFile(const std::string& _FilePath, std::vector<Ext_DirectXVertexData>& _OutVertices, std::vector<uint32_t>& _OutIndices)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(_FilePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == nullptr)
    {
        MessageBoxA(nullptr, Importer.GetErrorString(), "Mesh Import Error", MB_OK);
        return false;
    }

    ////////////////////////////// VertexBuffer를 위한 데이터 입력 부분 //////////////////////////////
    aiMesh* Mesh = Scene->mMeshes[0]; // 단일 메시 가정

    for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
    {
        Ext_DirectXVertexData VertexData;
        VertexData.POSITION = float4(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
        VertexData.NORMAL = Mesh->HasNormals() ? float4(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z) : float4::ZERO;
        if (Mesh->HasTextureCoords(0))
        {
            //VertexData.TEXCOORD = float4(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
            VertexData.TEXCOORD = float4(Mesh->mTextureCoords[0][i].x, 1.0f - Mesh->mTextureCoords[0][i].y); // CCW
        }
        else
        {
            VertexData.TEXCOORD = float4::ZERO;
        }

        _OutVertices.push_back(VertexData);
    }

    // CCW
    for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
    {
        const aiFace& Face = Mesh->mFaces[i];

        if (Face.mNumIndices == 3) // 삼각형만 처리
        {
            // CW → CCW로 전환 (Face.mIndices[1], [2] 순서 변경)
            _OutIndices.push_back(Face.mIndices[0]);
            _OutIndices.push_back(Face.mIndices[2]);
            _OutIndices.push_back(Face.mIndices[1]);
        }
        else
        {
            // 삼각형이 아닐 경우 그대로 넣거나 무시
            for (unsigned int j = 0; j < Face.mNumIndices; ++j)
            {
                _OutIndices.push_back(Face.mIndices[j]);
            }
        }
    }

    return true;
}
