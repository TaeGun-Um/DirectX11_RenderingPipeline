#pragma once

// 상수버퍼 저장용 인터페이스
struct ConstantBufferSetter
{
	std::string Name;
	std::weak_ptr<class Ext_DirectXShader> OwnerShader;
	int BindPoint = -1; // b0 t0 같은 몇번째 슬롯에 세팅되어야 하는지에 대한 정보.
	std::shared_ptr<class Ext_DirectXConstantBuffer> ConstantBuffer;
	const void* CPUData;
	UINT CPUDataSize;

	void Setting();
};

// 다양한 종류의 상수 버퍼를 저장하고 가져오기 위해 사용하는 클래스
class Ext_DirectXBufferSetter
{
	friend class Ext_MeshComponentUnit;

public:
	// constrcuter destructer
	Ext_DirectXBufferSetter();
	~Ext_DirectXBufferSetter();

	// delete Function
	Ext_DirectXBufferSetter(const Ext_DirectXBufferSetter& _Other) = delete;
	Ext_DirectXBufferSetter(Ext_DirectXBufferSetter&& _Other) noexcept = delete;
	Ext_DirectXBufferSetter& operator=(const Ext_DirectXBufferSetter& _Other) = delete;
	Ext_DirectXBufferSetter& operator=(Ext_DirectXBufferSetter&& _Other) noexcept = delete;

	// 생성된 상수버퍼 저장하기
	void InsertConstantBufferSetter(const ConstantBufferSetter& _Setter) { ConstantBufferSetters.insert(std::make_pair(_Setter.Name, _Setter)); } 

	// 상수버퍼 데이터 저장
	template<typename Type>
	void SetConstantBufferLink(std::string_view _Name, const Type& _Data)
	{
		SetConstantBufferLink(_Name, reinterpret_cast<const void*>(&_Data), sizeof(Type));
	}

protected:
	
private:
	std::multimap<std::string, ConstantBufferSetter> ConstantBufferSetters; // 상수 버퍼 저장용 컨테이너
	
	void SetConstantBufferLink(std::string_view _Name, const void* _Data, UINT _Size); // 상수버퍼 데이터 저장
	void Copy(const Ext_DirectXBufferSetter& _OtherBufferSetter); // 버퍼 세팅 복사/붙여넣기 실시
	void BufferSetting(); // 버퍼 세팅 호출
};