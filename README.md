<div align="center">
    <h1>✏️DIRECTX 12 STUDY✏️</h1>
    <h3>DEVELOPMENT ENVIRONMENT</h3>
    <img src="https://img.shields.io/badge/C++-00599C?style={flat}&logo=C%2B%2B&logoColor=white"/>
    <img src="https://img.shields.io/badge/DirectX12-5E5E5E?style={flat}&logo=microsoft&logoColor=white"/>
    <img src="https://img.shields.io/badge/Visual Studio-5C2D91?style={flat}&logo=VisualStudio&logoColor=white"/>
</div>

# 📑 프로젝트 설명
- DirectX 12를 공부하기 위해 생성된 레포지토리이다.
- 주로 수업 과제 해결과 졸업작품에서 사용할 프레임워크 구축 및 기술들을 공부한다.
- 이 프로젝트에서 사용된 리소스들은 업로드되어 있지 않다.

# 🔎 세부 설명
## 01. Framework

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157208051-07a3940c-f4c8-4e30-b7fa-c0f23038c713.gif">
</div>

DirectX 12를 이용하여 윈도우를 띄우고 큐브를 렌더링한다. 카메라가 큐브 하나를 따라다니도록 설정했으며 `WASD`로 이동하고 `L-SHIFT`로 내려가고 `SPACE`로 올라갈 수 있다. 키보드 명령의 경우 다른 프로젝트에서도 동일하다.

## 02. Instancing

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157211179-7b56b791-6de5-4b82-a3b9-c396d9dd8679.gif">
</div>

1,000개의 큐브를 인스턴싱으로 렌더링한다. 인스턴싱 버퍼를 만들어서 메쉬를 렌더링할 때 셰이더에 엮어줬다.

## 03. Picking

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157213471-66640f5f-f4bb-4e80-a148-fd6e690281f5.gif">
</div>

`L-CTRL`을 누르고 있으면 마우스를 움직일 수 있고, 마우스로 큐브를 선택하면 플레이어 큐브가 해당 위치로 이동한다. 마우스 클릭 좌표를 받아서 `스크린 좌표계 -> 투영 좌표계 -> 카메라 좌표계 -> 월드 좌표계`의 순서로 변환한다. 이 계산으로 `월드 좌표계`에서의 화면 중앙에서 마우스 쪽으로 가는 `광선`을 구할 수 있고 이 광선과 다른 게임오브젝트들과 충돌 검사한다.

## 04. Texture

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157216139-b858144a-6d95-4859-8a9b-6b97b4730f80.gif">
</div>

`.dds` 포멧의 파일을 읽어서 큐브에 입혔다. [이곳](https://github.com/microsoft/DirectXTex/tree/main/DDSTextureLoader)에서 `.dds` 파일을 읽을 수 있는 소스코드를 받아서 사용했다.

## 05. Terrain

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157217169-829cd44f-871d-4c21-a412-5f59c756187c.gif">
</div>

`높이맵 이미지`를 읽어와서 `그리드 메쉬`의 높이를 그에 맞게 설정한다. 플레이어도 지형의 높이에 맞게 움직이도록 구현했다. 또한 스카이박스도 구현했다. `스카이박스`는 플레이어의 위치를 계속해서 따라오고 다른 게임오브젝트보다 먼저 렌더링하며 깊이 버퍼에 쓰지 않는다.

## 06. Tessellation

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157219903-5c073e55-1c71-4bb4-a530-16e5a8b66ee3.png">
</div>

`기하 셰이더`를 이용하여 카메라에 가까이 있는 곳은 잘게 쪼개고 멀리 있는 곳은 넓게 자른다. 또한 테셀레이션해서 생긴 정점의 위치는 베지에 곡면을 이용하여 자연스럽게 연결되게 구현했다.

## 07. Blending

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157223795-edc3ff31-7f3f-4f2c-9c73-82bc47004ab3.gif">
</div>

`마우스 왼쪽 클릭`하면 보고있는 방향으로 총알이 발사되고 지형에 부딪히거나 멀리 날아가면 폭발한다. 폭발하는 이펙트는 `빌보드`와 `블렌딩`을 사용하여 폭발 이펙트 뒤의 지형이 보일 수 있도록 했다.

## 08. Mirror

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157224335-11d1b807-3c3e-4ca2-ae1e-f0aca9d3f2dd.gif">
</div>

`스텐실 버퍼`를 이용하여 거울을 구현했다. 씬을 렌더링할 때는 다음의 순서로 렌더링한다.
- 스텐실 버퍼 초기화 후 스텐실 참조값을 1로 설정한다.
- 스텐실 버퍼에 거울을 렌더링한다.
  - 거울 위치에만 스텐실 버퍼의 값이 1로 되어있다.
- 게임오브젝트들의 월드 행렬에 반사 행렬을 곱하고 렌더링한다.
  - 스텐실 버퍼가 1인 곳에만 렌더링한다.
- 거울을 보통 방법으로 렌더링한다.
  - 거울 뒤가 안보이도록 하기 위해 통상적인 렌더링도 한다.
- 스텐실 참조값을 0으로 설정한다.
- 스카이박스, 플레이어, 게임오브젝트, 지형을 렌더링한다.

## 09. Shadow

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157227053-aa434970-130a-45d0-883a-dc837f2fe551.gif">
</div>

광원 위치에서 씬을 렌더링한 것을 셰이더 텍스쳐에 저장하고 통상적으로 씬을 렌더링할 때 카메라에서 바라본 해당 픽셀 위치의 깊이값과 광원에서 바라본 해당 픽셀 위치의 깊이값과 비교해서 카메라에서 바라본 깊이값이 더 클 경우에 그림자로 판정한다. 그림자로 판정할 때 주변 픽셀의 그림자 여부도 확인하여 그림자 가장자리가 부드럽게 되도록 구현했다.

## 10. Blur

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157229794-79efb214-d6e5-4c79-9f1c-ae1a91ca1eb9.png">
</div>

`계산 셰이더`를 이용하여 블러를 구현했다. 렌더링이 완료된 장면을 셰이더 텍스쳐로 보내고 가로, 세로 블러링을 하고 UAV로 결과를 내보낸다. 내보낸 결과를 받아서 최종 렌더링한다.

## 11. StreamOutput

<div align="center">
    <img src="https://user-images.githubusercontent.com/34495921/157230435-b4f10c75-9782-4d14-abb7-619bf4dc90ba.gif">
</div>

`스트림 출력`을 이용하여 비를 구현했다. 한 번은 통상적인 렌더링을 하고 다음은 렌더 타겟 없이 렌더링하고 다음 렌더링 때 이전 렌더 타겟 없이 렌더링한 결과를 이용하여 렌더링한다. 렌더 타겟 없이 렌더링할 때 `계산 셰이더`에서 비 메쉬의 y좌표를 감소시키고 일정 높이 이하라면 y값을 높게 재설정해준다. 그 결과로 하늘에서 비가 떨어지는 효과를 GPU에서 구현했다.
