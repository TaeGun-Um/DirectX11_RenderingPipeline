#include "PrecompileHeader.h"
#include "Ext_DirectXBufferSetter.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXSampler.h"
#include "Ext_DirectXTexture.h"

// 버퍼 세팅 복사/붙여넣기 실시
void Ext_DirectXBufferSetter::Copy(const Ext_DirectXBufferSetter& _OtherBufferSetter)
{
	for (const std::pair<std::string, ConstantBufferSetter>& Setter : _OtherBufferSetter.ConstantBufferSetters)
	{
		ConstantBufferSetters.insert(Setter);
	}

	for (const std::pair<std::string, TextureSetter>& Setter : _OtherBufferSetter.TextureSetters)
	{
		TextureSetters.insert(Setter);
	}

	for (const std::pair<std::string, SamplerSetter>& Setter : _OtherBufferSetter.SamplerSetters)
	{
		SamplerSetters.insert(Setter);
	}

	//for (const std::pair<std::string, StructuredBufferSetter>& Setter : _BufferSetter.StructuredBufferSetters)
	//{
	//	StructuredBufferSetters.insert(Setter);
	//}
}

// 텍스쳐 값 변경
void Ext_DirectXBufferSetter::SetTexture(const std::string& _SettingTexture, std::string_view _NewTextureName)
{
	std::shared_ptr<Ext_DirectXTexture> NewTexture = Ext_DirectXTexture::Find(_NewTextureName);
	if (nullptr == NewTexture)
	{
		std::string Name = _NewTextureName.data();
		MsgAssert("이런 이름의 텍스쳐는 로드한 적이 없습니다." + Name);
		return;
	}

	std::string SlotName = _SettingTexture;

	// 자동 매핑 처리
	if (Base_String::ToUpper(_SettingTexture) == "AUTO")
	{
		std::string UpperName = Base_String::ToUpper(_NewTextureName.data());

		if (UpperName.find("BASECOLOR") != std::string::npos || UpperName.find("albedo") != std::string::npos)
		{
			SlotName = "BaseColorTex";
		}
		else if (UpperName.find("NORMAL") != std::string::npos)
		{
			SlotName = "NormalTex";
		}
		else if (UpperName.find("ROUGHNESS") != std::string::npos)
		{
			SlotName = "RoughnessTex";
		}
		else if (UpperName.find("METALLIC") != std::string::npos)
		{
			SlotName = "MetallicTex";
		}
		else if (UpperName.find("EMISSIVE") != std::string::npos)
		{
			SlotName = "EmissiveTex";
		}
		else
		{
			MsgAssert("알 수 없는 텍스처 이름입니다. 자동 슬롯 지정 실패: " + UpperName);
			return;
		}
	}

	std::string UpperSlotName = Base_String::ToUpper(SlotName);
	auto Range = TextureSetters.equal_range(UpperSlotName);

	if (Range.first == Range.second)
	{
		MsgAssert("이런 이름의 텍스쳐 슬롯은 없습니다. " + UpperSlotName);
		return;
	}

	for (auto Iter = Range.first; Iter != Range.second; ++Iter)
	{
		TextureSetter& Setter = Iter->second;
		Setter.Texture = NewTexture;
	}
}

// 상수 버퍼 데이터 저장
void Ext_DirectXBufferSetter::SetConstantBufferLink(std::string_view _Name, const void* _Data, UINT _Size)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	std::multimap<std::string, ConstantBufferSetter>::iterator FindIter = ConstantBufferSetters.find(UpperName);

	if (ConstantBufferSetters.end() == FindIter)
	{
		MsgAssert("존재하지 않는 상수버퍼를 세팅하려고 했습니다." + UpperName);
		return;
	}

	std::multimap<std::string, ConstantBufferSetter>::iterator NameStartIter = ConstantBufferSetters.lower_bound(UpperName);
	std::multimap<std::string, ConstantBufferSetter>::iterator NameEndIter = ConstantBufferSetters.upper_bound(UpperName);

	for (; NameStartIter != NameEndIter; ++NameStartIter)
	{
		ConstantBufferSetter& BufferSetter = NameStartIter->second;

		if (BufferSetter.ConstantBuffer->GetBufferSize() != _Size)
		{
			MsgAssert("상수버퍼와 세팅하려는 데이터의 크기가 다릅니다. 상수버퍼 : " + std::to_string(BufferSetter.ConstantBuffer->GetBufferSize()) + "유저가 세팅한 데이터" + std::to_string(_Size) + UpperName);
			return;
		}

		BufferSetter.CPUData = _Data;
		BufferSetter.CPUDataSize = _Size;
	}
}

