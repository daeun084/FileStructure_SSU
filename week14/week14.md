# FileStructure_SSU_Week14


### 시간 복잡도

---

- Sequential Search : $O(n)$
- Binary Search : $O(log2N)$
- B Tree Index : $O(logkN)$
- Hashing : $O(1)$

# Hashing

---

- hash table
    - FILE 형태로 저장
        - **Fixed-Length-Record 방식 (key + point)**
    - 정렬되어있지 않음 → Simple Index와의 차이점
- hash function : `h(k) = a`
    - transforming key k into addr which is used to store and retrieve records
    - k라는 key값을 해당 주소(a)에 저장해라
    - a : 범위의 정수값 (0~n-1)

<br>

### Search

k2 를 찾아라!

k2가 저장된 레코드의 포인터를 찾으려 함

→ `h(k2) = a2`

a2를 얻어내 해당 레코드에 접근함

<br>

### Hash Function

- n개의 사이즈면 레코드는 0~n-1임
    
    n = 1000 이라고 하면
    
    1. 0~999이므로, 주소의 뒷자리 3개(000)을 취함
    2. 주소 % 1000 을 함
- 문제점
    - 두  개 이상의 키에 대해 같은 hash가 리턴될 수 있음
    - → `Collision`
- 해결 방안
    1. **Overflow List** 생성
        - 해당 레코드에 대해 링크드리스트 형태로 만들어 키에 대한 레코드를 매달음
    2. 해당 주소로부터 아래로 내려가며 **처음 발견한 빈 자리에 레코드 저장**
    3. **또다른 Hash Function** 사용


<br><br>

<aside>

💡 **Collision에 영향을 주는 요인들**

---

- **hash function의 로직**
    - % 은 많이 생김
    - key를 가급적 골고루 분산하도록 해야 함
- **secondary memory** : table size
    - 크게 만드는게 유리함 → %할 때 겹칠 확률 감소
- 한 주소에 하나 이상의 키를 저장
</aside>

<br><br>

## Hashing Algorithm

참고만 하세요

### 1. Represent the Key in Numerical Form

### 2. Fold and Add

- 2개씩 fold
- overflow 발생 가능 O

### 3. Divide by Size of the Address Space

`a = s % n`

<br><br>

# Packing Density
레코드가 많이 packed 될 수록, r이 커질 수록 밀도가 높아져 collision이 발생할 확률도 커짐

**→ r은 고정되어있으므로, N을 조절해 density를 줄임**

<br><br>

## Poisson Distribution

확률분포함수 <br>
N을 얼마로 잡아야 수용 가능한 collision이 발생할지 예측

N : 주소 개수 / 최대 r개

r : record 개수

p(x) : x개의 키가 할당받을 확률

x가 2면 collision은 1개

x가 3면 collision은 2개

collision이 발생할 확률은 21.4%이다.

<br>

N값이 커질수록, 즉 packing density가 줄어들수록 collision이 발생할 확률이 적어짐

hash func이 해당 확률분포를 따른다는 가정 하에 성립함

func을 복잡하게 만들어야지만 collision 문제를 피할 수 있다


hash func은 많은 비용을 초래할 수 있기 때문에 실제로는 B+Tree를 많이 사용함


<br><br>


# Collision Resolution

### Progressive Overflow

= linear probing

- collision이 발생하면 전진하면서 빈 slot을 찾아 데이터 넣음
- 처음 만난 빈 공간을 사용
- **검색** 할 때 순회 종료 조건
    - 빈 공간을 찾을 떄까지
        - 순회하다가 빈 공간이 나온다는 건 insert하지 않음을 의미함
    - 찾기
- 한 바퀴 돌기 즉, 끝까지 순회하면 큰 비용이 요구됨


