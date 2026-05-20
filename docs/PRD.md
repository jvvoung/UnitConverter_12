# UnitConverter_12 — 제품 요구사항 문서 (PRD)

**문서 버전**: 1.0  
**기준**: Phase 4 Epic · User Journey · User Stories · Gherkin · Level 5 체크리스트  
**범위**: 본 저장소(README, 콘솔 CLI, Catch2, 외부 설정)  
**금지**: 구현 코드 · 클래스 설계 · 빌드 스크립트 본문

---

# 1. 프로젝트 개요

## 1.1 한 줄 목적문 (What / Who / Why)

**What**: meter 기준 길이 단위를 변환하는 C++ 콘솔 학습 시스템  
**Who**: C++·클린 아키텍처·TDD를 학습하는 개발자  
**Why**: 환산 알고리즘이 아니라 **계약·테스트·레이어 분리(BCE)** 로 확장 가능한 구조를 6시간 실습 안에 체득하기 위함

## 1.2 배경 및 문제 정의 (관찰 관점)

출발 상태는 **입력 파싱·비율 분기·stdout 출력이 한 진입점에 결합**된 절차형 변환기이다. README는 meter/feet/yard 변환, OCP·SRP, 입력 검증, 테스트, 설정 외부화, 동적 단위 등록, JSON/CSV/table 출력을 요구한다. 관찰상 **비율 상수가 여러 분기에 중복**되며, 단위·포맷·설정이 늘 때 **변환 핵심까지 수정**되는 구조 리스크가 있다. 학습자는 “곱셈 구현”보다 **입출력 계약 고정 → RED 테스트 → Domain/Boundary/Data 분리 → 회귀 스냅샷** 순서가 필요하다.

## 1.3 목표 (측정 가능)

| ID | 목표 | 측정 |
|----|------|------|
| G-01 | Domain 불변식·환산 규칙을 테스트로 고정 | Domain Line ≥95%, Branch ≥90% |
| G-02 | Boundary 입출력·에러 계약을 스냅샷으로 고정 | 계약 TC ≥10, stderr/stdout diff 0 |
| G-03 | README meter↔feet↔yard 환산·1dp table 예시 충족 | GH-1·Golden trio 스냅샷 통과 |
| G-04 | 단위·포맷 확장 시 변환 엔진 시그니처 불변 | Engine 변경 0건(리뷰·diff 확인) |
| G-05 | 설정·등록·실패 경로 E2E 검증 | 정상 E2E ≥3, 실패 E2E ≥4 GREEN |

## 1.4 비목표 (Non-Goal)

| ID | 비목표 | 사유 |
|----|--------|------|
| NG-01 | 웹 UI·REST API·DB·네트워크 | README·저장소 미포함 |
| NG-02 | REGISTER 결과의 디스크 영속화 | README 미요구; 세션 내 메모리만 |
| NG-03 | feet↔yard 직접 쌍 하드코딩 | meter 허브 정책과 상충 |

---

# 2. 사용자 및 이해관계자

## 2.1 타깃 사용자 (페르소나 1명)

| 항목 | 내용 |
|------|------|
| 이름(가칭) | 김학습 |
| 역량 | C++ 기초, 콘솔 입출력 경험; OCP·SRP·계약 테스트는 제한적 |
| 동기 | 6시간 실습·과제·회고에서 **측정 가능한 구조 개선** 증명 |
| 성공 정의 | SC·Gherkin·커버리지·스냅샷으로 “계약 우선 개발” 달성 |

**이해관계자**: 본인(구현·테스트), 강사/리뷰어(인수·체크리스트), CI(회귀·커버리지 게이트, 선택).

## 2.2 주요 사용 시나리오 (Journey 기반)

| ID | 시나리오 | Journey | 성공 관측 |
|----|----------|---------|-----------|
| SCN-01 | README 예시 `meter:2.5` 입력 → table 8.2 feet / 2.7 yard | J2 계약 → J4 TDD | GH-1 exit 0, stderr 빈 |
| SCN-02 | `REGISTER 1 cubit = 0.4572 meter` 후 `cubit:2` 변환 | J6 확장 | meter ≈0.9144, golden 불변 |
| SCN-03 | 깨진 설정 파일로 기동 → 명시적 실패 | J5 Boundary·Data | exit 2, 변환 미호출 |

---

# 3. 기능 요구사항

## 3.1 핵심 기능 목록

