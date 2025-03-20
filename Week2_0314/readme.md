# Week2

## 1. Tree 패키지 설치

`apt update`

설치 전 패키지 리포지터리를 업데이트 해준다.

`sudo apt install tree`

tree 패키지를 설치해준다.


## 2. 디렉토리 생성

  <img src="https://github.com/user-attachments/assets/ead27cf1-4408-4e53-b07f-7c014a3ef82b" width="50%"><br><br><br>

- `cd` 명령어로 원하는 디렉토리로 이동 후 `mkdir` 명령어를 사용하여 디렉토리를 생성한다.
- `mkdir -p ./PATH/DIRECTORY`와 같이 -p 옵션을 사용하면 타겟 경로의 부모 디렉토리도 함께 생성할 수 있다.
- -p 옵션 없이 `mkdir ./PATH/DIRECTORY` 명령을 입력하면 쉘은 현재 디렉토리에서 PATH 디렉토리를 찾아 그 내부에 DIRECTORY를 생성하려고 시도한다.
- 이 때 PATH 디렉토리는 존재하지 않으므로 에러가 발생한다.
