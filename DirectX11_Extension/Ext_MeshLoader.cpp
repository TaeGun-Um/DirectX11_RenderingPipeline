#include "PrecompileHeader.h"
#include "Ext_MeshLoader.h"

#include "Ext_DirectXVertexData.h"

// 로드 시 버텍스 버퍼 생성용 벡터와 인덱스 버퍼 생성용 버퍼를 전달해주면 값을 입력해줌, 그걸로 Create 실시하면 됨
bool Ext_MeshLoader::LoadMeshFromFile(const std::string& _FilePath, std::vector<Ext_DirectXVertexData>& _OutVertices, std::vector<uint32_t>& _OutIndices)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(_FilePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == nullptr)
    {
        MessageBoxA(nullptr, Importer.GetErrorString(), "Mesh Import Error", MB_OK);
        return false;
    }

    ////////////////////////////// VertexBuffer를 위한 데이터 입력 부분 //////////////////////////////
    aiMesh* Mesh = Scene->mMeshes[0]; // 단일 메시 가정

    for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
    {
        Ext_DirectXVertexData VertexData;
        VertexData.POSITION = float4(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
        VertexData.NORMAL = Mesh->HasNormals() ? float4(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z) : float4::ZERO;
        if (Mesh->HasTextureCoords(0))
        {
            //VertexData.TEXCOORD = float4(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
            VertexData.TEXCOORD = float4(Mesh->mTextureCoords[0][i].x, 1.0f - Mesh->mTextureCoords[0][i].y); // CCW
        }
        else
        {
            VertexData.TEXCOORD = float4::ZERO;
        }

        _OutVertices.push_back(VertexData);
    }

    // CCW
    for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
    {
        const aiFace& Face = Mesh->mFaces[i];

        if (Face.mNumIndices == 3) // 삼각형만 처리
        {
            // CW → CCW로 전환 (Face.mIndices[1], [2] 순서 변경)
            _OutIndices.push_back(Face.mIndices[0]);
            _OutIndices.push_back(Face.mIndices[2]);
            _OutIndices.push_back(Face.mIndices[1]);
        }
        else
        {
            // 삼각형이 아닐 경우 그대로 넣거나 무시
            for (unsigned int j = 0; j < Face.mNumIndices; ++j)
            {
                _OutIndices.push_back(Face.mIndices[j]);
            }
        }
    }

    return true;
}

