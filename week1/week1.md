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

### 📄 File ?
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