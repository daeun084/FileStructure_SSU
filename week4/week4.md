# FileStructure_SSU_Week4

<br>

# Flash Memory S/W

### V1
| Applications        |                       |
|---------------------|-----------------------|
| Flash File System   | 일반적인 파일 시스템 사용 |
| FTL                 | Wear-leveling         |
| Flash Device Driver | 플래시 메모리에 물리적 접근 |
| Flash Memory        |                       |

### V2
| Applications        |                       |
|---------------------|-----------------------|
| Dedicated Flash File System | 파일 시스템 + FTL |
| Flash Device Driver | 플래시 메모리에 물리적 접근 |
| Flash Memory        |                       |

<br><br>

# FTL
- 파일 시스템 : 용량에 따라 n개의 논리 섹터가 존재한다고 가정함
- FTL : 실제 n개의 물리 섹터에 데이터를 write함


각 layer는 독립적이며, 저장 매체로 누가 오든 영향을 받지 않도록 설계됨

#### 기능
- 플래시 메모리의 물리적 제약을 소프트웨어적으로 해결
- ⭐️ 파일시스템의 논리적 주소를 플래시 메모리의 물리적 주소로 맵핑 ⭐️
- lgn -> ppn

<br>

## Sector Mapping
- sector 단위로 주소를 관리하며, `page == sector`
  - logical page num == logical sector num
- flash memory의 sector의 초기값은 `-1`이며, 데이터가 들어오며 번호를 할당해나감
-  <b> Address mapping table </b>
   -  lsn, psn mapping
- ✅ Practice 1 : `write(9, A)`
  - File System : 9번 page 할당
  - logical page number 9와 physical page num 3을 mapping
  - FTL -> Driver : `write(3, A)` 명령
  - Driver : 3번 physical page에 A의 데이터를 write
- ✅ Practice 2 : `read(9, A)`
  - File System : logical page num 9에 데이터가 있음을 감지
  - FTL : lpn 9 과 ppn 3 의 mapping을 감지
  - FTL -> Driver : `read(3, A)` 명령
  - Driver : 3번 physical page의 데이터를 A buf에 read 
- 👍 장점
  - 페이지 할당 방법이 단순함
  - 업데이트가 단순하며 비용이 적게 들음
- 👎 단점
  - 테이블을 메인 메모리에서 유지함 : 빠른 속도
  - 테이블 사이즈가 커지면 메모리가 부족해짐
    - 💡 sector 단위를 쓰기 때문에 사이즈가 커지므로 block 단위를 사용해 해결

<br><br>

## Block Mapping
- 페이지 관리 단위는 sector이나, 블락을 기준으로 연산을 수행함
- `logical block num = (logical page num / page_per_block)`
- `offset = psn = (logical page num % page_per_block)`
-  <b> Address mapping table </b>
   -  lbn, pbn mapping
- 👍 장점
  - sector mapping보다 단위가 커져 테이블의 사이즈 감소
- 👎 단점
  - update가 복잡하고 비용이 많이 들음
    - 페이지 번호는 유지하되, 블락 번호만 변경 -> 블락 내의 다른 페이지들도 함께 카피해야 함 + mapping table 변경
    - 기존 블락은 가비지 컬렉터에게 넘김
    - 💡 만약 빈 블락이 없다면? <br>
        -> 가비지 컬렉터가 가지고 있는 블락들 중 하나를 택함
    - ⭐️ <b>빈 블락이 없는 경우를 대비하기 위해 항상 하나의 빈 블락을 남겨놓음</b> ⭐️


<br><br>

## Hybrid Mapping
block mapping과 유사함
- ⭐️ 순차적으로 빈 페이지를 찾아 할당 ⭐️
  - ‼️ update 과정에서 블락 단위 erase를 위해 페이지 copy를 할 필요 X
- mapping table을 통해 lbn, pbn을 매핑해 블락을 찾아가 비어있는 페이지에 순차적으로 데이터 작성
- 💡 빈 페이지가 없다면 -> 빈 블락 찾아서 값을 copy (overwrite)
  - 다른 mapping 방법에 비해 블락 copy 횟수가 확연히 감소
- 🚨 해당 데이터가 유효한지 어떻게 판단할 수 있는가
  - flash memory의 spare를 보고 판단
  - 순차적으로 데이터를 작성하므로, 아래에 위치할 수록 최신 데이터임
  - 블락을 다 읽어야 하므로 리딩 시간이 오래 걸림

flash memory 사용에 있어서, overwrite는 가장 고려해야 하는 연산이므로, 다른 맵핑 방법보다 해당 연산의 횟수가 적은 hybrid mapping 방법이 가장 효율적으로 생각됨

    flash memory는 in-place-update가 불가함
    sector 영역 : 불가
    spare 영역 : 가능하나 제한 횟수 존재