#include "PrecompileHeader.h"
#include "MathScene.h"

#include <DirectX11_Base/Base_Input.h>
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Imgui.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>

#include "InformationGUI.h"
#include "BoxActor.h"

void MathScene::Start()
{
	// 라이트 생성
	{
		std::shared_ptr<Ext_Light> Directional = CreateActor<Ext_Light>("Directional");
		Directional->SetLightType(LightType::Directional);
		SetDirectionalLight(Directional);
		Directional->GetTransform()->AddLocalRotation({ 40.0f, 0.f, 0.f });
		Directional->GetTransform()->AddLocalRotation({ 0.f, 40.f, 0.f });
		Directional->SetLightColor({ 1.0f, 1.0f, 1.0f, 0.3f });
	}

	// GUI 생성
	{
		Ext_Imgui::CreateImgui<InformationGUI>("InformationGUI");
	}

	// Actor 생성
	{
		std::shared_ptr<InformationGUI> GUI = std::dynamic_pointer_cast<InformationGUI>(Ext_Imgui::FindGUIWindow("InformationGUI"));
		std::shared_ptr<BoxActor> CubeActor1 = CreateActor<BoxActor>("BoxActor1");
		GUI->SetTransform1(CubeActor1->GetTransform());
		GUI->SetTransform2(CubeActor1->GetMesh()->GetTransform());
	}
}

void MathScene::Update(float _DeltaTime)
{
	if (Base_Input::IsDown("OnOff"))
	{
		int a = 0;
	}
}
