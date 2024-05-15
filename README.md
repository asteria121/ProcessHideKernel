# ProcessHideKernel
불투명 구조체 EPROCESS의 링크드리스트 연결을 중간에서 변조해 프로세스를 숨기는 루트킷.

## How to get member variable address of EPROCESS structure?
* 불투명 구조체 EPROCESS는 공식적으로 멤버 변수에 직접 접근을 지원하지 않음
* ntoskrnl.exe는 항상 PID가 4임
* ntoskrnl.exe는 항상 이미지 이름이 System임
* PID 다음에는 항상 LIST_ENTRY(양방향 링크드리스트) 구조체가 존재함
* 위 정보를 이용해 필요한 멤버 변수의 주소를 알 수 있음
