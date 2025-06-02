#include "PrecompileHeader.h"
#include "Ext_DirectXMesh.h"
#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXVertexData.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXDevice.h"

std::map<std::string, std::shared_ptr<class Ext_SkeltalMesh>> Ext_DirectXMesh::SkeletalMeshs;

void Ext_DirectXMesh::MeshSetting()
{
	InputAssembler1();
	InputAssembler2();
}

// 정점 버퍼 / 도형 종류 설정
void Ext_DirectXMesh::InputAssembler1()
{
	if (nullptr == VertexBufferPtr)
	{
		MsgAssert("버텍스 버퍼가 존재하지 않아서 인풋어셈블러1 과정을 실행할 수 없습니다.");
		return;
	}

	VertexBufferPtr->VertexBufferSetting(); // 정점 버퍼 설정
 
	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(Topology); // 도형 종류 설정
	// GPU에 삼각형 리스트, 라인 스트립, 포인트 리스트 등을 설정
	// 정점들을 삼각형으로 해석하도록 하기 위해 D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST 전달
}

// 인덱스 버퍼 설정
void Ext_DirectXMesh::InputAssembler2()
{
	// 그리는 순서에 대한 데이터를 넣어준다 // 012023
	if (nullptr == IndexBufferPtr)
	{
		MsgAssert("인덱스 버퍼가 존재하지 않아서 인풋 어셈블러2 과정을 실행할 수 없습니다.");
		return;
	}

	IndexBufferPtr->IndexBufferSetting(); // 인덱스 버퍼 설정
}

