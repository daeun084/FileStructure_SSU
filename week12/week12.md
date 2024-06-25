# FileStructure_SSU_Week12


# B-Tree
- 특정 필드에 대한 검색을 대비해 그 주제에 대한 인**덱스 파일**을 만들어 둠
    - 주제에 대해 정렬되어있음
    - 해당 파일은 **디스크에 존재함**
    - binary search를 이용함 → $O(log2N)$
        - N값이 커질 수록 비용이 증가함
        - 해당 경우는 메인 메모리에 올릴 수 있을 정도로 파일 크기가 작을 때만 사용할 수 있음
            - 각 인덱스에 대해서 프로그램을 가져옴 (load)
        - CPU cost + DISK I/O
- 파일의 사이즈가 커지면? → B-Tree / Hash
    - B-Tree →  $logK$ (k는 페이지의 키의 개수)
    
<br><br>    

### B-Tree가 왜 등장했는가

읽기가 발생함 키 값을 비교해야 하기 때문에

- 맨 처음에 개발된 거 : B-Tree
    - 장점
    - 단점
- B-Tree 개선 : B+-Tree
- B*-Tree

<br>

### Problem

- binary search
    - key에 대해서 정렬되어있어야 함
    - 만약 두 개의 필드에 대해 한번에 검색한다면 하나의 키에 대해서만 검색하고 sequential하게 다른 값과 해당 값을 비교해야 함
    - 위치를 결정 (어느 곳에 들어가는지) + 공간 마련 (오른 쪽으로 shift) .. → 관리비용이 너무 큼
    - 추가 비용과 검색 비용이 동일함 → 고비용 요구
    - O(log2N ) → 실전에 사용하기에는 비용이 너무 큼


<br>

### BST
4번 탐색 → 트리의 depth → log2N

- 무슨 차이가 있는가
    - 정렬에 대한 유무
    - 정렬이 필요없음
- 언발란스한 트리가 만들어질 수도 있음 → 문제점임 ..
    - BST이긴 함
    

<br>

### BST를 어떻게 저장하는가

- 각 노드가 가지고 있는 정보 : Key
- tree이기 때문에 반드시 child를 가질 수 있음
- 학번에 대해 검색 : 해당 키를 포함하는 레코드를 반환함

→ ⭐️**key + 노드에 대한 포인터 (왼, 오) + 레코드에 대한 주소값**

노드가 담고 있는 정보들을 레코드에 차례로 담음

fixed-length-record 방식으로 저장하며, 각 레코드의 크기는 동일함

| 0 | 1 | 2 |
| --- | --- | --- |
| key | key2 | key3 |
| lptr(1) | lptr2(null) | lptr3(null) |
| rptr(2) | rptr2(null) | rptr3(null) |
| addr | addr2 | addr3 |

lptr : 왼쪽 노드에 대한 레코드 번호

rptr : 오른쪽 노드에 대한 레코드 번호


레코드 몇 개를 읽어야 하는가 → `O(log2N)`

이러한 노드들을 저장할 때 순서를 고려하지 않음 맘대로 저장?

→ 순서를 고려해서 레코드를 저장해보자 : Paged Binary Trees


<br><br>

## Paged Binary Tree

가까운 미래에 접근할 노드들을 같은 페이지에 저장해둠

- 장점
    - 한 페이지만 읽어오면 많은 노드를 확보할 수 있음
- 단점
    - out of balance
    - tree의 생성에 있어서는 **Balance**에 대한 조건이 정말 중요하고 해당 조건을 만족시켜야 함
    - → B-Tree 가 해당 문제를 해결함
- performance
    - completely full balanced binary tree : `$log2(N+1)$`
    - completely full paged binary tree :`log(k+1)(N+1)`
    - k = keys hold in single page
    - N = keys in the tree


<br><br>

# AVL Tree

left tree, right tree의 depth를 각 구함

한 방향으로만 leaf가 추가되는 것을 막기 위한 방안

depth의 차이는 1보다 작거나 같음

$log2(N+1)$





<aside>

💡 **B-Tree VS B+-Tree**
---

- B-Tree
    - 모든 노드에 데이터가 저장됨
    - 검색은 leafNode에서 끝날 수도 아닐 수도 …
- B+-Tree
    - leafNode에만 데이터가 저장되며 NonLeafNode에는 키값만 저장되어있음
    - leafNode끼리 연결되어있음
    - 검색은 항상 leafNode에서 종료됨 → 일관된 시간 소요
</aside>

<br><br>

# B+Trees
한 노드가 가질 수 있는 노드의 개수를 증가시킴

퍼포먼스 : depth와 관련있음

- Splitting
-> 2개로 쪼갬
- Promotion of Key
-> split이 발생했을 때, 노드의 키를 더 상위의 키로 이동시킴

키를 기준으로 구분

스플릿 1차 → 다 차면 나눔 프로모트

위쪽 방향으로 계속 올라감

프로모트는 연속적으로 계속 생김

루트가 다 차면 루트가 두 개로 스플릿 → 또 다른 패런트가 생김

BTree가 계속해서 커짐


<br><br>

### ⭐️ B+Tree의 특징

---

- leaf node를 따라가면 키에 대해 정렬이 되어 있음
    
    → 검색 + 정렬을 동시에
    
    - leaf node의 오른쪽으로 난 포인터
- non-leaf node는 키만 소유하고 실제 레코드에 대한 데이터는 leaf-node만이 소유함
- leaf-node들끼리는 linked list로 연결되어있으며, 모두 같은 깊이에 존재함


- 공통점
    - key 개수가 n이면 포인터 개수는 n+1
- 차이점
    - non-leaf node : 포인터가 child node를 가리킴
    - leaf node : 해당 키를 가리키는 레코드에 대한 포인터

해당 자료구조는 search를 위해 많이 사용됨

특정 키에 대해서 검색해서 특정 레코드를 가져오기 위함


<br><br>

### 노드의 데이터 저장하기
각 노드가 담고 있는 정보(키, 포인터)를 파일에 저장


<aside>

👉 포인터의 물리적인 값은 ?
- non-leaf
    - child node가 존재하는 페이지 번호를 담음
- leaf
    1. 페이지 번호
    2. 오프셋(시작 바이트)
</aside>