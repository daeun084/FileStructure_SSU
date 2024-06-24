# FileStructure_SSU_Week10

- sort
    - internal sort
        - heap sort
            
            sorting을 할 record file 존재할 때 어떤 토픽에 대해 어떻게 정렬할 수 있는가
            
            - 메인메모리가 크다면 + 파일 사이즈가 작다면
                - 파일의 모든레코드를 메인 메모리로 통채로 로딩해 저장함
                - 이 상태에서 정렬
                - heap 이라는 구조를 이용함
    - external sort

# HeapSort
1. 첫 번째 레코드를 메인 메모리에 저장하고 해당 인덱스에 대한 HEAP 생성
2. 두 번째 레코드를 메인 메모리에 저장하고 해당 키에 대해서 HEAP 업데이트
3. …
4. 만들어진 HEAP을 이용해 정렬을 함
<br><br>

- 모든 레코드에 대해 HEAP 생성
- HEAP을 가지고 정렬을 수행함

<br><br>

## HEAP 이란?
- binary tree
    - **1️⃣ complete binary tree**
        - 맨 마지막 level에 존재하는 node는 반드시 왼쪽부터 오른쪽으로 노드가 추가되어야 함
        - 맨 마지막 level을 제외한 모든 노드는 두 개의 leaf node를 가져야 함
    - 2️⃣ 모든 **node의 key는 항상 parent node의 key보다 같거나 크다**

노드를 이용해 표현되는 heap은 logical한 표현임

실제 heap 자료구조는 주로 배열, linked list를 사용해 physical하게 메모리에 저장됨


- 첫 번째 인덱스 = `root`
- `i`(parent) → `2i` , `2i + 1` (children)
- `j` (children) → `j / 2` (parent)

<br><br><br>

### Procedure for building a heap

```
For i := 1 to RECORD_COUNT
	Read in the next record and append it to the end of the array; call its key K
	While K is less than the key of its parent
		Exchange the record with key K with its parent
next i
```

- record를 읽어들이고 배열의 마지막에 추가함
- binary tree로 변경
    - children이 parent보다 작다면, parent와 값 switch

계속해서 parent의 key 값과 child의 key 값을 비교해서 key를 exchange해야 함



<br><br>
🔎 Heap의 특징

---

- 가장 작은 값은 루트에 존재함
- 오름차순 정렬 (배열 상에서)



<br><br>

