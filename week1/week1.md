# FileStructure_SSU_Week1

<br>

# Memory
메모리에 있는 데이터를 읽고 쓰기 위해서는 메모리 접근이 필요함 <br>
-> Memory I/O를 통한 Access
- primary memory
  - register
  - cache
  - main memory
- secondary memory
  - disk

<br>

# File Structure
File Structure = `설계 + 구현` = `데이터의 표현 + 데이터의 계산`
- 평가기준
  - 시간
    - Disk Access의 횟수가 적어야 함
  - 가격
- 파일 구조 디자인이 왜 필요한가
  - 데이터 처리를 위해서는 하드 디스크 및 메모리에 있는 데이터를 읽고 쓰는 작업이 요구됨
  - 이를 위해서는 메모리 접근이 요구되는데, 하드 디스크 I/O는 아주 큰 비용이 소요됨
  - 따라서, Disk Access 횟수를 줄여 적은 비용으로 데이터 및 정보를 얻어오기 위해 파일 구조 디자인이 중요해짐 

### File ?
같은 종류의 레코드의 집합이며 Disk에 저장됨

### History
- Sequential Access
  - 순차적 접근, 시간적 비용이 크게 소모됨
- Simple Index
  - 하드 디스크의 발명으로 인해 혁명이 일어남
  - 데이터의 위치 정보를 이용해 **Random Access** 가능
  - 시간적 비용이 크게 감소함
- Flash Memory
  - SSD의 등장