// assimp 라이브러리를 활용하여 SaticMesh 로드하기
std::shared_ptr<Ext_DirectXMesh> Ext_DirectXMesh::CreateStaticMesh(std::string_view _FilePath)
{
    Assimp::Importer Importer;
    const aiScene* AIScene = Importer.ReadFile(_FilePath.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (AIScene == nullptr || AIScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || AIScene->mRootNode == nullptr)
    {
        MessageBoxA(nullptr, Importer.GetErrorString(), "Static Mesh Import Error", MB_OK);
        return nullptr;
    }

    ////////////////////////////////////// 메시로드 시, 파일 안에는 하나의 메시만 존재한다고 가정
    const aiMesh* AIMesh = AIScene->mMeshes[0];
    std::vector<Ext_DirectXVertexData> Vertices;
    std::vector<UINT> Indices;

    ////////////////////////////////////// 정점 정보 채우기 : POSITION, COLOR, TEXCOORD, NORMAL
    for (unsigned int i = 0; i < AIMesh->mNumVertices; ++i)
    {
        Ext_DirectXVertexData VertexData;
        VertexData.POSITION = float4(AIMesh->mVertices[i].x, AIMesh->mVertices[i].y, AIMesh->mVertices[i].z);
        VertexData.NORMAL = AIMesh->HasNormals() ? float4(AIMesh->mNormals[i].x, AIMesh->mNormals[i].y, AIMesh->mNormals[i].z) : float4::ZERO;
        if (AIMesh->HasTextureCoords(0))
        {
            //VertexData.TEXCOORD = float4(AIMesh->mTextureCoords[0][i].x, AIMesh->mTextureCoords[0][i].y);
            VertexData.TEXCOORD = float4(AIMesh->mTextureCoords[0][i].x, 1.0f - AIMesh->mTextureCoords[0][i].y); // CCW
        }
        else
        {
            VertexData.TEXCOORD = float4::ZERO;
        }

        Vertices.push_back(VertexData);
    }

    ////////////////////////////////////// 인덱스 정보 채우기
    for (unsigned int i = 0; i < AIMesh->mNumFaces; ++i)
    {
        const aiFace& Face = AIMesh->mFaces[i];

        if (Face.mNumIndices == 3) // 삼각형만 처리
        {
            // CW → CCW로 전환 (Face.mIndices[1], [2] 순서 변경)
            Indices.push_back(Face.mIndices[0]);
            Indices.push_back(Face.mIndices[2]);
            Indices.push_back(Face.mIndices[1]);
        }
        else
        {
            // 삼각형이 아닐 경우 그대로 넣거나 무시
            for (unsigned int j = 0; j < Face.mNumIndices; ++j)
            {
                Indices.push_back(Face.mIndices[j]);
            }
        }
    }

    std::string FileName = Base_Directory::GetFileName(_FilePath);

    Ext_DirectXVertexBuffer::CreateVertexBuffer(FileName, Vertices);
    Ext_DirectXIndexBuffer::CreateIndexBuffer(FileName, Indices);

	return CreateMesh(FileName.c_str());
}

// assimp 라이브러리를 활용하여 DynamicMesh 로드하기
std::shared_ptr<Ext_DirectXMesh>Ext_DirectXMesh::CreateDynamicMesh(std::string_view _FilePath)
{
    ////////////////////////////////////// Bone 정보 추출하기
    std::shared_ptr<Ext_SkeltalMesh> NewSkeleton = std::make_shared<Ext_SkeltalMesh>();
    
    const aiScene* AIScene = NewSkeleton->MeshImporter.ReadFile(_FilePath.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights);

    if (nullptr == AIScene || AIScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || nullptr == AIScene->mRootNode)
    {
        MessageBoxA(nullptr, NewSkeleton->MeshImporter.GetErrorString(), "Dynamic Mesh Import Error", MB_OK);
        return nullptr;
    }

    ////////////////////////////////////// 메시로드 시, 파일 안에는 하나의 메시만 존재한다고 가정
    const aiMesh* AIMesh = AIScene->mMeshes[0];
    std::vector<Ext_DirectXVertexData> Vertices;
    std::vector<UINT> Indices;

    Vertices.reserve(AIMesh->mNumVertices);
    Indices.reserve(AIMesh->mNumFaces * 3);

    ////////////////////////////////////// 정점 정보 채우기: POSITION, COLOR, TEXCOORD, NORMAL
    for (unsigned int i = 0; i < AIMesh->mNumVertices; ++i)
    {
        Ext_DirectXVertexData VertexData;
        VertexData.POSITION = float4(AIMesh->mVertices[i].x, AIMesh->mVertices[i].y, AIMesh->mVertices[i].z);
        VertexData.NORMAL = AIMesh->HasNormals() ? float4(AIMesh->mNormals[i].x, AIMesh->mNormals[i].y, AIMesh->mNormals[i].z) : float4::ZERO;
        if (AIMesh->HasTextureCoords(0))
        {
            //VertexData.TEXCOORD = float4(AIMesh->mTextureCoords[0][i].x, AIMesh->mTextureCoords[0][i].y);
            VertexData.TEXCOORD = float4(AIMesh->mTextureCoords[0][i].x, 1.0f - AIMesh->mTextureCoords[0][i].y); // CCW
        }
        else
        {
            VertexData.TEXCOORD = float4::ZERO;
        }

        Vertices.push_back(VertexData);
    }

    if (!AIMesh->HasBones())
    {
        MsgAssert("본이 없는 다이나믹메시는 이 방식으로 로드할 수 없습니다.");
        return nullptr;
    }

    // (혹시) 겹치는 이름이 있으면 걸러줌
    std::set<std::string> uniqueBoneNames;
    for (unsigned int b = 0; b < AIMesh->mNumBones; ++b)
    {
        uniqueBoneNames.insert(AIMesh->mBones[b]->mName.C_Str());
    }

    // 수집된 고유 Bone 이름을 순서대로 BoneID 부여 및 OffsetMatrix 저장
    for (auto& boneName : uniqueBoneNames)
    {
        // 첫 번째로 등장하는 aiBone에서 OffsetMatrix 가져오기
        aiMatrix4x4 offset;
        bool found = false;
        for (unsigned int m = 0; m < AIScene->mNumMeshes && !found; ++m)
        {
            aiMesh* mesh = AIScene->mMeshes[m];
            if (!mesh->HasBones()) continue;

            for (unsigned int b = 0; b < mesh->mNumBones; ++b)
            {
                if (boneName == mesh->mBones[b]->mName.C_Str())
                {
                    offset = mesh->mBones[b]->mOffsetMatrix;
                    found = true;
                    break;
                }
            }
        }

        Ext_SkeltalMesh::BoneInfomation BInf;
        BInf.ID = NewSkeleton->BoneCount;
        BInf.OffsetMatrix = offset; // 모델공간 → 본공간
        NewSkeleton->BoneInfomations[boneName] = BInf;

        ++NewSkeleton->BoneCount;
    }

    ////////////////////////////////////// 정점 정보 채우기: BONEID, WEIGHT
    for (unsigned int b = 0; b < AIMesh->mNumBones; ++b)
    {
        const aiBone* AibonePtr = AIMesh->mBones[b];
        std::string    BoneName(AibonePtr->mName.C_Str());

        auto It = NewSkeleton->BoneInfomations.find(BoneName);
        if (It == NewSkeleton->BoneInfomations.end()) continue;

        int BoneID = It->second.ID;
        for (unsigned int w = 0; w < AibonePtr->mNumWeights; ++w)
        {
            unsigned int VertID = AibonePtr->mWeights[w].mVertexId;
            float         Weight = AibonePtr->mWeights[w].mWeight;
            Ext_DirectXVertexData& DestV = Vertices[VertID];

            // 네 슬롯(0~3) 중 빈 슬롯에 BoneID/Weight 저장
            for (int k = 0; k < 4; ++k)
            {
                bool IsEmpty = false;
                switch (k)
                {
                case 0:  IsEmpty = (DestV.WEIGHT.x == 0.0f); break;
                case 1:  IsEmpty = (DestV.WEIGHT.y == 0.0f); break;
                case 2:  IsEmpty = (DestV.WEIGHT.z == 0.0f); break;
                case 3:  IsEmpty = (DestV.WEIGHT.w == 0.0f); break;
                }
                if (IsEmpty)
                {
                    switch (k)
                    {
                    case 0:
                        DestV.BONEID.x = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.x = Weight;
                        break;
                    case 1:
                        DestV.BONEID.y = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.y = Weight;
                        break;
                    case 2:
                        DestV.BONEID.z = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.z = Weight;
                        break;
                    case 3:
                        DestV.BONEID.w = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.w = Weight;
                        break;
                    }
                    break;
                }
            }
        }
    }

    ////////////////////////////////////// 인덱스 정보 채우기
    for (unsigned int i = 0; i < AIMesh->mNumFaces; ++i)
    {
        const aiFace& Face = AIMesh->mFaces[i];

        if (Face.mNumIndices == 3) // 삼각형만 처리
        {
            // CW → CCW로 전환 (Face.mIndices[1], [2] 순서 변경)
            Indices.push_back(Face.mIndices[0]);
            Indices.push_back(Face.mIndices[2]);
            Indices.push_back(Face.mIndices[1]);
        }
        else
        {
            // 삼각형이 아닐 경우 그대로 넣거나 무시
            for (unsigned int j = 0; j < Face.mNumIndices; ++j)
            {
                Indices.push_back(Face.mIndices[j]);
            }
        }
    }

    std::string FileName = Base_Directory::GetFileName(_FilePath);
    std::string UpperName = Base_String::ToUpper(FileName);

    Ext_DirectXVertexBuffer::CreateVertexBuffer(FileName, Vertices);
    Ext_DirectXIndexBuffer::CreateIndexBuffer(FileName, Indices);

    NewSkeleton->MeshScene = AIScene;
    SkeletalMeshs[UpperName] = NewSkeleton;

    return CreateMesh(FileName.c_str());
}