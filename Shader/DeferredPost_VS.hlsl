struct VSInput
{
    float4 Position : POSITION;
    float4 Texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

VSOutput DeferredPost_VS(VSInput _Input)
{
    VSOutput OutPut;
    OutPut.Position = _Input.Position;
    OutPut.Texcoord = _Input.Texcoord.xy;
    return OutPut;
}


// ������� ���ؼ� ���� �ݻ縦 �����ϰ� ����
// �� ���� ����ϴ� ������ ���� �������� ������ �ߴ�.
// ������ ������ ���� �ǹ̰� ����.
// ���̶�� �÷��� ���س��� ���� �ȴ�.

// �װ� ���ؽ� ���̴������ϸ� �װ� �������� �ϸ� �÷����̵�
// �װ� ���ؽ� ���̴������ϸ� �װ� ��ν��̵�
// �װ� �ȼ� ���̴������ϸ� �װ� �����̵�

// �׷���ī�忡�� �̷����°�.