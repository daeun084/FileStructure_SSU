# FileStructure_SSU_Week3

## Disk I/O
seektime에서 가장 많은 시간이 소요됨 <br>
-> 어떻게 하면 해당 시간을 줄일 수 있을까 <br>
-> ⭐️Extents 적극 활용⭐️

<br><br>

# File Manager
모든 파일에 대한 정보를 가지고 있음
- file descripte, 위치(sector, cylinder), 물리적 위치 정보, 접근 권한 등

<br>

#### 🔎 Write() 함수 수행 과정
1. Write statement -> OS
   - location of char / send one char to disk
2. OS -> FileManager
3. FileManager
   - file open 유무 확인
   - 파일에 대한 접근 권한 확인
   - physical file location 확인
   - search FAT -> sector의 physical location 확인
   - RAM에 해당 데이터가 존재하는지 Buffer 확인
4. FileManager -> I/O Processor
   - Buffer에 저장된 Byte 위치 혹은 Disk의 위치 넘겨줌
5. I/O Processor
   - get data from Buffer
   - put data in disk
6. I/O Processor -> Disk Controller
   - send Data to DiskController
7. Disk Controller
   - seek, rotate, transfer을 통한 실질적인 데이터 write 작업 수행

<br><br><br>

# Flash Memory

#### 플래시 메모리를 사용하는 이유
- HDD에 비해 빠른 접근 속도
- 작은 전력 소비량
- 좋은 내구성
- 작은 크기 & 작은 무게
- 소리가 안남
  
## Nand Flash Memory 구조
- chip <br>
- page <br>
  - sector - 512Byte / 데이터 저장
  - spare - 16Byte / 메타 데이터 저장
- block <br>
  page의 집합

<br>

#### 대블록 플래시 메모리
- 각 블록은 64개의 페이지로 구성됨
- 4개의 sector : 2048Byte
- 4개의 spare : 64Byte
#### 소블록 플래시 메모리
- 각 블록은 32개의 페이지로 구성됨
- 1개의 sector : 512Byte
- 1개의 spare : 16Byte

<br>

## HDD vs Flash Memory
- seek, rotate time X <br>
  HDD는 기계적인 장치로 구성되어 있기 때문에 구조적으로 회전이 필요함 그러나 플래시 메모리는 움직일 필요가 없기 때문에 seek, rotate, transfer time이 소요되지 않음 <br>
  ➡️ ** Random Access를 해도 I/O Cost가 동일함 ** 

- write cost가 read cost보다 10배 비쌈 <br>
- in-place update X <br>
  해당 페이지에 이미 데이터가 존재하면 ⭐️데이터 덮어쓰기를 할 수 없음⭐️ <br>
  erase op가 필요하나, 이는 read op의 10배 가격임 따라서, 플래시 메모리에서 데이터 업데이트를 위해서는 아주 큰 비용이 요구됨

<br>

## In-Place-Update
1block = 4page 라고 가정
1. 해당 블럭에 존재하는 다른 페이지들 카피
2. 빈 블럭에 해당 페이지 paste
3. 기존 블럭 erase
4. 카피했던 페이지를 기존 블럭에 다시 카피
5. 업데이트하려던 데이터 작성
6. 백업해둔 블럭 erase

너무 큰 비용이 요구되며, 다른 블럭의 빈 페이지에 새로 작성하는 Out-Of-Place Update 방법을 사용하고 정보의 주소를 수정하는 것 추천