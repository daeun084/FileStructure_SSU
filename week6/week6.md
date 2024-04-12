# FileStructure_SSU_Week6

## Record Keys
- Primary Key
  - 유일해야 함 → 레코드를 인식/식별
  - 바뀌지않아야 함
- Secondary Key
  - 유일하지 않아도 됨
- Canonical form
  - standard form for key

<br>

## Sequential Search
- 가장 기본적인 검색 방식
- $O(n)$
    - n에 linear하게 비례한다
- 순차적 검색이 유용한 경우
    - `grep`, `cat`, `wc` commands
    - record 수가 적은 경우
- 큰 비용이 발생함

<br><br>

🔎 **레코드를 내 프로그램으로 가져오려면?**
- `READ()`
- OS는 자체적으로 system I/O buffer를 가지고 있음
    - **원하는 데이터가 버퍼에 존재하는지 확인해야 함**
        - 버퍼 : RAM에 존재
    - 있다면 버퍼에 있는거 줌 → 레코드를 카피해서 리턴
        - **CPU 비용**만 발생함 : cost 줄어들음
    - 없다면 해당 레코드를 포함하고 있는 페이지/클러스터 를 읽어 버퍼에 저장
        - **DISK I/O 비용** 발생함 : cost 올라감

많은 프로그램들이 **버퍼를 경쟁적으로 사용**하고 있음

→ 따라서 버퍼에서 데이터가 사라질 수 있고, 이에 따라 데이터를 가져오기 위해 매번 DISK I/O 가 생길 수 있음

최악의 경우, 레코드 READ 할 때마다 매번 DISK I/O가 발생

: 👉`RECORD BLOCKING`👈

💡 해결 방법 : 한 레코드만 읽는게 아니라 **한 페이지 전체를 READ**

<br>


## Direct Access

- $O(1)$
    - n과 상관없이 항상 상수만큼 일정한 비용을 필요로 함
- **fixed-length** structure 사용하기
    - 공간낭비가 너무 심함
- **Index file** 유지
    - 주소를 어떻게 읽어야 하는가 → B+ Tree
    - 주소가 담겨있는 트리의 노드들을 순차로 탐색해서 주소를 찾아냄
- ex)
    - record structure에서 index file 사용해서 n번째 레코드의 시작 주소를 저장해 사용하는 방식
        - 레코드의 길이 정보 = 주소간의 차이

👉 최대한 가변 구조로 저장 후 Index file을 사용하는 방식이 많이 사용됨

<br>

## Header Records

- record placed at the beginning of file
- length of field
- 레코드를 구성하는 필드 개수
- 필드의 이름

- record 개수가 64다 = fix-length로 저장되어있다
- a b 둘 다 fix-length
- b
    - first record의 0028 = 40 → 길이가 아닐까 추측
    - second record의 0024 = 36→ 길이가 아닐까 추측
    - fix-length인데 해당 정보를 왜 쓰는가
        - 데이터가 저장된 곳과 데이터가 없는 곳을 구분하기 위함
- 첫 두 바이트 : 카운트
- 그 다음 두 바이트 : 사이즈

<br>

사용하는 환경에 따라 데이터를 저장하는 방식이나 표현하는 방식이 달라질 수도 있음

→ portability를 고려해야 함

구조를 바꾸는 과정에서 `‘\r’`, `‘\n’` 등을 사용해서 파싱함


<br><br><br><br>



# Data Compression


- 왜 하는가
    - 순차적으로 빠르게 read할 수 있음
        - 읽어야 하는 양이 줄어들음
    - access time 줄일 수 있음
- 단점
    - encoding / decoding 해야 함

<br>

## Using Different Notation

글자 하나를 저장하기 위해 필드에 `1byte`씩 할당해야 함 <br>
0-49까지 50가지의 state를 저장하기 위해 `6bit`를 사용함 <br>
원래 2byte 사용하는데 6bit만 할당해서 저장 <br>
→ 레코드마다 `10bit` 절약

<br>

## Run-length Encoding
같은 값이 계속 등장함
- **특수한 문자열**을 넣어 압축 - **hex**
- 24가 7번 연속으로 나옴 → `ff`로 문자열 표시 후 `24 07` 로 24가 7번 나옴을 명시

<br>

## Huffman code
- 가장 많이 사용되는 char → short bit
- 가장 적게 사용되는 char → longer bit

<br><br><br>


# Reclaiming Space
공간을 회수하다

💡 새로운 레코드의 길이가 기존 레코드의 길이보다 길다면?
레코드 추가, 업데이트, 삭제는 굉장히 어렵고 복잡한 과정임

<br>

### Storage Compaction
- 저장 압축
- ‘레코드 삭제’ 후 수행됨. 주기적 수행X
- 공간이 많다면
    - 삭제된 레코드를 건너뛰고 맨뒤에 새로운 레코드를 추가함
    - 혹은, 삭제된 자리를 **재사용**함
        - **실시간 회수 == 해당 메모리 재사용**


🔎 **레코드 삭제**
- 첫 번째 바이트에 **특수문자** 넣기 <br>
→ 해당 레코드는 삭제됨을 표시함
- 빈 공간을 만들 필요가 없음


<br>

🔎 **삭제된 레코드 찾기**
- 하나씩 다 읽어 첫번째 바이트를 비교해야 함 <br>
→ 시간적 비용이 큼 <br>
👉 삭제된 레코드들에 대해 어떻게 관리할 것인가

<br><br><br>

# Deleting Fixed Length-Records
- 길이가 다 같음
    - **head가 가리키는 레코드 데이터를 어느 자리든 넣으면 됨**
    - 가변길이면 데이터 길이가 다르기 때문에 다른 방법을 취해야 함
- 순차적 검사 : 큰 비용이 들음
    - 👉 use **`linked list`**
    - 삭제된 레코드 관리를 위해 사용함 → 효율적 관리
    - 🔎 다른 방법
        1. index file 생성 <br>
            → 별도의 파일을 만들어야 함
        2. header에 레코드에 대한 정보 저장 <br>
            → 래코드 개수가 늘어나면 한계점이 생김
            
<br>

## Linked List

실시간 공간 회수 구현

- 레코드 삭제 후 마킹
    - 해당 레코드 이전 delete 값에 대한 정보로 링크
    - **→ header에서 정보 가져옴**
- ⭐️**가장 최근에 삭제한 레코드를 맨 앞으로 할당함**⭐️

<br><br>


3, 5, 1 순서로 삭제 가정

1. 3번 삭제
    - header에 3 저장
    - list[3]에 -1 저장 후 삭제 마크(`*`)
    
    (linked list : `[  NULL → 3 ]`)
    

| header | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 3 |  |  |  | * -1 |  |  |  |  |

<br>

2. 5번 삭제
    - header에서 3을 copy하고 header 값을 5로 update
    - list[5]에 `*3` 저장
    
    (linked list : `[  NULL → 5 -> 3 ]`)
    

| header | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 5 |  |  |  | * -1 |  | * 3 |  |  |

<br>

3. 1번 삭제
    
    (linked list : `[  NULL → 1 -> 5 -> 3 ]`)
    

| header | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 |  | * 5 |  | * -1 |  | * 3 |  |  |

<br>

4. 새로운 값 4 ADD
    - linked list의 맨 앞 요소 copy 후 erase
        - 1의 다음값인 5를 header에 update
    - 해당 요소에 4 저장 **(1번 인덱스에 add하려는 value 저장)**
    
    (linked list : `[  NULL → 5 -> 3 ]`)
    

| header | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 5 |  | 4 |  | * -1 |  | * 3 |  |  |