bool Ext_MeshLoader::LoadMeshWithBoneFromFile
(
    const std::string& _FilePath,
    std::vector<Ext_DirectXVertexData>& _OutVertices,
    std::vector<uint32_t>& _OutIndices
)
{
    Assimp::Importer importer;
    const aiScene* AssScene = importer.ReadFile
    (
        _FilePath,
        aiProcess_Triangulate
        | aiProcess_JoinIdenticalVertices
        | aiProcess_GenNormals
        | aiProcess_CalcTangentSpace
    );

    if (!AssScene || (AssScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !AssScene->mRootNode)
    {
        MessageBoxA(nullptr, importer.GetErrorString(), "Mesh Import Error", MB_OK);
        return false;
    }

    aiMesh* AssMesh = AssScene->mMeshes[0]; // 단일 메시 가정

    // 1) 버텍스당 본 가중치를 임시 보관할 버퍼 준비
    std::vector<std::vector<std::pair<uint32_t, float>>> boneData;
    boneData.resize(AssMesh->mNumVertices);

    // 2) Assimp의 aiBone → boneData 채우기
    for (uint32_t b = 0; b < AssMesh->mNumBones; ++b)
    {
        const aiBone* bone = AssMesh->mBones[b];
        uint32_t      boneIndex = b; // 간단히 0..mNumBones-1 매핑

        for (uint32_t w = 0; w < bone->mNumWeights; ++w)
        {
            const aiVertexWeight& vw = bone->mWeights[w];
            // vw.mVertexId: 이 본이 영향을 주는 버텍스 인덱스
            // vw.mWeight: 이 버텍스에 대한 가중치
            boneData[vw.mVertexId].emplace_back(boneIndex, vw.mWeight);
        }
    }

    // 3) 실제 Ext_DirectXVertexData 벡터에 정점 데이터 채우기
    _OutVertices.clear();
    _OutVertices.reserve(AssMesh->mNumVertices);

    for (uint32_t i = 0; i < AssMesh->mNumVertices; ++i)
    {
        Ext_DirectXVertexData V;

        // (가) 위치/법선/UV 채우기
        // 위치: aiVector3D → float4(x,y,z,1)
        {
            const aiVector3D& p = AssMesh->mVertices[i];
            V.POSITION = float4(p.x, p.y, p.z, 1.0f);
        }

        // 법선: aiMesh->HasNormals() 체크 후
        if (AssMesh->HasNormals())
        {
            const aiVector3D& n = AssMesh->mNormals[i];
            V.NORMAL = float4(n.x, n.y, n.z, 0.0f);
        }
        else
        {
            // 법선 정보가 없으면 기본값 (0,1,0,0)
            V.NORMAL = float4(0.0f, 1.0f, 0.0f, 0.0f);
        }

        // UV: 0번 채널만 사용, float4(u, 1-v, 0, 0)
        if (AssMesh->HasTextureCoords(0))
        {
            const aiVector3D& uv = AssMesh->mTextureCoords[0][i];
            V.TEXCOORD = float4(uv.x, 1.0f - uv.y, 0.0f, 0.0f);
        }
        else
        {
            V.TEXCOORD = float4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        // 색상 정보: FBX에 없으면 기본 흰색(1,1,1,1)
        V.COLOR = float4(1.0f, 1.0f, 1.0f, 1.0f);

        // (나) BONEID / WEIGHT 초기화
        //     Ext_DirectXVertexData의 생성자에서 이미 0으로 초기화되었다고 가정.
        //     XMUINT4 ids(0,0,0,0); → V.BONEID = ids; 가능
        //     float4(0,0,0,0) → V.WEIGHT = float4(0,0,0,0); 가능

        // ⇒ 여기서는 따로 초기화 코드 없이 넘어갑니다.

        _OutVertices.push_back(V);
    }

    // 4) boneData를 순회하며 “버텍스별 BoneID, Weight” 채우기 (최대 4개 본만 사용)
    for (uint32_t i = 0; i < AssMesh->mNumVertices; ++i)
    {
        // boneData[i] 에는 이 버텍스에 영향을 주는 (boneIndex, weight) 리스트
        auto& inf = boneData[i];
        size_t count = std::min<size_t>(inf.size(), 4);

        // XMUINT4 ids(0,0,0,0) → BONEID
        DirectX::XMUINT4 ids(0, 0, 0, 0);

        // XMFLOAT4 w4(0,0,0,0) → XMFLOAT4를 float4로 변환할 때는 float4(w4.x, w4.y, w4.z, w4.w)
        float w0 = 0.0f, w1 = 0.0f, w2 = 0.0f, w3 = 0.0f;

        for (size_t k = 0; k < count; ++k)
        {
            switch (k)
            {
            case 0:
                ids.x = inf[0].first;   // boneIndex
                w0 = inf[0].second;  // weight
                break;
            case 1:
                ids.y = inf[1].first;
                w1 = inf[1].second;
                break;
            case 2:
                ids.z = inf[2].first;
                w2 = inf[2].second;
                break;
            case 3:
                ids.w = inf[3].first;
                w3 = inf[3].second;
                break;
            default:
                break;
            }
        }

        // 이제 V.BONEID, V.WEIGHT를 설정
        _OutVertices[i].BONEID = ids;              // XMUINT4 → OK
        _OutVertices[i].WEIGHT = float4(w0, w1, w2, w3);
    }

    // 5) 인덱스 채우기 (CCW)
    _OutIndices.clear();
    _OutIndices.reserve(AssMesh->mNumFaces * 3);

    for (uint32_t f = 0; f < AssMesh->mNumFaces; ++f)
    {
        const aiFace& face = AssMesh->mFaces[f];
        if (face.mNumIndices == 3)
        {
            // CCW 순서(0, 2, 1)로 뒤집어 들어가야 맞다면, 아래처럼 삽입
            _OutIndices.push_back(face.mIndices[0]);
            _OutIndices.push_back(face.mIndices[2]);
            _OutIndices.push_back(face.mIndices[1]);
        }
        else
        {
            for (uint32_t j = 0; j < face.mNumIndices; ++j)
            {
                _OutIndices.push_back(face.mIndices[j]);
            }
        }
    }

    return true;
}