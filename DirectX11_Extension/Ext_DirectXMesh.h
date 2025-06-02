#pragma once
#include "Ext_ResourceManager.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DynamicMesh의 Bone 정보
class Ext_SkeltalMesh
{
	friend class Ext_DirectXMesh;

public:
	struct BoneInfomation
	{
		int ID; // 0부터 시작하는 본 인덱스
		aiMatrix4x4 OffsetMatrix;  // aiBone->mOffsetMatrix: “모델 스페이스 → 본 로컬 스페이스”
	};

	// Getter
	int GetBoneCount() { return BoneCount; }
	const aiScene* GetMeshScene() { return MeshScene; }
	std::unordered_map<std::string, BoneInfomation>& GetBoneInfomations() { return BoneInfomations; }

private:
	std::unordered_map<std::string, BoneInfomation> BoneInfomations;
	int BoneCount = 0;

	Assimp::Importer MeshImporter; // 이거 없으면 밑에 깨짐(릴리즈됨)
	const aiScene* MeshScene = nullptr; // OwnerAIScene

};

// 만들어진 버텍스들의 정보를 저장하기 위한 클래스
class Ext_DirectXMesh : public Ext_ResourceManager<Ext_DirectXMesh>
{
	friend class Ext_MeshComponentUnit;

public:
	// constrcuter destructer
	Ext_DirectXMesh() {}
	~Ext_DirectXMesh() {}

	// delete Function
	Ext_DirectXMesh(const Ext_DirectXMesh& _Other) = delete;
	Ext_DirectXMesh(Ext_DirectXMesh&& _Other) noexcept = delete;
	Ext_DirectXMesh& operator=(const Ext_DirectXMesh& _Other) = delete;
	Ext_DirectXMesh& operator=(Ext_DirectXMesh&& _Other) noexcept = delete;

	// 버텍스 버퍼와 인덱스 버퍼 정보 입력 및 메시 생성
	static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name)
	{
		return CreateMesh(_Name, _Name, _Name);
	}

	// 버텍스 버퍼와 인덱스 버퍼 정보 입력 및 생성
	static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name, std::string_view _VBName, std::string_view _IBName)
	{
		std::string UpperName = Base_String::ToUpper(_Name);
		std::shared_ptr<Ext_DirectXMesh> NewMesh = Ext_ResourceManager::CreateNameResource(UpperName);
		NewMesh->VertexBufferPtr = Ext_DirectXVertexBuffer::Find(_VBName);
		NewMesh->IndexBufferPtr = Ext_DirectXIndexBuffer::Find(_IBName);

		if ((nullptr == NewMesh->VertexBufferPtr) || (nullptr == NewMesh->IndexBufferPtr))
		{
			MsgAssert("메시 생성 실패");
		}

		return NewMesh;
	}

	// StaticMesh 경로 넣어주면 알아서 생성 후 저장
	static std::shared_ptr<Ext_DirectXMesh> CreateStaticMesh(std::string_view _FilePath);

	// DynamicMesh 경로 넣어주면 알아서 생성 후 저장
	static std::shared_ptr<Ext_DirectXMesh> CreateDynamicMesh(std::string_view _FilePath);

	// Getter
	std::shared_ptr<class Ext_DirectXVertexBuffer> GetVertexBuffer() { return VertexBufferPtr; }
	std::shared_ptr<class Ext_DirectXIndexBuffer> GetIndexBuffer() { return IndexBufferPtr; }

	static std::shared_ptr<class Ext_SkeltalMesh> FindSkeletalMesh(std::string_view _MeshName)
	{
		std::string UpperName = Base_String::ToUpper(_MeshName);
		if (SkeletalMeshs.find(UpperName) == SkeletalMeshs.end())
		{
			MsgAssert("해당 메시는 스켈레톤이 없는 메시입니다.");
			return nullptr;
		}

		return SkeletalMeshs[UpperName];
	}

protected:
	
private:
	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::shared_ptr<class Ext_DirectXVertexBuffer> VertexBufferPtr; // 상수버퍼 데이터 저장용
	std::shared_ptr<class Ext_DirectXIndexBuffer> IndexBufferPtr; // 인덱스버퍼 데이터 저장용
	
	void MeshSetting(); // InputAssembler1(), InputAssembler2() 호출
	void InputAssembler1(); // 인풋어셈블러 1단계 실시, IASetVertexBuffers(), IASetPrimitiveTopology() 실시
	void InputAssembler2(); // 인풋어셈블러 2단계 실시, IndexBufferSetting() 호출

	// DynamicMesh를 로드할 때 저장했던 Bone 데이터를 여기에 저장, 나중에 Animator에서 찾아서 활용
	static std::map<std::string, std::shared_ptr<class Ext_SkeltalMesh>> SkeletalMeshs;
};