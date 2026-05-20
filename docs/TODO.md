# To-Do 리스트 — UnitConverter (C++)

**기준 문서**: [PRD v1.0](./PRD.md) (Phase 5)  
**저장소**: UnitConverter_12  
**v1.0 릴리스 정의**: 필수(Must) 전항 완료 + AC-01~03·AC-07 통과 + 회귀 체크리스트 전항

---

## 🔴 필수 (Must-Have) — v1.0 릴리스 차단 항목

| ☐ | 작업 설명 | 연관 PRD | 완료 기준 (누가·무엇을·어떻게) |
|:-:|-----------|----------|--------------------------------|
| ☐ | CMake + Catch2 프로젝트 골격 및 Domain/Boundary/Data/통합 타깅 분리 | §4.1, §4.2, G-01 | **학습자**가 `ctest` 실행 시 4타깅 빌드·테스트 바이너리가 분리되어 GREEN이고, **리뷰어**가 Domain 타깅이 Boundary/Data를 link/include하지 않음을 확인하면 통과 |
| ☐ | Domain `UnitCatalog` + `ConversionEngine` (meter 허브 선형 환산) | F-02, F-03, §5.1, G-01 | **학습자**가 DT-01·DT-03·DT-13(또는 동등 TC) 전건 GREEN, feet↔yard meter 경유 일관·허용오차 §4.3 충족 시 통과 |
| ☐ | Domain `LengthQuantity`·`MetersPerUnit` 검증 (POL-NEG Domain 경계) | POL-NEG, F-05, IN-04 | **학습자**가 음수·0·NaN `create` 실패 TC GREEN이면 통과 |
| ☐ | Boundary CONVERT 파서 (`unit:value`) | F-01, IN-01~03, §3.3 | **학습자**가 `meter:2.5` 파싱 성공·`meter2.5`/`meter:abc` 실패 TC GREEN, 실패 시 Domain Mock 호출 0회이면 통과 |
| ☐ | Boundary POL-NEG 검증 (음수·0 stdin 거부) | POL-NEG-02~03, GH-6~7 | **학습er**가 `meter:-2.5`·`feet:0` → exit 1, stderr `Value must be positive: {value}` 바이트 일치 TC GREEN이면 통과 |
| ☐ | Boundary 미지원 단위 처리 | F-05, GH-8, US-07 | **학습자**가 `stone:2.5` → exit 1, `Unknown unit: stone`, stdout·부분 줄 없음 TC GREEN이면 통과 |
| ☐ | Boundary table Formatter + POL-OUT | F-04, §6.1~6.2, GH-1~2 | **학습자**가 `meter:2.5` → `2.5 meter = 8.2 feet`·`2.5 meter = 2.7 yard` 2줄, `feet:8.2` 모든 줄 `8.2 feet =` prefix TC GREEN이면 통과 |
| ☐ | Control `ConversionApplicationService` (Catalog 로드→convert→format) | §4.2, SCN-01 | **학습자**가 InMemory Catalog만으로 E2E `meter:2.5` exit 0 TC 1건 GREEN이면 통과 |
| ☐ | 기본 3단위 InMemory Catalog 부트스트랩 | §5.1, Background | **학습자**가 meter=1.0, feet=0.3048, yard=0.9144 로드 TC가 README 비율 검증과 함께 GREEN이면 통과 |
| ☐ | §3.3 필수 에러 코드 4종 (FORMAT/NUMBER/NEGATIVE/UNKNOWN) | §3.3, AC-01 | **학습자**가 4종 stderr 스냅샷 diff 0·exit 1 TC GREEN, **리뷰어**가 메시지 치환 외 바이트 동일 확인 시 통과 |
| ☐ | Golden trio table 스냅샷 등록 | RR-02, G-03, AC-02 | **학습자**가 입력 `{1.0, 2.5, 0.1}` × `meter:` 스냅샷 3건 저장 후 재실행 diff 0이면 통과 |
| ☐ | Domain 커버리지 게이트 | §4.3, AC-06, G-01 | **학습자**가 커버리지 리포트 제출, **강사**가 Domain Line≥95%·Branch≥90% 확인 시 통과 |
| ☐ | AC-07 구조 인수 (Engine 확장 diff 0) | AC-07, G-04, F-03 | **리뷰어**가 포맷·단위 확장 PR에서 `ConversionEngine` 시그니처 diff 0 확인 시 통과 |

---

