# FileStructure_SSU_Week13

<br>

## B+Tree

어떤 노드든 키와 포인터의 개수 n, n+1

<aside>
📌 특징

1. 모든 leaf node는 동일한 레벨에 존재
2. leaf node의 포인터는 sequence pointer을 제외하고 가리킴
3. number of pointer/key
</aside>

<br>

- `split`함
    - 두 개의 노드로 구분
    - tree를 만족시키기 위해 parent-child 구조 설정
        - 포인터 연결
    - 두 노드에 대한 p의 키 → 오른쪽 노드의 키 중 가장 작은값 → `Promote`
- split 전후 차이
    - depth 바뀜
    - leaf → non-leaf
    - 혹은 root 유무
- b+tree에 대한 update
    - delete
    - insert
        - 새로운 key insert → leafnode에서 해당 키에 대한 범위를 검색 해야 함
        - leaf에 추가할 수 있으면 추가
        - n에 한정되어 추가할 수 없다면?
            - 노드를 새로 생성
            - 새로운 노드에대한 parent 연결 → 키 promote

<br>

<aside>

👉 **Updated Cases**

---

- simple case
- leaf overflow
- non-leaf overflow
- new root
</aside>



<br><br>

# B-Tree

- leaf node만이 아닌 다른 node들도 레코드에 대한 포인터 가지고 있음
- 검색할 때 더 빠른 속도
- non-leaf와 leaf의 페이지 사이즈가 다름
    - → 재사용률이 떨어짐
- del/insert 복잡함

→ B+Tree가 더 낫다 (tradeoff에 의해)

<br><br>

책마다 B-Tree에 대해 조금씩 다른 정보를 가지고 있음 <br>
해당 자료구조는 빠른 검색 기능을 구현하기 위해 만들어짐

1. 한 페이지 읽어옴
    - 키 값 비교 → 대소관계 나옴
    - 작으면 왼쪽 노드 → 페이지 읽음
    - 반복
2. 해당 키를 찾으면 페이지의 포인터를 이용해 레코드가 포함된 페이지를 읽음

즉, 자료구조 안에서는 **트리의 depth**만큼 읽기 횟수가 발생

<br><br>


### 검색비용

---

- depth와 일치함
- 최악의 경우 depth는?
    - 모든 노드의 키를 다 채우면 세모모양 (폭은 넓고 높이는 작은) ( 키의 개수 == n)
    - 최소 키의 개수만 채우면 depth가 커짐

- 최악의 경우 d는 3.37
- 즉, 1,000,000개의 키가 있을 때 페이지를 3개만 읽어도 검색 가능
- 읽기 횟수 줄이기
    - 항상 루트 노드를 탐색하기 때문에 버퍼에 루트 노드를 고정시켜둠 → 읽기 횟수 줄이기 가능


<br><br>

누가 질문한 내용

<aside>

💡 **키의 개수가 많다면**

---

장점 : depth가 줄어 읽기 횟수가 적음

단점 : insert를 할 때 split하는 상황이 많이 발생할 수 있음 (확률이 높아짐)

</aside>


<br><br><br>

# Hashing
webbrowser - DBMS - DM

- DBMS <br>
    검색 수행
    - B+Tree
    - SimpleIndex (sorted)
    - Hashing
    
    위와 같이 도구가 있으면 활용하고 없으면 sequential access
    
<br><br>

### Hashing

테이블 형태임

정렬되어있지 않음 → simpleIndex와의 차이점

- SimpleIndex : `Ordered Index`
- Hashing : `Unordered Index`
- `O(1)`
    - 개수(n)와 상관없이 상수 c개에 대해서만 접근하면 됨
- hashing table 만들기
    - **hash function**에 의해 어디에 값을 넣을지가 결정됨
    - k1을 가지는 레코드의 주소 : 포인터로 저장
    - func(k1) = b 라면. b라는 인덱스에 k1을 저장 + 포인터 저장
    
    | num | 학번 | 주소 |
    | --- | --- | --- |
    | a | k2 | 주소2 |
    | b | k1 | 주소1 |
- 검색하기
    - k1라는 학번을 가지는 학생 찾기 → 포인터(주소)값을 원함
    - **동일한 hash func 사용**
    - func(k1) → b 리턴
    - 즉, b에 접근해서 포인터 값을 가져옴
    - → func에 대해 한 번만 접근함
        - 여러 번 읽어야 하는 상황도 있긴 함
        - → 항상 좋진 않음