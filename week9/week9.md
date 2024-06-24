# FileStructure_SSU_Week9



- 데이터를 다루는 데에 가장 중요한 것 : 검색
    - insert, delete, update, select .. 중 select가 제일 중요함
- 검색의 특징
    - 비용이 큼

💡 학번이 입력된 레코드에서 특정 학번을 찾으려면?

- 비정렬
    - $O(N)$

- 정렬
    - binary search → $O(log2N)$
  
<br><br>

# Indexing

가장 기본적인 검색 툴 / 더 빠른 검색을 위한 **도구**로 사용됨 <br>
ex) B+-Tree. Hash File, Simple Index, …

### Simple Index
- label : 회사명
- ID number : 책 발급 순서
    - 회사마다 발급 순서가 겹칠 수 있기 때문에 PK X


<br><br>

🔎 `PK` 만들기 : **Label + ID number**

- `Identify`해야 함
- `Minimum`해야 함


<br><br>

PK를 key로 사용하는 인덱스 = `Primary Index`
다른 조건을 Key로 사용할 수도 있음 → `Secondary Index` <br>
    - byte offset보다 primary index에 연관되어있음

👉 검색 조건에 따라 어떤 인덱스 파일을 쓸지 결정


<br><br>


⭐ 인덱스 파일 저장

---

- 파일 저장 위치 = `DISK`
- 인덱스 ⇒ `파일` 형태로 존재
- 각 레코드 구분 ⇒ `fixed-length-record`로 저장
- 각 필드 구분 ⇒ `fixed-length-field`로 저장

<br><br>


💡만약 ‘**FF245**’에 대해 검색 요청이 온다면? <br>
→ Binary Search 이용

10^6 이라면 log2N → log210^6 + 1 → 21 <br>
레코드 크기가 작을 때만 사용 <br>
→ 인덱스에 대해 정렬되어 있기 때문에 sequential search X <br>

entry-sequenced (항목 정렬) → 기록된 순서대로 정렬되어있음


<br><br><br>


🔎 Search Engine

---

- Disk에 존재하는 파일(인덱스)를 메인 메모리에 올려둠 (미리)
- RAM에서
- 메인메모리에 존재하는 인덱스를 사용해 검색함
- CPU 비용만 발생
    - 파일을 대상으로 하면 DISK I/O

즉, 이런 인덱싱 방식은 인덱스 파일의 크기가 작아야만 사용 가능함. 인덱스를 메인 메모리에 미리 올려야 하기 때문


<br><br><br>

## Maintain Indexed File

- how to init simple index file
    - 6-2 key sort 참고
    - 각 레코드에서 키 값을 뽑아내 배열 형태로 저장하고 sort함
    - → **fixed-length-record** 방식으로 저장
    - → simple index file
    - index의 유효기간을 체크하는 로직이 요구됨 → 시스템 오류 방지
        - 유효기간이 되면 프로그램은 인덱스를 다시 생성해야 함
- index file에 대한 update, insert, delete

<br>

### Index file Update

1. key update
    - index update 후 sort
2. addr update
    - record size가 증가해 더 큰 주소가 필요할 경우 데이터를 새로운 레코드에 저장한 후 offset을 새 주소로 변경함

<br>

### Simple index의 단점

- 인덱스 파일의 크기가 작을 때만 사용 가능
    - RAM에 올려야 하기 때문
    - 그게 아니라면 파일을 대상으로 연산해야 하며, DISK I/O 가 발생
- 크기가 아주 크다면 → 다른 방법 사용 (B+ Tree, Hash Table, …)
    - B+ Tree
        - DISK에 존재
        - $O(logkN)$
            - N이 10^9이라면 3


<br><br><br>

💡 KeySort VS Indexing

---

- keysort
    - key를 기준으로 데이터를 정렬함
        - 실제 레코드를 RAM에 write해, RAM에서 재배열
- indexing
    - 키와 관련된 인덱스 파일 생성
    - 레코드의 위치 매핑
    - 검색의 효율성을 높이기 위해 사용됨
    - 원본 데이터를 수정하지 않고 별도의 데이터 구조 생성


<br><br><br>

### Secondary indexes affects file op

- addition
    - adding an entry to the secondary index
    - record in secondary **index must be shifted**
- deletion
    - removing corresponding entry in the primary index
    - removing all of the entries in the secondary indexes referring to this primary index entry
- updating
    - when changing the secondary key, we have to reorder the secondary index
    - when changing the primary key, we update only the affected reference field in all the secondary indexes
    

→ 🚨 2가지 문제점 발생

1. **파일에 새로운 레코드가 업데이트될 때마다 인덱스 파일의 재정렬이 필요함**
2. **secondary index가 반복되어 메모리 낭비**

<br><br>

## ✨ **해결 방안**

### 1. Using array of primary key references for each secondary key

- 장점 : direct access가 가능함
- 단점 : internal fragmentation와 같은 공간 낭비가 생길 수 있음
    - 공간이 꽉 차서 추가적인 요소가 저장되지 못할 수도 있음
    

### 2. Linking the list of primary key references

- 장점
    - secondary index를 재배열하기 용이함
    - 재배치가 빠름 (레코드 수가 적고 크기가 작기 때문)
    - 삭제된 레코드를 관리하기 위한 로직 구현 용이
    - label ID list file이 항목 시퀀스이기 때문에 정렬할 필요가 없음
- 단점
    - 모든 primary key를 가져올 때 값을 읽는 과정에서 비용이 많이 발생함
        - 즉, **지역성(locality)**이 보장되지 않음
        - → random 읽기를 해야 하기 때문에 **seeking cost**가 더 많이 발생함
