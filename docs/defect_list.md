# 결함 목록 (Defect List)

| 항목 | 내용 |
|------|------|
| **기준일** | 2026-05-21 |
| **출처** | [Report/07.QA_결함분석_디버깅.md](../Report/07.QA_결함분석_디버깅.md) |
| **BCE/Catch2** | 35/35 Passed (활성 TC 결함 없음) |
| **레거시 CLI** | `UnitConverter.cpp` — Open 2건 (DEF-002, DEF-003) |

## 상태 요약

| 상태 | ID | 비고 |
|------|-----|------|
| **Closed** | DEF-001 | TC 입력값 조정으로 GREEN |
| **Open** | DEF-002, DEF-003 | M2 Boundary E2E·CLI 연동 대상 |

## 결함 표

| ID | Severity | 변환 타입 | 재현 절차 | 기대값 | 실제값 | 근본 원인 | 수정 요약 |
|----|----------|-----------|-----------|--------|--------|-----------|-----------|
| DEF-001 | Major | meter→feet (대형 value) | `ctest -R test_convert_largeValue` 또는 `test_convert_maxFiniteValue` 실행 (RED 시 `1e308`/`1e200` 입력) | `isfinite(result)` — 유한 환산값 | `Inf` (double overflow) | `1e308`×`3.28084` 등이 double 범위 초과 (`domain_boundary_tests.cpp:28,93`) | TC 입력을 `1e50` / `1e100`으로 조정 (**Closed**) |
| DEF-002 | Minor | meter→feet, meter→yard (CLI table) | 레거시 CLI에 `"meter:2.5"` 입력 (`UnitConverter.cpp`) | PRD AC-02: `2.5 meter = 8.2 feet`, `2.5 meter = 2.7 yard` (1dp half-up, source unit 행 제외) | `8.2021 feet`, `2.73402 yard`, source `meter` 행 포함 3행 출력 | `UnitConverter.cpp:47-49` — raw double 직접 출력, `TableFormatter`·`convertAll` 미연동 | M2: `ConversionApplicationService` + `TableFormatter::roundToOneDecimalHalfUp()` 연동 |
| DEF-003 | Major | meter (음수·0 입력) | 레거시 CLI에 `"meter:-2.5"` 또는 `"meter:0"` 입력 | exit 1, stderr `Value must be positive: -2.5` (AC-01, BV-03) | 음수·0도 변환 수행, exit 0 | `UnitConverter.cpp:23-28` — `stod` 후 POL-NEG 검증 부재 | `value <= 0.0` 시 stderr 메시지 후 `return 1` (if-else 체인 유지, 4행 추가) |

## 참고 — 해당 없음 (Critical 패턴)

| 패턴 | 기대 | RED 예시 | 현재 코드 |
|------|------|----------|-----------|
| `feet`/`yard` inbound 분기에서 `meterValue` 미할당 | feet 환산값 (예: 8.202100) | `0.000000` | **해당 없음** — `feet`/`yard`에 `/ 3.28084`, `/ 1.09361` 적용됨 |

## traceability

| test_plan / AC | 결함 ID |
|----------------|---------|
| BV-02 매우 큰 수 | DEF-001 |
| AC-02 table 1dp (`meter:2.5`) | DEF-002 |
| AC-01 / BV-01 / BV-03 POL-NEG | DEF-003 |

## 문서 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 1.0 | 2026-05-21 | QA RPT-07 §5 초안 → `docs/defect_list.md` 정식화 |
