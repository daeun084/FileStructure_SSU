# FileStructure_SSU_Week7

# Deleting Variable-Length Records
fixed-length records를 제거하는 것 보다 더 복잡함 <br>
추가하려는 레코드의 길이가 기존 삭제된 레코드의 길이보다 클 수도 있음 <br> 
-> 그 다음 비어 있는 레코드의 길이를 확인 <br>

위의 방식처럼 리스트를 순회하며 사이즈를 확인하는 것은 긴 시간이 소요됨

### Linked List 활용
- RRN 대신 Byte Offset 활용
  - 길이가 가변적이기 때문에, 레코드의 상대적인 시작 위치를 나타내는 RRN은 사용하기 어려움
- 레코드의 길이 정보가 필요하기 때문에, 실제 데이터(n byte) + 길이 정보(2 byte) = (n+2) byte을 offset을 통해 정보를 저장함
- 레코드를 reuse 하기 위해 해당 레코드의 길이 정보를 확인해야 함 -> Offset
  - 길이가 크거나 같은지 확인
  - 최악의 경우 N번 확인해야 함
- Linked List의 중간 요소를 삭제해, 연결 순서를 수정해야 하는 경우 생김

<br>

## Storage Fragmentation
<strong>💡 72 byte의 삭제된 레코드에 55 byte의 data가 들어간다면? </strong><br>
- data (55 byte) + 길이 정보 (2 byte) = 57 byte 를 사용
- 나머지 15 byte는 낭비됨 -> Record 내부에서 발생했으므로 👉`Internal Fragmentation`👈
  
<br>

해당 공간을 어떻게 사용할 것인가 -> remove / reuse

<br>

### Fragmentation Reuse
남는 15 byte를 linked list에 추가함
- 레코드의 앞 부터 데이터 채우기
  - 기존 레코드의 크기를 57 byte로 수정
  - 뒤에 남는 공간 15 byte에 대해서 기존 레코드를 삭제 후 새로운 삭제 레코드 등록이 필요함
- 레코드의 뒤 부터 데이터 채우기
  - 기존 레코드의 크기(비어있는 레코드)를 15 byte로 수정
  - 뒤에 57 byte만큼의 데이터 작성
  - 비용적인 측면에서 데이터를 맨 뒤에서부터 채우는 것이 더 효율적임

<br>


### External Fragmentation
💡 레코드가 과도하게 단편화 되었다면? <br>
8byte 크기의 삭제 레코드가 존재하고, 레코드의 최소 저장 단위가 20byte라면 해당 레코드는 영원히 사용되지 않음 -> 👉`External Fragmentation`👈

- storage compaction
  - 주기적으로 메모리를 제거함
- 이웃하는 레코드와 합쳐서 더 큰 공간으로 만들기
- fragmentation이 덜 발생하도록 `placement strategies`를 이용해 할당 사이즈에 주의하기
  - placement strategies
    - first-fit
      - 할당하려는 레코드보다 사이즈가 같거나 큰 처음 발견한 레코드를 할당
    - best-fit
      - 할당하려는 레코드의 사이즈와 사이즈가 가장 유사한 레코드를 할당
      - 아주 작은 공간이 남을 수 있음 -> Internal fragmentation 유발
      - sorting이 필요하며, n개의 레코드를 다 확인해야 함
    - worst-fit
      - 가장 큰 공간을 가진 레코드를 할당함
      - 시간상 뒤 쪽에서 fragmentation이 발생함
      - 마찬가지로 sorting이 필요하며, n개의 레코드를 다 확인해야 함