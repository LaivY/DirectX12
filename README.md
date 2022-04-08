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

`기하 셰이더`를 이용하여 카메라에 가까이 있는 곳은 잘게 쪼개고 멀리 있는 곳은 넓게 자른다.