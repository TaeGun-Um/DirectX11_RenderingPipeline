#pragma once
#include "Ext_Actor.h"

// ȭ���� �������� ����ϴ� Actor
class Ext_Camera : public Ext_Actor
{
	friend class Ext_Scene;

public:
	// constrcuter destructer
	Ext_Camera() {}
	~Ext_Camera() {}

	// delete Function
	Ext_Camera(const Ext_Camera& _Other) = delete;
	Ext_Camera(Ext_Camera&& _Other) noexcept = delete;
	Ext_Camera& operator=(const Ext_Camera& _Other) = delete;
	Ext_Camera& operator=(Ext_Camera&& _Other) noexcept = delete;

	void PushMeshComponent(std::shared_ptr<class Ext_MeshComponent> _MeshComponent); // �޽� ������Ʈ�� MeshComponents�� ����
	void PushMeshComponentUnit(std::shared_ptr<class Ext_MeshComponentUnit> _Unit, RenderPath _Path = RenderPath::Unknown); // �޽� ������Ʈ ������ MeshComponentUnits�� ����

	const float4x4& GetViewMatrix() { return ViewMatrix; } // ����� ��������
	const float4x4& GetProjectionMatrix() { return ProjectionMatrix; } // �������� ��� ��������
	const float4x4& GetViewPortMatrix() { return ViewPortMatrix; } // ����Ʈ ��� ��������

	void SetCameraType(ProjectionType _CameraType) { CameraType = _CameraType; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override {}
	void Destroy() override {}
	
private:
	void CameraTransformUpdate(); // ī�޶��� ��, ��������, ����Ʈ ��� ���� �ǽ�
	void Rendering(float _Deltatime); // ī�޶��� MeshComponents, MeshComponentUnits�� ���� ������ ���������� ��� ����
	
	void ViewPortSetting(); //////////////////////////////////// �׽�Ʈ��

	std::map<int, std::vector<std::shared_ptr<class Ext_MeshComponent>>> MeshComponents; // ������ �޽� ������Ʈ��
	std::map<RenderPath, std::map<int, std::list<std::shared_ptr<class Ext_MeshComponentUnit>>>> MeshComponentUnits; // ������ �޽� ������Ʈ ���ֵ�
	ProjectionType CameraType = ProjectionType::Perspective;
	float4x4 ViewMatrix;			// ī�޶� ���� �����
	float4x4 ProjectionMatrix;	// ī�޶� ���� �������� ���
	float4x4 ViewPortMatrix;		// ī�޶� ���� ����Ʈ ���
	float Width = 0.0f;				// �ʺ�(�����)
	float Height = 0.0f;				// ����(�����)
	float FOV = 90.0f;				// FOV��
	float Near = 10.f;				// �����
	float Far = 100000.0f;		// �����
	
	D3D11_VIEWPORT ViewPortData; //////////////////////////////////// �׽�Ʈ��
};