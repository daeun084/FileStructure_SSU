# FileStructure_SSU_Week11

# Cosequential Processing
- 여러 프로세스가 동시에 일을 진행

### EX) 키에 대해서 정렬되었다고 가정

- 파처 수업, DB 수업을 듣는 반 학생들 리스트가 여러 개 있다고 가정
- 두 수업을 모두 듣는 학생을 구하여라 / 두 수업 중 한 수업 이상을 듣는 학생을 구하여라
    - 전자(**교**집합) = `MATCH`
        
        **👉 두 리스트의 각 첫 번째 레코드를 가져와서 비교함**
        
        - 같으면 : 둘 중 하나를 결과 파일에 넣음 + 다음 레코드 읽어옴
        - 전자/후자의 키 값이 더 크면 : 작은 쪽 리스트의 레코드를 계속해서 읽음 (두 레코드의 값이 같을 때까지)
        
        🔎 비용 (레코드를 몇 번 읽었느냐)
        
        - 최대 **`M + N`**
        - 기존 M * N
    - 후자(**합**집합) = `MERGE`

<br><br><br>

# Cosequential Processing
`MERGE`

- 두 리스트의 원소들을 계속해서 비교
- 작은 녀석의 집합에서 다른 리스트의 원소보다 같거나 큰게 나올 때까지 다음 원소를 찾음
    - 리스트가 정렬되었다는 가정

이러한 과정을 통해 얻은 **합집합도 ‼️정렬‼️되어있음**

<br>
<aside>
💡 MERGE VS MATCH

---

MERGE : 두 개의 리스트를 끝까지 다 읽어봐야 함

MATCH : 하나라도 리스트를 다 읽으면 종료

</aside>
<br><br>

# Extension : K-way Merge
key에 대해 정렬되어 있는 k개의 리스트

정렬된 합 집합을 구함

### 1️⃣ key 값에 중복이 있는 경우

- 가장 작은 key부터 시작해야 함
    - 얘는 어느 리스트에 있을까 → 각 집합의 첫 번째 리스트를 비교해서 찾음
- 가장 작은 값이 존재하면 리스트에 추가하고 해당 집합의 다음 원소를 올려서 비교
- 중복이 있기 때문에, 해당 집합에서는 다음 원소를 가져옴
- 계속해서 값이 떨어질 때까지 해당 로직을 반복함
- → 두 개의 리스트로 합집합을 하는 과정을 그저 확장한 것임!

<br>

### 2️⃣ key 값에 중복이 없는 경우

- Q. 중복이 없으니까 그냥 k개의 리스트 합치면 안됨?
    - → 정렬해야 함
- 맨 처음꺼가 가장 작다고 가정하고 계속해서 비교해나감
- k개 중 가장 작은거 찾기 → 위의 알고리즘 사용
    - **몇 번 비교**하는가 : k-1번
- 대소관계비교는 **CPU연산**임 DISK I/O에 비하면 0에 수렴함 → 솔직히 효율적인 알고리즘 찾을 필요 없는데, 개선하기 위해 **selection tree**를 쓰면 좋다

- 제일 작은 값을 위해 몇 번 : k-1번
- 그 다음 작은 값을 위해 몇 번 : 각각 3번
    
    = 트리의 **depth** = `log2(k)` 
    
<br><br><br>


heap sort는 크기가 작은 파일에 대해 유효한 알고리즘임 (internal sort)

메모리에 올려서 정렬한 후 파일에 작성

크기가 큰 파일을 정렬하기 위해서는 작성과 정렬을 동시에 해야 함 (external sort) → 크기를 잘게 나누어 각개전투로 정렬함

정렬된 조각들을 합침

<br><br>

# Merge Sort

- 사용할 수 있는 **버퍼의 크기만큼** 단위를 설정해 파일을 잘게 나눔
- `K-way Merge` Algorithm 사용해서 조각들을 합침

## How Much Time Does a Merge Sort Take?

