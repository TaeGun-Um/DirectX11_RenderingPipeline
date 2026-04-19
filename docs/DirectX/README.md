# DirectX

DirectX11 API와 이론 정리. 프레임워크 구현 전 단계로, **각 DirectX 개념이 무엇이고 왜 필요한지**에 집중한다.

## 목차 (시간순)

### 출발점
| # | 제목 | 주제 |
|---|---|---|
| 01 | [Device와 Context 생성하기](01-Device-Context.md) | GPU와의 첫 연결 |
| 02 | [SwapChain과 RenderTarget 생성](02-SwapChain-RenderTarget.md) | 더블 버퍼링·렌더 대상 |
| 03 | [Microsoft::WRL::ComPtr<T>](03-ComPtr.md) | COM 수명 관리 |

### 기하 + 셰이더
| # | 제목 | 주제 |
|---|---|---|
| 04 | [InputAssembler 단계 준비와 도형 출력](04-InputAssembler.md) | VB/IB/InputLayout |
| 05 | [카메라와 뷰·프로젝션·뷰포트 행렬](05-Camera-Matrix.md) | 좌표 공간 변환 |
| 06 | [상수버퍼와 셰이더](06-ConstantBuffer-Shader.md) | cbuffer·리플렉션 |

### 파이프라인 상태
| # | 제목 | 주제 |
|---|---|---|
| 07 | [DepthStencilState 생성](07-DepthStencilState.md) | Z-Test·Stencil |
| 08 | [RasterizerState 생성](08-RasterizerState.md) | Culling·Fill Mode |
| 09 | [DirectXTex를 통한 Texture 활용](09-DirectXTex-Texture.md) | 텍스처 로드 |
| 10 | [Texcoord(UV)와 Sampler](10-UV-Sampler.md) | UV·Address Mode·Filter |
| 11 | [BlendState 생성](11-BlendState.md) | Alpha·Additive·Min/Max |

## 읽는 순서 제안

**01 → 04** 순서로 따라오면 "아무것도 없는 상태에서 첫 삼각형을 띄우는 데까지" 필요한 건 다 커버된다. 그다음 파이프라인 상태들(07~11)은 필요할 때 참고해도 됨.

## 참고

- 이 문서들은 블로그에서 이관됐고, 현재 코드 기준으로 일부 교정·보강됨
- 프레임워크 단 구현은 [../Framework/](../Framework/) 참조
- 작업 이력은 [../WorkLog/](../WorkLog/)
