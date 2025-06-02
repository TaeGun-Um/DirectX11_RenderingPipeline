#include "PrecompileHeader.h"
#include "Ext_CollisionComponent.h"
#include "Ext_Scene.h"
#include "Ext_Transform.h"

std::function<bool(const CollisionData&, const CollisionData&)> Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Unknown)][static_cast<int>(CollsionType::Unknown)];

class CollisionFunctionInitialize
{
public:
	CollisionFunctionInitialize()
	{
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Sphere3D)][static_cast<int>(CollsionType::Sphere3D)] = &Ext_CollisionComponent::SphereToSpehre;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Sphere3D)][static_cast<int>(CollsionType::AABB3D)] = &Ext_CollisionComponent::SphereToAABB;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Sphere3D)][static_cast<int>(CollsionType::OBB3D)] = &Ext_CollisionComponent::SphereToOBB;

		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::AABB3D)][static_cast<int>(CollsionType::Sphere3D)] = &Ext_CollisionComponent::AABBToSpehre;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::AABB3D)][static_cast<int>(CollsionType::AABB3D)] = &Ext_CollisionComponent::AABBToAABB;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::AABB3D)][static_cast<int>(CollsionType::OBB3D)] = &Ext_CollisionComponent::AABBToOBB;

		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::OBB3D)][static_cast<int>(CollsionType::Sphere3D)] = &Ext_CollisionComponent::OBBToSpehre;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::OBB3D)][static_cast<int>(CollsionType::AABB3D)] = &Ext_CollisionComponent::OBBToAABB;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::OBB3D)][static_cast<int>(CollsionType::OBB3D)] = &Ext_CollisionComponent::OBBToOBB;

		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Sphere2D)][static_cast<int>(CollsionType::Sphere2D)] = &Ext_CollisionComponent::Sphere2DToSpehre2D;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Sphere2D)][static_cast<int>(CollsionType::AABB2D)] = &Ext_CollisionComponent::Sphere2DToAABB2D;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::Sphere2D)][static_cast<int>(CollsionType::OBB2D)] = &Ext_CollisionComponent::Sphere2DToOBB2D;

		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::AABB2D)][static_cast<int>(CollsionType::Sphere2D)] = &Ext_CollisionComponent::AABB2DToSpehre2D;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::AABB2D)][static_cast<int>(CollsionType::AABB2D)] = &Ext_CollisionComponent::AABB2DToAABB2D;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::AABB2D)][static_cast<int>(CollsionType::OBB2D)] = &Ext_CollisionComponent::AABB2DToOBB2D;

		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::OBB2D)][static_cast<int>(CollsionType::Sphere2D)] = &Ext_CollisionComponent::OBB2DToSpehre2D;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::OBB2D)][static_cast<int>(CollsionType::AABB2D)] = &Ext_CollisionComponent::OBB2DToAABB2D;
		Ext_CollisionComponent::CollisionFunction[static_cast<int>(CollsionType::OBB2D)][static_cast<int>(CollsionType::OBB2D)] = &Ext_CollisionComponent::OBB2DToOBB2D;
	}
};

CollisionFunctionInitialize InitFunction;

void Ext_CollisionComponent::Start()
{
	__super::Start();
	GetOwnerScene().lock()->PushCollision(GetSharedFromThis<Ext_CollisionComponent>(), Order); // Collision은 Order를 꼭 지정해주도록 함
}

