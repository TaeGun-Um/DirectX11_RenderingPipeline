#pragma once
#include "Ext_Actor.h"

// 원근인지, 직교인지 설정
enum class ProjectionType
{
	Perspective,
	Orthogonal,
	Unknown
};

// 화면의 렌더링을 담당하는 Actor
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

	void PushMeshComponent(std::shared_ptr<class Ext_MeshComponent> _MeshComponent); // 메시 컴포넌트를 MeshComponents에 저장
	void PushMeshComponentUnit(std::shared_ptr<class Ext_MeshComponentUnit> _Unit, RenderPath _Path = RenderPath::Unknown); // 메시 컴포넌트 유닛을 MeshComponentUnits에 저장

	const float4x4& GetViewMatrix() { return ViewMatrix; } // 뷰행렬 가져오기
	const float4x4& GetProjectionMatrix() { return ProjectionMatrix; } // 프로젝션 행렬 가져오기
	const float4x4& GetViewPortMatrix() { return ViewPortMatrix; } // 뷰포트 행렬 가져오기

	void SetCameraType(ProjectionType _CameraType) { CameraType = _CameraType; }

	bool IsCameraAcc() { return bIsCameraAcc; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	
private:
	void RemoveMeshByActor(std::shared_ptr<Ext_Actor> DeadActor);
	void CameraTransformUpdate(); // 카메라의 뷰, 프로젝션, 뷰포트 행렬 연산 실시
	void Rendering(float _Deltatime); // 카메라의 MeshComponents, MeshComponentUnits에 대한 렌더링 파이프라인 결과 적용
	
	void ViewPortSetting(); //////////////////////////////////// 테스트용

	std::map<int, std::vector<std::shared_ptr<class Ext_MeshComponent>>> MeshComponents; // 생성된 메시 컴포넌트들
	std::map<RenderPath, std::map<int, std::list<std::shared_ptr<class Ext_MeshComponentUnit>>>> MeshComponentUnits; // 생성된 메시 컴포넌트 유닛들
	ProjectionType CameraType = ProjectionType::Perspective;
	float4x4 ViewMatrix;			// 카메라 기준 뷰행렬
	float4x4 ProjectionMatrix;	// 카메라 기준 프로젝션 행렬
	float4x4 ViewPortMatrix;		// 카메라 기준 뷰포트 행렬
	float Width = 0.0f;				// 너비(모니터)
	float Height = 0.0f;				// 높이(모니터)
	float FOV = 90.0f;				// FOV값
	float Near = 0.1f;				// 근평면
	float Far = 10000.0f;		// 원평면

	//////////////////////////////////// 테스트용
	D3D11_VIEWPORT ViewPortData; 
	void bIsCameraSwitch() { bIsCameraAcc = !bIsCameraAcc; }
	bool bIsCameraAcc = false;           // Q를 눌러 자유 모드인지 아닌지
	bool bPrevCameraAcc = false;          // 이전 프레임의 bIsCameraAcc 값
	float AccTime = 0.f;
	float Yaw = 0.f;
	float Pitch = 0.f;

	float4 SavedPos = float4::ZERO;
	float4 SavedRot = float4::ZERO;

};