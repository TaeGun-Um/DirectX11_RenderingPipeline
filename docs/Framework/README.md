# Framework

프레임워크 구현 관련 문서. 기반 구조부터 시작해 렌더링 기법들을 차례로 쌓아올린다.

## 목차 (시간순)

### 기초 구조
| # | 제목 | 주제 |
|---|---|---|
| 01 | [프레임워크 기능 추가 - 1](01-Framework-Features-1.md) | 디버그·수학·리소스 매니저 |
| 02 | [프레임워크 기능 추가 - 2](02-Framework-Features-2.md) | 프로젝트 구성·Flow·Scene/Actor/Component |
| 03 | [Object마다의 렌더링 파이프라인 구조](03-Per-Object-Pipeline.md) | Mesh / Material / MeshComponentUnit |
| 04 | [렌더링 파이프라인 설정 결과물 출력](04-Rendering-Pipeline-Output.md) | 첫 프레임을 그리는 전체 흐름 |

### 메시·애니메이션·라이팅
| # | 제목 | 주제 |
|---|---|---|
| 05 | [Assimp로 3D Mesh와 애니메이션 로드하기](05-Assimp-Mesh-Animation.md) | 외부 모델·스키닝 |
| 06 | [프레임워크 기능 추가 - 3 + Lighting](06-Framework-Features-3-Lighting.md) | FSM·Collision·Directional Light |
| 07 | [Lighting 추가하기](07-Lighting.md) | Directional + Point + Phong |
| 08 | [Normal Mapping](08-Normal-Mapping.md) | TBN 행렬·탄젠트 공간 |

### 렌더링 고도화
| # | 제목 | 주제 |
|---|---|---|
| 09 | [디퍼드 렌더링으로 변경하기](09-Deferred-Rendering.md) | G-Buffer·라이트 패스 |
| 10 | [그림자 매핑 (Shadow Mapping)](10-Shadow-Mapping.md) | Shadow Map·Depth 비교 |
| 11 | [포스트 프로세싱](11-PostProcess.md) | Blur·OldFilm·Distortion |
| 12 | [반투명 물체 테스트하기](12-Transparency-Test.md) | 디퍼드 + Forward 알파 패스 |
| 13 | [스카이박스](13-SkyBox.md) | CubeMap·월드 배경 |
| 14 | [리플렉션 프로브](14-Reflection-Probe.md) | 정적 환경 반사 |

### 엔진 인프라 개선 (2026-04 이후 추가)
| # | 제목 | 주제 |
|---|---|---|
| 15 | [오브젝트 수명 주기](15-Release-Lifecycle.md) | Destroy/Release · Fast-path 가드 · Dead list |
| 16 | [셰이더 Hot Reload](16-Shader-Hot-Reload.md) | F6 로 런타임 재컴파일 |

## 읽는 순서 제안

처음 본다면 **01 → 02 → 03 → 04** 순서로 프레임워크 기본 구조부터 이해. 그다음 렌더링 기법들(05부터)은 관심 있는 순서대로.

## 참고

- 최근 리팩토링·개선 이력은 [../WorkLog/](../WorkLog/) 참조
- DirectX API 자체에 대한 설명은 [../DirectX/](../DirectX/) 참조
- 원본 블로그(현재 운영 중단): https://umtimos.tistory.com