std::shared_ptr<Ext_CollisionComponent> Ext_CollisionComponent::CollisionCheck(int _TargetGroup, CollsionType _ThisColType, CollsionType _OtherColType)
{
	if (false == this->IsUpdate()) return nullptr;
	if (_ThisColType == CollsionType::Unknown) _ThisColType = ColType;

	std::vector<std::shared_ptr<Ext_CollisionComponent>>& CollisionGroup = *GetOwnerScene().lock()->FindCollsionGroup(_TargetGroup);
	for (std::shared_ptr<Ext_CollisionComponent>& TargetCollision : CollisionGroup)
	{
		if (false == TargetCollision->IsUpdate()) continue;
		if (_OtherColType == CollsionType::Unknown) _OtherColType = TargetCollision->ColType;

		CollisionParameter CheckParameter;
		CheckParameter.ThisType = _ThisColType;
		CheckParameter.OtherType = _OtherColType;

		// 즉, CollisionFunction 은 실제로 6×6 크기(인덱스 05까지만 유효)인데, Unknown 의 값(=6)을 그대로 크기로 써 버렸기 때문에 "05"까지만 만들고, 6번째(인덱스 6)까진 없는 상태가 됩니다.
		// 그런데 런타임에는 ThisType 혹은 OtherType 이 Unknown(= 6) 이 되면서, 결국 CollisionFunction[6][…] 이나 CollisionFunction[…][6] 처럼 “유효 범위(0~5)를 넘어선 인덱스”로 접근하려고 시도합니다.
		// 	이때 MSVC 정적 분석기는
		// 	“읽을 수 있는 전체 크기는 6×6×sizeof(…) = 384바이트인데, 인덱스가 6일 때 읽으려 하면 그 바이트 범위(384바이트)를 넘어갈 수 있다”
		// 	고 판단해서 C6385 경고(CollisionFunction[…][…] 에서 유효 범위를 초과하는 읽기가 발생할 수 있다) 를 내어 주는 것입니다.

		CollisionFunction[static_cast<int>(CheckParameter.ThisType)][static_cast<int>(CheckParameter.OtherType)](this->GetCollisionData(), TargetCollision->GetCollisionData());
	}

	return nullptr;
}

bool Ext_CollisionComponent::SphereToSpehre(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.Sphere.Intersects(_Right.Sphere);
}

bool Ext_CollisionComponent::SphereToAABB(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.Sphere.Intersects(_Right.AABB);
}

bool Ext_CollisionComponent::SphereToOBB(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.Sphere.Intersects(_Right.OBB);
}

bool Ext_CollisionComponent::AABBToSpehre(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.AABB.Intersects(_Right.Sphere);
}

bool Ext_CollisionComponent::AABBToAABB(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.AABB.Intersects(_Right.AABB);
}

bool Ext_CollisionComponent::AABBToOBB(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.AABB.Intersects(_Right.OBB);
}

bool Ext_CollisionComponent::OBBToSpehre(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.OBB.Intersects(_Right.Sphere);
}

bool Ext_CollisionComponent::OBBToAABB(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.OBB.Intersects(_Right.AABB);
}

bool Ext_CollisionComponent::OBBToOBB(const CollisionData& _Left, const CollisionData& _Right)
{
	return _Left.OBB.Intersects(_Right.OBB);
}

bool Ext_CollisionComponent::Sphere2DToSpehre2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.Sphere.Intersects(RightData.Sphere);
}

bool Ext_CollisionComponent::Sphere2DToAABB2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.Sphere.Intersects(RightData.AABB);
}

bool Ext_CollisionComponent::Sphere2DToOBB2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.Sphere.Intersects(RightData.OBB);
}

bool Ext_CollisionComponent::AABB2DToSpehre2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.AABB.Intersects(RightData.Sphere);
}

bool Ext_CollisionComponent::AABB2DToAABB2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.AABB.Intersects(RightData.AABB);
}

bool Ext_CollisionComponent::AABB2DToOBB2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.AABB.Intersects(RightData.OBB);
}

bool Ext_CollisionComponent::OBB2DToSpehre2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.OBB.Intersects(RightData.Sphere);
}

bool Ext_CollisionComponent::OBB2DToAABB2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.OBB.Intersects(RightData.AABB);
}

bool Ext_CollisionComponent::OBB2DToOBB2D(const CollisionData& _Left, const CollisionData& _Right)
{
	CollisionData LeftData = _Left;
	LeftData.OBB.Center.z = 0.0f;
	CollisionData RightData = _Right;
	RightData.OBB.Center.z = 0.0f;

	return LeftData.OBB.Intersects(RightData.OBB);
}