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
	GetOwnerScene().lock()->PushCollision(GetSharedFromThis<Ext_CollisionComponent>(), Order); // Collision�� Order�� �� �������ֵ��� ��
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

		// ��, CollisionFunction �� ������ 6��6 ũ��(�ε��� 05������ ��ȿ)�ε�, Unknown �� ��(=6)�� �״�� ũ��� �� ���ȱ� ������ "05"������ �����, 6��°(�ε��� 6)���� ���� ���°� �˴ϴ�.
		// �׷��� ��Ÿ�ӿ��� ThisType Ȥ�� OtherType �� Unknown(= 6) �� �Ǹ鼭, �ᱹ CollisionFunction[6][��] �̳� CollisionFunction[��][6] ó�� ����ȿ ����(0~5)�� �Ѿ �ε������� �����Ϸ��� �õ��մϴ�.
		// 	�̶� MSVC ���� �м����
		// 	������ �� �ִ� ��ü ũ��� 6��6��sizeof(��) = 384����Ʈ�ε�, �ε����� 6�� �� ������ �ϸ� �� ����Ʈ ����(384����Ʈ)�� �Ѿ �� �ִ١�
		// 	�� �Ǵ��ؼ� C6385 ���(CollisionFunction[��][��] ���� ��ȿ ������ �ʰ��ϴ� �бⰡ �߻��� �� �ִ�) �� ���� �ִ� ���Դϴ�.

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