| 우선순위 | ID | 기능 | Story |
|----------|-----|------|-------|
| **필수** | F-01 | `단위:값` CONVERT 파싱·검증 | US-01 |
| **필수** | F-02 | meter 허브 전 단위 환산 | US-03 |
| **필수** | F-03 | UnitCatalog·OCP 확장 | US-02 |
| **필수** | F-04 | table 기본 출력·원입력 보존 | US-04, GH-2 |
| **필수** | F-05 | 음수·0·형식·미지원 단위 거부 | US-01, US-07 |
| **권장** | F-06 | JSON/YAML 설정 로드 | US-05 |
| **권장** | F-07 | `REGISTER` 동적 단위 등록 | US-06 |
| **권장** | F-08 | `--format json\|csv\|table` | US-04 |
| **선택** | F-09 | Traceability·RED 36건 매핑 문서 | Epic SC-10 |

## 3.2 기능별 입·출력 계약 (문자열 수준)

### F-01 CONVERT

| 항목 | 계약 |
|------|------|
| 입력 | 한 줄: `{unit}:{value}` · `unit`=`[A-Za-z][A-Za-z0-9_]*` · `value`=양의 유한 십진 리터럴 |
| 성공 stdout | 포맷별 POL-OUT·§6 준수 |
| 실패 stderr | §3.3 표 준수, 1줄, exit 1 |

### F-02 환산

| 항목 | 계약 |
|------|------|
| 입력 | 파싱된 `(unit, value)` + 로드된 Catalog |
| 처리 | `target = source × (k_source / k_target)`, `k`=`meters_per_unit` |
| 출력 | Catalog 전체 단위 중 `source` 제외 N−1건 |

### F-07 REGISTER

| 항목 | 계약 |
|------|------|
| 입력 | `REGISTER 1 {unit} = {k} meter` · `k`>0 유한 |
| 성공 | exit 0, stderr 빈, 동일 프로세스 Catalog 갱신 |
| 실패 | §3.3 `INVALID_REGISTER_SYNTAX` / `DUPLICATE_UNIT` |

### F-06 설정 로드

| 항목 | 계약 |
|------|------|
| 입력 | `--config {path}` · JSON 또는 YAML |
| 성공 | Catalog에 `units[]` 반영 |
| 실패 | exit 2, `Failed to load config: {path}` |

## 3.3 제약 사항 (Gherkin Background·POL과 일치)

### Background Given (고정 전제)

| 항목 | 값 |
|------|-----|
| base unit | `meter` |
| `meter` meters_per_unit | `1.0` |
| `feet` meters_per_unit | `0.3048` |
| `yard` meters_per_unit | `0.9144` |
| 규칙 1 | `1 meter = 3.28084 feet` 성립 |
| 규칙 2 | `1 meter = 1.09361 yard` 성립 |
| 기본 출력 | `table`, 소수 **1자리 half-up** |

### 입력 형식

| 규칙 ID | 내용 | 실패 코드 |
|---------|------|-----------|
| IN-01 | 콜론 필수 `unit:value` | `INVALID_FORMAT` |
| IN-02 | unit 비어 있음 금지 | `INVALID_FORMAT` |
| IN-03 | value 비수치·깨진 소수 | `INVALID_NUMBER` |
| IN-04 | POL-NEG: `0 < value < +∞` finite | `NEGATIVE_VALUE` 메시지 패턴 |

### POL-NEG (음수·0 정책)

- **POL-NEG-01**: 허용 값은 양의 유한 실수만.
- **POL-NEG-02**: `value ≤ 0`·NaN·Inf는 Boundary에서 Domain 호출 전 거부.
- **POL-NEG-03**: stderr `Value must be positive: {value}`, exit 1.

### 미지원 단위

- Catalog에 없는 `unit` → exit 1, stderr `Unknown unit: {unit}`, stdout 변환 없음, 부분 줄(`{value} {unit} =`) 금지 (GH-8).

### 에러 메시지·exit (고정)

| error_code | exit | message (치환 외 바이트 고정) |
|------------|------|-------------------------------|
| INVALID_FORMAT | 1 | `Invalid format. Use unit:value (ex: meter:2.5)` |
| INVALID_NUMBER | 1 | `Invalid number: {token}` |
| NEGATIVE_VALUE | 1 | `Value must be positive: {value}` |
| UNKNOWN_UNIT | 1 | `Unknown unit: {unit}` |
| DUPLICATE_UNIT | 1 | `Unit already registered: {unit}` |
| INVALID_REGISTER_SYNTAX | 1 | `Invalid register syntax. Use REGISTER 1 {unit} = {k} meter` |
| CONFIG_LOAD_FAILED | 2 | `Failed to load config: {path}` |
| EMPTY_CATALOG | 2 | `No units available for conversion` |

---

