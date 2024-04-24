# FileStructure_SSU_Week2

<br>

## File Processing
- **Physical Files**
  - Disk에 존재하는 파일 형태
  - 위치 정보, 사이즈, 데이터 저장 형태 등과 같은 정보들을 가르킴
- **Logical Files**
  - 사용자가 정보에 접근하기 위해 알아야 하는 논리적 정보
  - 즉, 프로그램 시점에서 관측되는 파일

유저는 파일에 접근하기 위해 OS에게 명령을 내림
OS가 파일과의 연결을 수립해주기 때문에 유저는 파일에 대한 Logical한 정보만 알고 있으면 됨

<br>

## File I/O
1. **Connection 명령 수립**
   - 명령 -> `call`
   - open하기 위해 파일의 정보 필요 
2. **File Descript return**
   - logical file을 뜻함
3. **명령 수행**
   - ex) read, write


<br><br><br>

# System Call

## ✅ open
파일과의 연결을 맺기 위해 사용하는 함수 <br>
- 파일이 존재한다면 file open <br>
- 파일이 존재하지 않는다면 파일을 생성 후 open <br>

파일을 오픈하면, 파일 내부의 포인터는 0번을 가르키고 있음 <br>
맨 마지막 포인터의 값(EOF) == -1

``` c
fd = open(filename, flags[,pmode])
fp = fopen(filename, type)
```

- `O_APPEND`
- `O_CREAT`
  - 반드시 pmode를 통한 접근 권한 설정이 필요함
    - 0, 1을 사용해 8진수로 설정
    - owner, group, other
- `O_EXCL`
  - 독점적 사용을 명시 (exclusive)
  - 다른 사용자와의 파일 동시 접근 방지
- `O_RDONLY`
- `O_RDWR`
- `O_TRUNC`
  - 파일을 없애라 -> 사이즈를 0으로 만들어라
- `O_WRONLY`

<br>

## ✅ close
🚨 프로그램 마지막에 호출을 요구함
- 파일 디스크립터를 OS에게 return
  - 한 프로세서가 사용할 수 있는 파일 디스크립터는 한정되어있기 때문
- 내부적으로 `write`기능을 수행함
    - write 함수는 호출되면 데이터를 버퍼에 저장함
    - OS의 판단 하에 데이터 쓰기 작업을 수행함
    - 👉 close를 호출하면, 버퍼에 남아있는 데이터를 write해 버퍼를 비움

```c
close(fd)
fclose(fp)
```

<br>

## ✅ lseek
포인터의 위치 변경을 위해 사용하는 함수
- origin <br>
    기준 포인터의 위치
  - SEEK_SET
  - SEEK_END
  - SEEK_CUR
- offset <br>
  - 바꾸려는 Byte 개수
  - 정수타입 Long을 사용하며, 숫자 뒤에 l을 붙임
  - SEEK_CUR의 경우, `offset>0 ? 오른쪽 이동 : 왼쪽 이동`

<br>

``` c
lseek(Fd, offset, origin)
fseek(fp, offset, origin)
```
<br>

## ✅ read
파일에서 size만큼 데이터를 읽어 버퍼에 저장 후, 읽은 데이터의 크기(Byte)를 리턴하는 함수 <br><br>
💡어디서부터 read해야 하는가 <br>
-> 내부적으로 가지는 파일의 포인터를 기준으로 함

``` c
read(source_file, destination_addr, size)
fread(), fget(), fscanf()
```
<br>

## ✅ write
💡어디서부터 write해야 하는가 <br>
-> 내부적으로 가지는 파일의 포인터를 기준으로 함

``` c
write(destination_file, source_addr, size)
fwrite()
```

<br><br><br>


    💡 System call vs C libraries

    c libraries 함수를 구현할 때 system call을 내부적으로 호출함
    ➡️ 두 함수 중 무엇을 호출하든 상관없음
    - System Call
      - 운영체제 커널의 기능을 직접 호출하는 프로그램과 운영체제간의 인터페이스
      - 프로그램은 시스템 콜을 통해 파일 I/O, 메모리 할당 및 해제 등의 운영체제 기능을 사용할 수 있음
    - Library Call
      - 프로그램은 특정 기능을 수행하기 위해 라이브러리에 정의된 함수를 호출해 사용함


<br><br><br><br>


# Hard Disk Drive

### Platter
- sector
  - 디스크의 가장 작은 단위
  - track을 구성함
- cluster
  - sector의 집합
  - **파일 시스템에 할당되는 가장 작은 단위**로, 가상의 단위임
  - 파일 매니저는 FAT(file allocation table)을 사용해 클러스터의 위치 정보를 저장함
- extents
  - 연속적인 저장 공간
- track
  - 데이터를 저장하는 길
- cylinder
  - 같은 위치에 존재하는 트랙끼리 연결

<br>

## Disk Access
효율적인 Disk I/O => 디스크에 적게 접근해 비용과 시간을 줄여야 함
- Seek
  - header을 데이터가 있는 실린더로 이동시킴
- Rotate
  - head와 데이터가 존재하는 sector가 접촉할 수 있도록 cylinder를 회전시킴
  - 맨 처음 1byte가 header 아래에 위치하도록 회전시킴
  - rotatioanl delay
- Transfer
  - 처음 1byte부터 마지막 byte를 읽을 때까지 계속해서 회전시켜 데이터를 읽음
  - transfer time
    - 데이터의 크기가 명시되어 있으면 정확한 시간을 구할 수 있음
    - $numOfByteTransferred / numOfBytesOnTrack*RotationTime$ 
