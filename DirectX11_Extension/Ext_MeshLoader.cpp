#include "PrecompileHeader.h"
#include "Ext_MeshLoader.h"

#include "Ext_DirectXVertexData.h"

// �ε� �� ���ؽ� ���� ������ ���Ϳ� �ε��� ���� ������ ���۸� �������ָ� ���� �Է�����, �װɷ� Create �ǽ��ϸ� ��
bool Ext_MeshLoader::LoadMeshFromFile(const std::string& _FilePath, std::vector<Ext_DirectXVertexData>& _OutVertices, std::vector<uint32_t>& _OutIndices)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(_FilePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == nullptr)
    {
        MessageBoxA(nullptr, Importer.GetErrorString(), "Assimp Error", MB_OK);
        return false;
    }

    ////////////////////////////// VertexBuffer�� ���� ������ �Է� �κ� //////////////////////////////
    aiMesh* Mesh = Scene->mMeshes[0]; // ���� �޽� ����

    for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
    {
        Ext_DirectXVertexData VertexData;
        VertexData.POSITION = float4(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
        VertexData.NORMAL = Mesh->HasNormals() ? float4(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z) : float4::ZERO;
        if (Mesh->HasTextureCoords(0))
        {
            //VertexData.TEXCOORD = float4(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y); // CW
            VertexData.TEXCOORD = float4(Mesh->mTextureCoords[0][i].x, 1.0f - Mesh->mTextureCoords[0][i].y); // CCW
        }
        else
        {
            VertexData.TEXCOORD = float4::ZERO;
        }

        _OutVertices.push_back(VertexData);
    }

    ////////////////////////////// IndexBuffer�� ���� ������ �Է� �κ� //////////////////////////////
    // CW
    //for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
    //{
    //    const aiFace& Face = Mesh->mFaces[i];
    //    for (unsigned int j = 0; j < Face.mNumIndices; ++j)
    //    {
    //        _OutIndices.push_back(Face.mIndices[j]);
    //    }
    //}

    // CCW
    for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
    {
        const aiFace& Face = Mesh->mFaces[i];

        if (Face.mNumIndices == 3) // �ﰢ���� ó��
        {
            // CW �� CCW�� ��ȯ (Face.mIndices[1], [2] ���� ����)
            _OutIndices.push_back(Face.mIndices[0]);
            _OutIndices.push_back(Face.mIndices[2]);
            _OutIndices.push_back(Face.mIndices[1]);
        }
        else
        {
            // �ﰢ���� �ƴ� ��� �״�� �ְų� ����
            for (unsigned int j = 0; j < Face.mNumIndices; ++j)
            {
                _OutIndices.push_back(Face.mIndices[j]);
            }
        }
    }

    return true;
}