## 🟡 권장 (Should-Have) — 품질 향상 항목

| ☐ | 작업 설명 | 연관 PRD | 완료 기준 (누가·무엇을·어떻게) |
|:-:|-----------|----------|--------------------------------|
| ☐ | JSON 설정 파일 로드 (`schema_version:1`, `base_unit:meter`) | F-06, §5.2, AC-05 | **학습자**가 유효 `config/units.json` 로드 ST-01 GREEN, **강사**가 k 3값이 §5.1과 일치 확인 시 통과 |
| ☐ | 설정 로드 실패 (깨진 JSON·빈 units·중복 name) | F-06, SCN-03, AC-05 | **학습자**가 실패 fixture ≥3종 → exit 2, `Failed to load config: {path}`, 변환 0회 TC GREEN이면 통과 |
| ☐ | `REGISTER 1 {unit} = {k} meter` 파싱·Catalog 반영 | F-07, §5.3, AC-04 | **학습자**가 cubit 등록 후 `cubit:2` → meter≈0.9144 TC GREEN이면 통과 |
| ☐ | REGISTER 실패 (문법·중복·k≤0) | §5.3, §3.3 | **학습자**가 3종 stderr 스냅샷·exit 1 TC GREEN이면 통과 |
| ☐ | REGISTER 후 Golden trio 불변 (RR-04) | RR-04, AC-04 | **학습자**가 cubit 등록 후 `meter:2.5` 스냅샷 diff 0이면 통과 |
| ☐ | `--format json` 출력 | F-08, §6.3 | **학습자**가 `source`·`conversions[]` 필드·길이 N−1·well-formed TC ≥3 GREEN이면 통과 |
| ☐ | `--format csv` 출력 | F-08, §6.4 | **학습자**가 헤더 고정·4필드·N−1행 TC ≥3 GREEN이면 통과 |
| ☐ | 포맷 간 target unit 집합 동일 | §6.5 | **학습자**가 동일 입력에 table/json/csv target 이름 집합 equality TC GREEN이면 통과 |
| ☐ | Boundary 계약 TC ≥10 + stderr/stdout 스냅샷 | G-02, RR-03 | **리뷰어**가 계약 TC ≥10건 목록·전건 GREEN·스냅샷 diff 0 확인 시 통과 |
| ☐ | Boundary·Data·Control 커버리지 게이트 | §4.3, AC-06 | **강사**가 Boundary Line≥90%/Branch≥85%, Data ≥85%/80%, Control ≥80%/75% 확인 시 통과 |
| ☐ | 통합 E2E (정상≥3, 실패≥4) | G-05, SCN-01~03 | **학습자**가 IT 목록 7건 이상 제출·전건 GREEN이면 통과 |
| ☐ | README 빌드·실행·`docs/PRD.md` 링크 갱신 | —, 회귀 체크 | **학습자**가 README에 CMake·ctest·설정 예시 반영, **리뷰어**가 문서-PRD 수치 일치 확인 시 통과 |

---

## 🟢 선택 (Nice-to-Have) — v2.0 후보

| ☐ | 작업 설명 | 기대 가치 |
|:-:|-----------|-----------|
| ☐ | Traceability Matrix 문서 (Concept→Test 10행+) | F-09, SC-10; 인수·회고 시 객관 추적 |
| ☐ | Catch2 RED 제목 36건 ↔ invariant 매핑 표 | J4; TDD 방향 붕괴 방지 |
| ☐ | Gherkin 8 Scenario 자동화(Catch2 BDD 또는 스크립트) | GH-1~8 living documentation |
| ☐ | CI 워크플로(커버리지·스냅샷·RR-01 비율 잠금) | PR마다 회귀 무인 검증 |
| ☐ | YAML 설정 지원 (JSON과 동형 schema) | §5.2 운영 유연성 |
| ☐ | `.cursorrules` 8키 값 채우기 (forbidden·tdd_rules 구체화) | AI 보조 시 계약 이탈 감소 |

---

## 🔵 기술 부채 (Tech Debt)