# 4. 비기능 요구사항

## 4.1 기술 스택

| 항목 | 버전·도구 |
|------|-----------|
| 언어 | C++17 |
| 빌드 | CMake (멀티 타깅: Domain / Boundary / Data / 통합) |
| 테스트 | Catch2 v2 또는 v3 (프로젝트 1종 고정) |
| 플랫폼 | README 기준 로컬 콘솔(g++ 호환 환경) |

## 4.2 아키텍처 원칙

| 원칙 | 요구 |
|------|------|
| BCE | Entity(Domain)·Control(Application)·Boundary(CLI/Parser/Formatter)·Data(Config Source) |
| 의존 방향 | Boundary→Control→Domain; Data→Domain 정의 로드만; **Domain→Boundary/Data 금지** |
| SRP | 파싱 / 검증 / Catalog / Engine / 포맷 / 설정 로드 책임 분리 |
| OCP | 신규 단위=`Catalog.add`; 신규 포맷=`Formatter` 추가; `ConversionEngine` 시그니처 변경 0 |

## 4.3 테스트 커버리지 목표

| 레이어 | Line | Branch |
|--------|------|--------|
| Domain | ≥95% | ≥90% |
| Boundary | ≥90% | ≥85% |
| Data | ≥85% | ≥80% |
| Control | ≥80% | ≥75% |
| 전체 | ≥88% Line | — |

**수치 허용오차(환산 TC)**: `|a−e| ≤ max(1e−9, |e|×1e−9)`.

## 4.4 확장성 원칙

1. 신규 단위는 **meters_per_unit 1개**로 Catalog에만 추가.
2. feet↔yard 등 파생 비율은 **meter 경유 계산만** 허용.
3. 포맷 추가는 출력 계층만; 환산 결과 DTO는 공통.
4. REGISTER는 **프로세스 수명** Catalog만 변경(파일 쓰기 비목표).

---

# 5. 데이터 요구사항

## 5.1 단위 비율 상수 (meter 허브)

| unit | meters_per_unit (1 unit = k meter) | README 대응 |
|------|--------------------------------------|---------------|
| meter | 1.0 | 기준 |
| feet | 0.3048 | 1 m = 3.28084 ft |
| yard | 0.9144 | 1 m = 1.09361 yd |

**검증**: `k_feet = 1/3.28084`, `k_yard = 1/1.09361` within §4.3 오차.

## 5.2 설정 외부화

| 항목 | 규칙 |
|------|------|
| 포맷 | JSON **또는** YAML (프로젝트 1종 + 파서 1종 고정) |
| schema_version | `1`만 허용 |
| base_unit | `meter`만 허용, 불일치 시 `CONFIG_LOAD_FAILED` |
| units[] | `{ name, meters_per_unit }` · 중복 name 금지 · 빈 배열 금지 |
| 실패 정책 | fallback 없음, exit 2 |
| 테스트 | InMemory Source는 Data·Domain TC 전용; 운영 경로는 File |

**기본값(파일 미지정 시)**: 위 §5.1 3단위 InMemory Catalog와 **수치 동일**.

## 5.3 동적 단위 등록 계약

| 항목 | 계약 |
|------|------|
| 문법 | `REGISTER 1 {unit} = {k} meter` |
| unit | `[A-Za-z][A-Za-z0-9_]*`, Catalog 미존재 |
| k | `0 < k < +∞` finite |
| 예시(README) | `REGISTER 1 cubit = 0.4572 meter` → `cubit:2` → meter 0.9144 (오차 규칙) |
| 중복 | `DUPLICATE_UNIT` |
| 문법 위반 | `INVALID_REGISTER_SYNTAX` |

---

# 6. 출력 요구사항

## 6.1 POL-OUT (공통 표현 계약)

- **POL-OUT-01**: 성공 시 모든 포맷에서 **source unit·source value는 사용자 입력 파싱값 그대로**.
- **POL-OUT-02**: target으로 역산한 값으로 source 표시를 대체하지 않음.
- **POL-OUT-03**: source 표시 자릿수는 입력 토큰과 동일(`2.5`≠`2.50` 강제 변환 금지).

## 6.2 table (기본, 필수)

| 필드 | 규칙 |
|------|------|
| 줄 형식 | `{source_value} {source_unit} = {target_value} {target_unit}` |
| 줄 수 | N−1 (source 제외) |
| target_value | 1dp half-up (GH-1: 8.2, 2.7) |
| stderr | 성공 시 빈 |

## 6.3 JSON (권장)