// 상수 버퍼 세팅(상수버퍼, 샘플러, 텍스쳐)
void Ext_DirectXBufferSetter::BufferSetting()
{
	// 상수 버퍼 세팅
	std::multimap<std::string, ConstantBufferSetter>::iterator CBStartIter = ConstantBufferSetters.begin();
	std::multimap<std::string, ConstantBufferSetter>::iterator CBEndIter = ConstantBufferSetters.end();
	for (; CBStartIter != CBEndIter; ++CBStartIter)
	{
		ConstantBufferSetter& CBSetter = CBStartIter->second;
		CBSetter.ConstantBufferSetting();
	}

	// 샘플러 세팅
	std::multimap<std::string, SamplerSetter>::iterator SPStartIter = SamplerSetters.begin();
	std::multimap<std::string, SamplerSetter>::iterator SPEndIter = SamplerSetters.end();
	for (; SPStartIter != SPEndIter; ++SPStartIter)
	{
		SamplerSetter& SPSetter = SPStartIter->second;
		SPSetter.SamplerSetting();
	}

	// 텍스처 세팅
	std::multimap<std::string, TextureSetter>::iterator TexStartIter = TextureSetters.begin();
	std::multimap<std::string, TextureSetter>::iterator TexEndIter = TextureSetters.end();
	for (; TexStartIter != TexEndIter; ++TexStartIter)
	{
		TextureSetter& TexSetter = TexStartIter->second;
		TexSetter.TextureSetting();
	}
}

// 상수버퍼 세팅
void ConstantBufferSetter::ConstantBufferSetting()
{
	ConstantBuffer->ChangeData(CPUData, CPUDataSize);

	ShaderType Type = OwnerShader.lock()->GetType();

	switch (Type)
	{
	case ShaderType::Unknown:
	{
		MsgAssert("어떤 쉐이더에 세팅될지 알수없는 상수버퍼 입니다.");
		break;
	}
	case ShaderType::Vertex:
	{
		ConstantBuffer->VSSetting(BindPoint);
		break;
	}
	case ShaderType::Pixel:
	{
		ConstantBuffer->PSSetting(BindPoint);
		break;
	}
	default:
		break;
	}
}

// 샘플러 세팅, 바인딩 슬롯 찾아서 VSSetSamplers(), PSSetSamplers() 실시
void SamplerSetter::SamplerSetting()
{
	ShaderType Type = OwnerShader.lock()->GetType();

	switch (Type)
	{
	case ShaderType::Unknown:
	{
		MsgAssert("어떤 쉐이더에 세팅될지 알수없는 샘플러 입니다.");
		break;
	}
	case ShaderType::Vertex:
	{
		Sampler->VSSetting(BindPoint);
		break;
	}
	case ShaderType::Pixel:
	{
		Sampler->PSSetting(BindPoint);
		break;
	}
	default:
		break;
	}
}

// 텍스쳐 세팅, 바인딩 슬롯 찾아서 VSSetShaderResources(), VSSetShaderResources() 실시
void TextureSetter::TextureSetting()
{
	ShaderType Type = OwnerShader.lock()->GetType();

	switch (Type)
	{
	case ShaderType::Unknown:
	{
		MsgAssert("어떤 쉐이더에 세팅될지 알수없는 텍스쳐 입니다.");
		break;
	}
	case ShaderType::Vertex:
	{
		Texture->VSSetting(BindPoint);
		break;
	}
	case ShaderType::Pixel:
	{
		Texture->PSSetting(BindPoint);
		break;
	}
	default:
		break;
	}
}

void TextureSetter::TextureReset()
{
	ShaderType Type = OwnerShader.lock()->GetType();

	switch (Type)
	{
	case ShaderType::Unknown:
	{
		MsgAssert("어떤 쉐이더에 세팅될지 알수없는 텍스쳐 입니다.");
		break;
	}
	case ShaderType::Vertex:
	{
		Texture->VSReset(BindPoint);
		break;
	}
	case ShaderType::Pixel:
	{
		Texture->PSReset(BindPoint);
		break;
	}
	default:
		break;
	}
}