| ☐ | 문제 설명 | 발생 원인 | 해결 방향 |
|:-:|-----------|-----------|-------------|
| ☐ | 비율 상수가 `UnitConverter.cpp` 분기에 하드코딩 | 절차형 출발 코드 | Catalog+설정 로드로 단일 출처; RR-01 게이트 |
| ☐ | 단일 `main`에 파싱·환산·출력 결합 | README 초기 샘플 구조 | BCE 레이어 분리 후 `main`은 Boundary 진입만 |
| ☐ | 테스트·CMake 부재 | g++ 단일 파일 빌드만 문서화 | §4.1 스택 도입, Domain-first RED |
| ☐ | table 1dp vs JSON 정밀도 이중 규칙 혼선 위험 | 표현 계층 요구 차이 | Formatter별 TC 분리·PRD §6 명시 유지 |
| ☐ | README 예시 반올림(8.2/2.7)과 내부 k 소수 혼용 | 표시·계산 정책 미분리 | target만 1dp half-up, JSON은 full precision |
| ☐ | PRD·Gherkin·README 3원천 동기화 부담 | Phase 4~5 문서 병행 작성 | 계약 변경 시 RR-05(버전 bump+Gherkin 동시 수정) |

---

## ✅ 완료 항목 (Done)

| ☑ | 완료 내용 | 완료일 | 관련 커밋/PR 메시지 |
|:-:|-----------|--------|---------------------|
| [x] | Phase 5 PRD v1.0 (`docs/PRD.md`) 작성 | 2026-05-20 | docs: add PRD v1.0 from Phase 4 requirements |
| [x] | 절차형 출발점 `UnitConverter.cpp` (meter/feet/yard 기본 동작) | (기존) | initial: procedural unit converter baseline |
| [x] | README 요구사항·6시간 Activities 정의 | (기존) | docs: README training requirements |
| [x] | `.cursorrules` YAML 뼈대 8키 | 2026-05-20 | chore: add cursorrules skeleton |

---

## 📋 회귀 방지 체크리스트 (PRD §7.2 기반)

**v1.0 태그/제출 전 — 담당: 학습자 실행, 강사/리뷰어 승인**

| ☐ | 항목 | PRD | 통과 조건 |
|:-:|------|-----|-----------|
| ☐ | 계약 테스트 | RR-03, G-02 | 계약 TC ≥10 GREEN, stderr/stdout 스냅샷 diff 0 |
| ☐ | Golden trio | RR-02, G-03 | `{1.0, 2.5, 0.1}` meter table 스냅샷 diff 0 |
| ☐ | REGISTER 후 golden | RR-04 | cubit 등록 후 `meter:2.5` 스냅샷 diff 0 |
| ☐ | 비율 잠금 게이트 | RR-01 | feet `meters_per_unit` 의도 변경 시 관련 TC FAIL 확인(잠금 동작) |
| ☐ | AC 전항 | §7.1 | AC-01~07 체크박스 전부 ☑, 증거(로그·리포트) 첨부 |
| ☐ | 커버리지 | §4.3, AC-06 | Domain/Boundary/Data(·Control) 임계값 충족 리포트 |
| ☐ | Gherkin 정합 | RR-05, GH-1~8 | GH-1~8 수동 또는 자동 TC와 PRD §3.3·POL 일치 |
| ☐ | README 갱신 | — | 빌드·실행·설정·포맷·PRD 링크가 현행 동작과 일치 |
| ☐ | PRD 버전 | RR-05 | 계약 수치·문구 변경 시 PRD 버전 bump + Gherkin 동시 수정 기록 |

---

## 🗓️ 마일스톤

| 마일스톤 | 포함 항목 (PRD) | 목표일 | 상태 |
|----------|-----------------|--------|------|
| **M0 문서·계약** | PRD, POL-NEG/OUT, Gherkin(참조), TODO | 2026-05-20 | ✅ 완료 |
| **M1 Domain GREEN** | F-02, F-03, §5.1, G-01, AC-07(부분) | T+1일 (실습 2h) | 🔲 대기 |
| **M2 Boundary·table v1** | F-01, F-04, F-05, AC-01~03, GH-1~8(필수) | T+2일 | 🔲 대기 |
| **M3 권장 기능** | F-06, F-07, F-08, AC-04~06, SCN-02~03 | T+3일 (실습 +2h) | 🔲 대기 |
| **M4 v1.0 릴리스** | Must 전항, 회귀 체크리스트, README, AC-01~07 | T+4일 (회고 전) | 🔲 대기 |

*T = 실습 시작일(팀별 지정). 6시간 Activities: M1≈2h, M2≈0.5h TC+M1 tail, M3≈2h, M4≈1h 회고·인수.*

---

## 문서 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 1.0 | 2026-05-20 | PRD §3·§7 기반 초안 |
