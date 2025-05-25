#pragma once

// 행렬 정보(정점 셰이더랑 동일한 크기로 설정해야함)
struct TransformData
{
	// ✅ 로컬 정보
	float4 LocalPosition = float4::ZERO;
	float4 LocalRotation = float4::ZERONULL;
	float4 LocalScale = float4::ONE;
	float4 LocalQuaternion = float4::ZERONULL;
	float4x4 LocalMatrix;

	// ✅ 월드 정보 (최종 렌더링에 사용됨)
	float4 WorldPosition = float4::ZERO;
	float4 WorldRotation = float4::ZERONULL;
	float4 WorldQuaternion = float4::ZERONULL;
	float4 WorldScale = float4::ONE;
	float4x4 WorldMatrix;

	// 뷰, 프로젝션, 뷰포트
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 WorldViewMatrix;
	float4x4 WorldViewProjectionMatrix;
	
	void CalculateLocalMatrix();
	void CalculateWorldMatrix(const float4x4& _ParentMatrix);
	void SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection);

};

// 기하학 구조를 위한 클래스, World는 Scene 전체 데카르트좌표계 기준, Local은 자신이 속한 부모 기준의 위치/회전/스케일
class Ext_Transform : public std::enable_shared_from_this<Ext_Transform>
{
	friend class Ext_Camera;
	friend class Ext_MeshComponent;
	friend class Ext_Component;
	friend class Ext_Actor;

public:
	// constrcuter destructer
	Ext_Transform();
	~Ext_Transform() {}

	// delete Function
	Ext_Transform(const Ext_Transform& _Other) = delete;
	Ext_Transform(Ext_Transform&& _Other) noexcept = delete;
	Ext_Transform& operator=(const Ext_Transform& _Other) = delete;
	Ext_Transform& operator=(Ext_Transform&& _Other) noexcept = delete;

	// this 가져오기
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}

	void SetLocalPosition(const float4& _Value)
	{
		TFData->LocalPosition = _Value;
		TransformUpdate();
	}

	void SetLocalRotation(const float4& _Value)
	{
		TFData->LocalRotation = _Value;
		TransformUpdate();
	}

	void SetLocalScale(const float4& _Value)
	{
		TFData->LocalScale = _Value;
		TransformUpdate();
	}

	void AddLocalRotation(const float4& _Value)
	{
		TFData->LocalRotation += _Value;
		TransformUpdate();
	}

	void AddLocalPosition(const float4& _Value)
	{
		TFData->LocalPosition += _Value;
		TransformUpdate();
	}

	float4 GetLocalPosition() const { return TFData->LocalPosition; }
	float4 GetLocalRotation() const { return TFData->LocalRotation; }
	float4 GetLocalScale() const { return TFData->LocalScale; }

	// 월드 Getter
	float4 GetWorldPosition() const { return TFData->WorldPosition; }
	float4 GetWorldRotation() const { return TFData->WorldRotation; }
	float4 GetWorldScale() const { return TFData->WorldScale; }
	float4x4 GetWorldMatrix() const { return TFData->WorldMatrix; }
	std::shared_ptr<TransformData> GetTransformData() { return TFData; }

	float4 GetLocalForwardVector() { return TFData->WorldMatrix.ArrVector[2].NormalizeReturn(); }
	float4 GetLocalUpVector() { return TFData->WorldMatrix.ArrVector[1].NormalizeReturn(); }
	float4 GetLocalRightVector() { return TFData->WorldMatrix.ArrVector[0].NormalizeReturn(); }

	/////////////////////////////// 부모자식
	void SetParent(const std::shared_ptr<Ext_Transform>& InParent)
	{
		// [1] 기존 부모가 있다면 자신을 자식 목록에서 제거
		if (auto OldParent = Parent.lock())
		{
			OldParent->RemoveChild(GetSharedFromThis<Ext_Transform>());
		}

		// [2] 새로운 부모 등록
		Parent = InParent;

		// [3] 부모가 유효하면, 자신을 자식 목록에 추가
		if (InParent)
		{
			InParent->Children.push_back(GetSharedFromThis<Ext_Transform>());
		}

		// [4] 부모 변경되었으므로 위치 갱신
		TransformUpdate();
	}

	void RemoveChild(const std::shared_ptr<Ext_Transform>& Child)
	{
		Children.erase(std::remove(Children.begin(), Children.end(), Child), Children.end());
	}

	const std::vector<std::shared_ptr<Ext_Transform>>& GetChildren() const
	{
		return Children;
	}

	void SetOwnerComponent(const std::shared_ptr<Ext_Component>& Comp)
	{
		OwnerComponent = Comp;
	}

	std::weak_ptr<Ext_Component> GetOwnerComponent() const
	{
		return OwnerComponent;
	}

protected:
	
private:
	void SetCameraMatrix(const float4x4& _View, const float4x4& _Projection) { TFData->SetViewProjectionMatrix(_View, _Projection); } // 행렬 월드, 뷰, 프로젝션 적용
	void TransformUpdate(); // 행렬 업데이트
	void Destroy();

	std::shared_ptr<TransformData> TFData = nullptr; // TransformData 정보

	////////////////////////////////// 부모자식 테스트중
	std::weak_ptr<Ext_Transform> Parent;
	std::vector<std::shared_ptr<Ext_Transform>> Children;
	std::weak_ptr<Ext_Component> OwnerComponent;

};