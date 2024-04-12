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

