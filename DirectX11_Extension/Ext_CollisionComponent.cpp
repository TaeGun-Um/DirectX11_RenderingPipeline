#include "PrecompileHeader.h"
#include "Ext_CollisionComponent.h"
#include "Ext_Scene.h"
#include "Ext_Transform.h"

void Ext_CollisionComponent::Start()
{
	__super::Start();
	GetOwnerScene().lock()->PushCollision(GetSharedFromThis<Ext_CollisionComponent>(), Order); // Collision은 Order를 꼭 지정해주도록 함
}

void Ext_CollisionComponent::Update(float _DeltaTime)
{
	ColData.OBB.Center = GetTransform()->GetTransformData()->WorldPosition.DirectFloat3;
	ColData.OBB.Extents = (GetTransform()->GetTransformData()->WorldScale * 0.5f).DirectFloat3;
	ColData.OBB.Orientation = GetTransform()->GetTransformData()->WorldQuaternion.DirectFloat4;
	ColData.ScaleABS();
}

// nullptr이 아니면 충돌한것
std::shared_ptr<Ext_CollisionComponent> Ext_CollisionComponent::Collision(int _TargetGroup, CollsionType _ThisColType, CollsionType _OtherColType)
{
	if (false == this->IsUpdate()) return nullptr;
	if (CollsionType::Unknown == _ThisColType)
	{
		_ThisColType = this->GetCollisionType();
	}

	std::vector<std::shared_ptr<Ext_CollisionComponent>>& CollisionGroup = *GetOwnerScene().lock()->FindCollsionGroup(_TargetGroup);
	for (std::shared_ptr<Ext_CollisionComponent>& TargetCollision : CollisionGroup)
	{
		if (false == TargetCollision->IsUpdate()) continue;
		if (CollsionType::Unknown == _OtherColType)
		{
			_ThisColType = TargetCollision->GetCollisionType();
		}

		if (CollisionCheck(GetSharedFromThis<Ext_CollisionComponent>(), TargetCollision))
		{
			return TargetCollision;
		}
	}

	return nullptr;
}

// 두 콜리전의 CollisionType에 따라 CollisionData를 가져와서 충돌
bool Ext_CollisionComponent::CollisionCheck(std::shared_ptr<Ext_CollisionComponent> _ThisCollision, std::shared_ptr<Ext_CollisionComponent> _OtherCollision)
{
	return CollisionFunction
		[static_cast<int>(_ThisCollision->GetCollisionType())]
		[static_cast<int>(_OtherCollision->GetCollisionType())]
		(_ThisCollision->GetCollisionData(), _OtherCollision->GetCollisionData());
}

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
	}
};

CollisionFunctionInitialize InitFunction;

bool Ext_CollisionComponent::SphereToSpehre(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.Sphere.Intersects(_Other.Sphere);
}

bool Ext_CollisionComponent::SphereToAABB(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.Sphere.Intersects(_Other.AABB);
}

bool Ext_CollisionComponent::SphereToOBB(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.Sphere.Intersects(_Other.OBB);
}

bool Ext_CollisionComponent::AABBToSpehre(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.AABB.Intersects(_Other.Sphere);
}

bool Ext_CollisionComponent::AABBToAABB(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.AABB.Intersects(_Other.AABB);
}

bool Ext_CollisionComponent::AABBToOBB(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.AABB.Intersects(_Other.OBB);
}

bool Ext_CollisionComponent::OBBToSpehre(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.OBB.Intersects(_Other.Sphere);
}

bool Ext_CollisionComponent::OBBToAABB(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.OBB.Intersects(_Other.AABB);
}

bool Ext_CollisionComponent::OBBToOBB(const CollisionData& _This, const CollisionData& _Other)
{
	return _This.OBB.Intersects(_Other.OBB);
}