- 가정 : single **sequential access**
    - data를 읽기 위해 순차적으로 접근함을 가정함 데이터를 작성할 때도 순차적으로 함
    - **one seek** is required for access
    - **one rotational delay** is required per access
    - 값을 write할 때 → 동일함
    - 그 다음 데이터를 읽을 때는 seek가 발생하지 않음 (같은 트랙에 있으니까) → 가정과 다르지 않는가? ㄴㄴ
        - 읽고 정렬하고 작성하고 읽고 정렬하고 작성하고
        - 위와 같은 과정을 거치기 때문에 **데이터가 연속해서 위치해도 seek가 반복해서 발생할 수 밖에 없음**

<br><br>

<aside>
💡 data를 읽을 때 공간이 남는다면, 한 레코드씩이 아니라 더 큰 단위로 읽으면 안되는가 (버퍼 다 채움)

---

- 한 레코드씩 읽으나, 여러 개 한꺼번에 읽으나 어차피 한 트라이에 1 seek가 소요됨
- 8,000,000 sorted record(100byte)이고 버퍼의 크기가 10Mbyte라고 하면, 각 런에서 80개가 나옴 → 총 6400번 읽음
    - 한 번 읽을 때마다 1 seek, 1 rotational delay
</aside>

<br><br>

102.4 * 1024

대충 100,000 record

→ 한 버퍼에 레코드 100,000개 → 전체 파일을 80개의 버퍼(런)으로 나눌 수 있음

<br><br>

### **1️⃣ Reading Record into RAM for Sorting**

- Access: 80 runs * (1 seek+rotation / run) => 80 * (8+3) msec = 880 msec » 1sec
- Transfer: 800,000,000 bytes / (14,500 bytes/msec) = 55147msec » 60sec
- Total: 1 sec + 60 sec = 61 sec

### **2️⃣ Writing Sorted Runs out to Disk**

- The same amount of time that step1 takes

### **3️⃣ Reading Sorted Runs into RAM for Merging**

- Access: 80 runs * (80 seek+80rotation) = 6,400 * (seek+rotation)
    
    => 6,400 * 11 msec = 70 sec
    
- Transfer: 800 Mbytes / (14,500 bytes/msec) = 60 sec
    


### 4️⃣ Writing Sorted File out to Disk

- output buffer을 따로 둠 (200,000 bytes)
- buffer에 값을 채우고 버퍼가 다 차면 한 번에 Write → Disk I/O 줄임
- 4,000회 access

대략 800메가바이트 파일을 키에 대해 정렬하는 데에 6분이 걸림 → 실생활에서 못씀

- 어떻게 해야 성능을 개선할 수 있을까
    - 버퍼의 사이즈를 더 크게 → Disk access 횟수 줄일 수 있음

<br><br><br>


### 1️⃣ Hardware-based Improvement

---

메인메모리의 사이즈를 바꿈


10Mbyte → 40 Mbyte로 바꿈 : 전체 시간 대략 1/3배 ~ 1/4배

- 전체 800Mbyte → 200으로 바뀜
- 한 런에는 몇 개가 있을까 → 200개
- 청크의 개수 = 40,000개
- output buffer는 바꾸지 않았기 때문에 수치상 아웃풋에는 변화가 없음
- (1/4) ^ 2 배가 됨

만약 output buffer의 크기도 바꾸면? (200KB→800KB)만으로 바뀜

즉, 메모리의 크기를 바꾸는게 효과가 있음

### 2️⃣ Disk 추가

---

디스크는 하나임

결과 파일에 sequential하게 값을 작성한다고 해도 동일한 한 파일에 대해서 읽고 쓰고 읽고 쓰고 하기 때문에 읽을 때마다 seek time이 소요됨 

디스크 파일을 두 개 두어 쓸 때마다 seek time이 발생하는 것을 막을 수 있음

→ 한 트랙을 작성할 때 seek 한 번 발생

트랙 사이즈가 t이고 결과 파일의 크기가 8000Mbyte일 때, seek time은 8000Mbyte / t 만큼 발생함

- 읽음을 위한 디스크
- 결과 파일 작성을 위한 디스크

**→ 읽기와 쓰기를 병렬적으로 처리할 수 있음**