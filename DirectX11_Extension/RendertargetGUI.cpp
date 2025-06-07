#include "PrecompileHeader.h"
#include "RendertargetGUI.h"

std::map<int, NameRenderTarget> RendertargetGUI::DebugRenderTarget;

void ImageShotWindow::RenderTextureSetting(ImTextureID _RenderTexture, ImVec2 _Size)
{
	RenderTexture = _RenderTexture;
	Size = _Size;
}

void ImageShotWindow::OnGUI(std::shared_ptr<Ext_Scene> _Scene, float _DeltaTime)
{
	if (true == ImGui::ImageButton(RenderTexture, { Size.x, Size.y }))
	{
		UpdateOff();
	}
}

void RendertargetGUI::OnGUI(std::shared_ptr<Ext_Scene> _Scene, float _DeltaTime)
{
	if (ImGui::Button("DebugSwitch"))

	for (std::pair<int, NameRenderTarget> RenderTargetPair : DebugRenderTarget)
	{
		std::string Name = RenderTargetPair.second.Name;
		// 트리노드라는 Imgui 기능을 사용할것이다.
		if (true == ImGui::TreeNodeEx(Name.c_str(), 0))
		{
			std::shared_ptr<Ext_DirectXRenderTarget> RenderTarget = RenderTargetPair.second.RenderTarget;

			int Index = 0;

			for (ID3D11ShaderResourceView* _View : RenderTarget->SRVs)
			{
				float4 Scale = RenderTarget->Textures[Index]->GetScale();
				float4 SmallScale = Scale * 0.2f;

				++Index;

				if (true == ImGui::ImageButton(static_cast<ImTextureID>(_View), { SmallScale.x, SmallScale.y }))
				{
					std::shared_ptr<ImguiWindow> NewWindow = nullptr;

					if (nullptr == Ext_Imgui::FindGUIWindow("ImageShot"))
					{
						NewWindow = Ext_Imgui::CreateImgui<ImageShotWindow>("ImageShot");
					}
					else
					{
						NewWindow = Ext_Imgui::FindGUIWindow("ImageShot");
					}

					NewWindow->UpdateOn();
					std::shared_ptr<ImageShotWindow> Window = std::dynamic_pointer_cast<ImageShotWindow>(NewWindow);
					Window->RenderTextureSetting(static_cast<ImTextureID>(_View), { Scale.x ,Scale.y });
				}
			}

			ImGui::TreePop();
		}
	}
}

void RendertargetGUI::AddDebugRenderTarget(int _Order, std::string_view _Name, std::shared_ptr<Ext_DirectXRenderTarget> _Target)
{
	DebugRenderTarget[_Order] = { _Name.data(), _Target };
}

void RendertargetGUI::Clear()
{
	DebugRenderTarget.clear();
}