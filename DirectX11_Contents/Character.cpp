#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Base/Base_Math.h>

void Character::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });

	MeshComp = CreateComponent<Ext_MeshComponent>("BodyMesh");
	MeshComp->CreateMeshComponentUnit("Belorian Soldier LP", "PBR");
	std::vector<std::string> TextureNames =
	{
		"Belorian_Soldier_LP_Armor_BaseColor.png",
		"Belorian_Soldier_LP_Armor_Normal.png",
		"Belorian_Soldier_LP_Armor_Roughness.png",
		"Belorian_Soldier_LP_Armor_Metallic.png",
		"Belorian_Soldier_LP_Armor_Emissive.png"
		//"Belorian_Soldier_LP_Body_BaseColor.png",
		//"Belorian_Soldier_LP_Body_Normal.png",
		//"Belorian_Soldier_LP_Body_Roughness.png",
		//"Belorian_Soldier_LP_Body_Metallic.png",
		//"Belorian_Soldier_LP_Body_Emissive.png",
		//"Belorian_Soldier_LP_Box_BaseColor.png",
		//"Belorian_Soldier_LP_Box_Normal.png",
		//"Belorian_Soldier_LP_Box_Roughness.png",
		//"Belorian_Soldier_LP_Box_Metallic.png",
		//"Belorian_Soldier_LP_Box_Emissive.png"
		//"Belorian_Soldier_LP_Device_BaseColor.png",
		//"Belorian_Soldier_LP_Device_Normal.png",
		//"Belorian_Soldier_LP_Device_Roughness.png",
		//"Belorian_Soldier_LP_Device_Metallic.png",
		//"Belorian_Soldier_LP_Device_Emissive.png"
		// "Belorian_Soldier_LP_Strap_BaseColor.png",
		// "Belorian_Soldier_LP_Strap_Normal.png",
		// "Belorian_Soldier_LP_Strap_Roughness.png",
		// "Belorian_Soldier_LP_Strap_Metallic.png"
	};

	//std::vector<std::string> TextureNames =
	//{
	//	//"Belorian_Soldier_LP_Armor_BaseColor.png",
	//	//"Belorian_Soldier_LP_Body_BaseColor.png"
	//	// "Belorian_Soldier_LP_Box_BaseColor.png"
	//	//"Belorian_Soldier_LP_Device_BaseColor.png"
	//	//"Belorian_Soldier_LP_Strap_BaseColor.png"
	//};

	for (int i = 0; i < TextureNames.size(); i++)
	{
		MeshComp->SetTexture(TextureNames[i], "AUTO");
	}

	int a = 0;
}

void Character::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}