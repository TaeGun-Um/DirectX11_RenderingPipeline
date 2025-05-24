#include "PrecompileHeader.h"
#include "Ext_Scene.h"

#include "Ext_Actor.h"
#include "Ext_MeshComponent.h"
#include "Ext_Camera.h"

#include "Ext_DirectXResourceLoader.h" // �ӽ�
#include "Ext_DirectXDevice.h" // �ӽ�

// Actor ���� �� �ڵ� ȣ��(�̸� ����, ���� �� ����, ���� ����)
void Ext_Scene::ActorInitialize(std::shared_ptr<Ext_Actor> _Actor, std::weak_ptr<Ext_Scene> _Level, std::string_view _Name, int _Order /*= 0*/)
{
	_Actor->SetName(_Name);
	_Actor->SetOwnerScene(_Level);
	_Actor->SetOrder(_Order);
	_Actor->Start();
}

// ������ ī�޶� MeshComponent ����
void Ext_Scene::PushMeshToCamera(std::shared_ptr<Ext_MeshComponent> _MeshComponent, std::string_view _CameraName)
{
	std::shared_ptr<Ext_Camera> FindCam = FindCamera(_CameraName);

	if (nullptr == FindCam)
	{
		MsgAssert("�������� �ʴ� ī�޶� �������� �������� �����ϴ�.");
		return;
	}

	FindCam->PushMeshComponent(_MeshComponent);
}

// �̸����� ī�޶� ã��
std::shared_ptr<Ext_Camera> Ext_Scene::FindCamera(std::string_view _CameraName)
{
	std::map<std::string, std::shared_ptr<Ext_Camera>>::iterator FindIter = Cameras.find(_CameraName.data());

	if (FindIter == Cameras.end())
	{
		return nullptr;
	}

	std::shared_ptr<Ext_Camera> FindCam = FindIter->second;

	return FindCam;
}

void Ext_Scene::SceneChangeInitialize()
{
	// Scene ���� �� ȣ��
}

void Ext_Scene::SceneChangeEnd()
{
	// Scene ���� �� ȣ��
}

void Ext_Scene::Start()
{
	// Scene ���� �� ȣ��
}

// Actors�� Actor���� Update ȣ��
void Ext_Scene::Update(float _DeltaTime)
{
	for (auto& [Key, ActorList] : Actors)
	{
		for (const std::shared_ptr<Ext_Actor>& CurActor : ActorList)
		{
			if (!CurActor->GetIsUpdate()) continue;
			else
			{
				CurActor->Update(_DeltaTime);
			}
		}
	}
}

// Camera���� Rendering ȣ��
void Ext_Scene::Rendering(float _DeltaTime)
{
	// Rendering ������Ʈ
	for (auto& Iter : Cameras)
	{
		std::shared_ptr<Ext_Camera> CurCamera = Iter.second;

		CurCamera->CameraTransformUpdate(); // ī�޶� ���� ��, ��������, ����Ʈ ��� �ֽ�ȭ
		CurCamera->ViewPortSetting(); // ����Ʈ ����

		// �ӽ�
		{
			Ext_DirectXDevice::GetContext()->RSSetState(Ext_DirectXResourceLoader::RasterState);
		}

		CurCamera->Rendering(_DeltaTime);
	}
}