| 필드 | 타입 | 규칙 |
|------|------|------|
| source.unit | string | 입력 unit |
| source.value | number | 입력 value 전체 정밀도 |
| conversions[].unit | string | target unit |
| conversions[].value | number | 1dp 아님, §4.3 오차 |
| conversions 길이 | N−1 | |
| well-formed | — | 파서 검증 TC 통과 |

## 6.4 CSV (권장)

| 행 | 내용 |
|----|------|
| 헤더(고정) | `source_unit,source_value,target_unit,target_value` |
| 데이터 | N−1행, source는 입력값·단위 반복 |
| 구분자 | 콤마 4필드, 필드 수 고정 TC |

## 6.5 포맷 선택

- CLI `--format table|json|csv`, 미지정 시 `table`.
- 동일 입력·Catalog에서 **target unit 집합 동일** (포맷 간 TC).

---

# 7. 성공 지표

## 7.1 인수 기준 (체크박스, Story·Gherkin 정합)

- [ ] **AC-01 (US-01, GH-3~7)**: 형식·비수치·음수·0·unknown 전건 exit·stderr·stdout 규칙 일치
- [ ] **AC-02 (US-03, GH-1, BG)**: `meter:2.5` → `2.5 meter = 8.2 feet`, `2.5 meter = 2.7 yard`, exit 0
- [ ] **AC-03 (GH-2, US-04)**: `feet:8.2` 모든 줄 prefix `8.2 feet =`
- [ ] **AC-04 (US-06)**: REGISTER cubit 후 `cubit:2` meter≈0.9144, golden trio 불변
- [ ] **AC-05 (US-05)**: 깨진 config → exit 2, `Failed to load config:` prefix, 변환 0회
- [ ] **AC-06 (E-SC-02~05)**: Domain/Boundary/Data 커버리지 임계값 충족 리포트 제출
- [ ] **AC-07 (US-02, E-SC-01)**: Domain→Boundary/Data 의존 0, Engine 확장 diff 0

## 7.2 회귀 보호 규칙 (계약 변경 금지)

| ID | 규칙 | 검증 |
|----|------|------|
| RR-01 | 기본 3단위 `meters_per_unit` 변경 시 CI **의도적 FAIL** | SC-08 |
| RR-02 | Golden trio table 스냅샷 PR마다 diff 0 | SC-07 |
| RR-03 | §3.3 에러 message 패턴 바이트 변경 시 스냅샷 TC FAIL | SC-03 |
| RR-04 | REGISTER 후 `meter:2.5` table 스냅샷 diff 0 | US-06 |
| RR-05 | POL-NEG·POL-OUT·Background 수치 변경은 **PRD 버전 bump + Gherkin 동시 수정** 필수 | SG-06 |

---

# 8. 용어 정의 (Glossary)

| 용어 | 정의 |
|------|------|
| **meter 허브** | 모든 환산의 중간 기준; 각 단위는 `meters_per_unit` 하나로 표현 |
| **UnitCatalog** | `UnitName`→`MetersPerUnit` 등록·조회; 이름 유일 |
| **meters_per_unit (k)** | `1 {unit} = k meter` 양수 유한 상수 |
| **POL-NEG** | 양의 유한 길이만 허용; 0·음수·비유한 거부 정책 |
| **POL-OUT** | 출력에 source unit·value를 입력 그대로 보존하는 표현 계약 |
| **Boundary** | stdin/argv·파싱·검증·Formatter·stderr/exit 담당; 환산식 없음 |
| **Domain** | Catalog·ConversionEngine·불변식; Boundary/Data 비의존 |
| **Golden trio** | 입력값 `{1.0, 2.5, 0.1}` × `meter:` table 스냅샷 고정 세트 |
| **Contract TC** | Mock Domain 하에서 입출력·에러 문자열만 검증하는 Boundary 테스트 |
| **REGISTER** | 런타임 Catalog 확장 명령; `REGISTER 1 {unit} = {k} meter` |

---

## 부록 A — Phase 4 추적 매트릭스 (요약)

| PRD 절 | Epic | Journey | Story | Gherkin |
|--------|------|---------|-------|---------|
| §1 G-01~05 | SC-01~10 | J1~J7 | US-01~07 | GH-BG, 1~8 |
| §3.3 | Background | J2 | US-01,07 | 3~8 |
| §6 POL-OUT | — | J4 | US-04 | 1~2 |
| §7 AC | SC | J7 | US | GH |

---

## 부록 B — 문서 변경 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 1.0 | Phase 4 기준 | Epic/Story/Gherkin/Level5 통합 초안 |

---

*본 PRD는 UnitConverter_12 README 및 Phase 4 산출물과 정합.*
