# 대화형 프롬프트 Export — UnitConverter_12 (TDD 전주기 검증·커버리지 종합)

| 항목 | 내용 |
|------|------|
| **대응 보고서** | [Report/202605211_UnitConverter_C++_Verification_Report.md](../Report/202605211_UnitConverter_C++_Verification_Report.md) |
| **선행 Prompt Export** | [18.REFACTOR_전체검증_커버리지측정-Prompt.md](./18.REFACTOR_전체검증_커버리지측정-Prompt.md) (REFACTOR 후 1차 검증) · [13.GREEN_전체검증_커버리지측정-Prompt.md](./13.GREEN_전체검증_커버리지측정-Prompt.md) |
| **Export 일자** | 2026-05-21 |
| **워크스페이스** | `c:\DEV\UnitConverter_12` |
| **형식** | Turn 기반 대화형 transcript (User → Assistant 산출 요약) |

---

## 사용 안내

- 각 **Turn**의 `User` 블록은 실제 입력 프롬프트 원문이다.
- `Assistant` 블록은 해당 턴에서 생성된 **산출물 요약**이다 (전체 응답 본문 아님).
- Turn 1: **커버리지 점검** — lcov/gcov · Invariant 미커버 분기 · 추가 TC 제안 (구현 금지).
- Turn 2: **TDD 전주기 종합 보고서** — `Report/202605211_UnitConverter_C++_Verification_Report.md`.
- Turn 3: 본 Export (문서만).
- 재현 시 Turn 1→2 순서 권장.
- **선행 전제**: RPT-17 REFACTOR 7커밋 완료 · Golden Master 4/4 · Dual-Track 13/14 GREEN · BCE 35/35 · RPT-18 검증 완료.

---

## Turn 1 — 커버리지 점검 (lcov / gcov · Invariant 분석)

### User

```
프로젝트: @c:\DEV\UnitConverter_12\ 

현재 테스트 커버리지를 점검해줘.
- 실행: # gcov / lcov && gcov UnitConverter.cpp
- 커버되지 않은 줄 중 Invariant 관련 분기 우선 식별
  (특히: meter↔feet/yard 비율 변환 분기, 음수 입력 처리, 없는 단위 처리)
- Domain Logic 95% 미달 시: 누락 테스트 케이스 제안 (구현 코드 추가 금지)
- Boundary 85% 미달 시: 누락 계약 테스트 케이스 제안

출력:
| 파일 | Stmts | Miss | Cover | 미달 원인 | 추가 테스트 제안 |
```

### Assistant (산출 요약)

**MinGW `build_cov` 재빌드·측정**:

```powershell
$env:Path = "C:\mingw64\bin;C:\ProgramData\chocolatey\bin;" + $env:Path
cmake -S . -B build_cov -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build_cov -j 4
lcov --directory build_cov --zerocounters
ctest --test-dir build_cov --output-on-failure
lcov --capture --directory build_cov --output-file build_cov/coverage.info --ignore-errors gcov
lcov --extract build_cov/coverage.info "*/src/domain/*" -o build_cov/domain.info --ignore-errors gcov
lcov --extract build_cov/coverage.info "*/src/boundary/*" -o build_cov/boundary.info --ignore-errors gcov
lcov --summary build_cov/domain.info
lcov --summary build_cov/boundary.info
echo "meter:2.5" | .\build_cov\unit_converter_cli.exe
cd build_cov\CMakeFiles\unit_converter_cli.dir; gcov -b UnitConverter.cpp.gcda
```

**ctest**: **49/50 PASS** (RED 1건: `json_format_returns_valid_schema`).

**레이어별 lcov**:

| 레이어 | Stmts | Hit | Miss | Cover | 목표 | 판정 |
|--------|------:|----:|-----:|------:|------|------|
| Domain | 92 | 84 | 8 | **91.3%** | ≥95% | ❌ |
| Boundary | 107 | 85 | 22 | **79.4%** | ≥85% | ❌ |
| UnitConverter.cpp (gcov) | 23 | 18 | 5 | **78.3%** | 참고 | CLI happy path |

**Invariant 미커버 분기 (우선순위)**:

| Invariant | 상태 | 미커버 위치 |
|-----------|------|-------------|
| meter↔feet/yard 비율 | ✅ 양호 | `ConversionEngine`·`LegacyCliConversion` 핵심 경로 커버 |
| 음수 입력 (POL-NEG) | ⚠️ 부분 | CLI `main()` L26–29; `ConversionApplicationService` L52 |
| 없는 단위 | ⚠️ 부분 | CLI L33–35; `InputParser` EmptyUnit L56–58 |

**파일별 표 (요약)** — User 요청 형식으로 출력:

| 파일 | Stmts | Miss | Cover | 미달 원인 | 추가 테스트 제안 |
|------|------:|-----:|------:|-----------|------------------|
| `ConversionEngine.cpp` | 17 | 1 | 94.1% | return 계측 | `convertAll("yard",1.0)` |
| `UnitCatalog.cpp` | 50 | 5 | 90.0% | `empty()` 미호출 | `clear()` 후 `empty()` 검증 |
| `TableFormatter.cpp` | 9 | 7 | 22.2% | `formatLine()` 0% | `formatLine` 1dp 문자열 TC |
| `CliErrorMessages.hpp` | 4 | 4 | 0.0% | CLI stderr 미실행 | subprocess E2E 3건 |
| `InputParser.cpp` | 50 | 10 | 80.0% | unit명·부분 파싱 | `:2.5`, `123:1.0`, `meter@:1` |
| Domain 합계 | 92 | 8 | 91.3% | +4 line 필요 | yard 역변환·`add(NaN)` 등 |
| Boundary 합계 | 107 | 22 | 79.4% | +6 line 필요 | TableFormatter + CLI E2E + TC-A-05 |

**코드 변경**: 본 Turn **없음** (측정·분석·TC 제안만).

---

## Turn 2 — TDD 전주기 종합 보고서 작성

### User

```
지금까지 작업한 내용을 Report 폴더에 보고서로 내보내줘.
프로젝트: @c:\DEV\UnitConverter_12\ 

포함 항목:
1. 작업 개요 (브랜치·날짜·작업자)
2. 완료된 To-Do 항목 요약 (Phase 6 To-Do 기준)
3. RED 단계 결과 (작성한 테스트 목록 + 실패 확인 여부)
4. GREEN 단계 결과 (통과한 테스트 + 커밋 메시지)
5. Refactoring 결과 (선택 항목·변경 파일·회귀 테스트 통과 여부)
6. 커버리지 현황 (레이어별 수치)
7. 미완료 항목 및 다음 단계 제안
8. 발견된 이슈 및 해결 방법
9. 생성형 AI 활용 회고 (도움이 된 순간 / 한계 / TC 작성 팁)

파일명 규칙: Report/YYMMDDn_UnitConverter_C++_[단계]_Report.md
```

### Assistant (산출 요약)

- [06](./06.Catch2_TDD_GREEN_구현.md) · [09](./09.DualTrack_RED_테스트작성.md) · [12](./12.DualTrack_TDD_GREEN_증분구현.md) · [17](./17.DualTrack_REFACTOR_CLI_BCE분리.md) · [18](./18.REFACTOR_전체검증_커버리지측정.md) 형식·내용 통합
- **RPT-2026-05-21-202605211**, TDD RED → GREEN → REFACTOR → 커버리지 재측정 **누적 종합**
- §1~11: 작업 개요 · Phase 6 To-Do · RED/GREEN/REFACTOR · lcov · 미완료 · 이슈 · AI 회고 · 결론
- **핵심**: 기능 회귀 0 · 아키텍처 목표 충족 · 커버리지 게이트 미달 · README [x] 미동기화
- **산출 파일**: `Report/202605211_UnitConverter_C++_Verification_Report.md`

---

## Turn 3 — 본 Export (현재 턴)

### User

```
현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해줘.
파일명규칙: (Report보고서 파일명)-Prompt.md
```

### Assistant (산출 요약)

- 본 문서 생성
- **산출 파일**: `Prompting/202605211_UnitConverter_C++_Verification_Report-Prompt.md`

---

## 산출물 매핑 (Turn → 파일)

| Turn | 작업 | 저장소 파일 |
|------|------|-------------|
| 1 | lcov/gcov · Invariant 분석 · TC 제안 표 | `build_cov/domain.info`, `build_cov/boundary.info`, `.gcov` 산출물 |
| 2 | TDD 전주기 종합 보고서 | `Report/202605211_UnitConverter_C++_Verification_Report.md` |
| 3 | Prompt Export | `Prompting/202605211_UnitConverter_C++_Verification_Report-Prompt.md` |

**의도적 미변경**: 구현 코드 · `json_format_returns_valid_schema` (RED) · DEF-002·003 Open · README [x] · 커버리지 게이트 미달 해소 코드.

**선행 세션 산출** ([18.REFACTOR_전체검증_커버리지측정-Prompt.md](./18.REFACTOR_전체검증_커버리지측정-Prompt.md)): REFACTOR 후 6항목 체크리스트 · RPT-18.

---

## Phase 재현용 — 축약 연속 프롬프트 (복사용)

> 새 세션에서 Turn 1~2를 한 번에 재현할 때:

```
워크스페이스: c:\DEV\UnitConverter_12
전제: REFACTOR 7커밋 완료 — GM 4/4, Dual-Track 13/14, BCE 35/35, RPT-18 존재.

[Turn 1] 커버리지 점검 + Invariant 미커버 분석
Step 1: MinGW build_cov 재빌드
  cmake -S . -B build_cov -G "MinGW Makefiles" -DENABLE_COVERAGE=ON
  cmake --build build_cov -j 4

Step 2: lcov
  lcov --zerocounters → ctest → capture → extract domain/boundary → --summary
  기대: Domain 91.3%, Boundary 79.4% (게이트 미달)

Step 3: gcov UnitConverter.cpp
  echo "meter:2.5" | .\build_cov\unit_converter_cli.exe
  gcov -b UnitConverter.cpp.gcda

Step 4: .gcov 역추적 — Invariant 우선
  - meter↔feet/yard: ConversionEngine/LegacyCli (대체로 커버)
  - 음수: CLI main L26-29, Service L52
  - unknown unit: CLI L33-35, InputParser L56-58

Step 5: 출력 표 (구현 코드 추가 금지)
  | 파일 | Stmts | Miss | Cover | 미달 원인 | 추가 테스트 제안 |
  Domain ≥95% / Boundary ≥85% 미달 시 TC 제안만.

[Turn 2] Report/202605211_UnitConverter_C++_Verification_Report.md
  9항목 + AI 회고, RED→GREEN→REFACTOR 누적, Phase 6 To-Do 기준.
```

---

## Turn 1 단독 재현용 — 커버리지 + Invariant 표

```text
@c:\DEV\UnitConverter_12\ @docs/test_plan.md §7 @Report/18.REFACTOR_전체검증_커버리지측정.md

현재 테스트 커버리지를 점검해줘.
- 실행: gcov / lcov && gcov UnitConverter.cpp (MinGW build_cov)
- Invariant 미커버 우선: meter↔feet/yard, 음수, unknown unit
- Domain <95% / Boundary <85% → 누락 TC 제안 (구현 금지)

출력:
| 파일 | Stmts | Miss | Cover | 미달 원인 | 추가 테스트 제안 |
```

---

## Turn 2 단독 재현용 — 종합 보고서 9항목

```text
@c:\DEV\UnitConverter_12\ @Report/06.Catch2_TDD_GREEN_구현.md @Report/12.DualTrack_TDD_GREEN_증분구현.md @Report/17.DualTrack_REFACTOR_CLI_BCE분리.md @Report/18.REFACTOR_전체검증_커버리지측정.md

지금까지 작업한 내용을 Report 폴더에 보고서로 내보내줘.

포함: 1.개요 2.Phase6 To-Do 3.RED 4.GREEN+커밋 5.REFACTOR 6.커버리지 7.미완료 8.이슈 9.AI회고

파일명: Report/202605211_UnitConverter_C++_Verification_Report.md
```

---

## Turn 2 연계 — 커버리지 게이트 충족 (종합 보고 §7 P0)

```text
@c:\DEV\UnitConverter_12\ @src/domain/ @src/boundary/ @tests/ @docs/test_plan.md §7.4

커버리지 게이트 미달 해소 (테스트만, 구현 금지):
- Domain 91.3% → ≥95% (+4 line):
  · UnitCatalog::empty() after clear()
  · add(NaN) → InvalidMetersPerUnitError
  · yard→meter / convertAll("yard",1.0)
- Boundary 79.4% → ≥85% (+6 line):
  · TableFormatter::formatLine 1dp TC
  · CLI subprocess stderr (CliErrorMessages)
  · InputParser :2.5, 123:1.0, meter@:1
  · Dual-Track TC-A-05 service.convert("meter:abc")

완료 판정:
  lcov --summary build_cov/domain.info   → Line ≥ 95.0%
  lcov --summary build_cov/boundary.info → Line ≥ 85.0%
```

---

## Turn 2 연계 — README [x] 동기화

```text
@c:\DEV\UnitConverter_12\ @README.md

Dual-Track 13/14 GREEN · GM 4/4 · defect_list 생성 완료 상태에 맞게
README "RED 단계 To-Do" 체크박스 [x] 갱신 (사용자 요청 시만).
TC-A-05·json RED·커버리지 목표는 ⬜ 유지.
```

---

## 주의 (프롬프트 해석)

| 항목 | User 요청 | Assistant·문서 처리 |
|------|-----------|---------------------|
| `&&` in shell | gcov / lcov && gcov | PowerShell → **`;`** · MinGW **`build_cov`** |
| Domain 95% | lcov extract | **91.3% 미달** — `UnitCatalog::empty()` 등 |
| Boundary 85% | lcov extract | **79.4% 미달** — `TableFormatter::formatLine` 0% |
| 구현 금지 | TC 제안만 | Turn 1 **코드 diff 없음** |
| Phase 6 To-Do | README 기준 | Track A/B + GM + 커버리지 + 결함 통합 |
| 보고서 파일명 | `YYMMDDn_..._[단계]_Report.md` | **`202605211_UnitConverter_C++_Verification_Report.md`** |
| Prompt 파일명 | `(Report)-Prompt.md` | **`202605211_UnitConverter_C++_Verification_Report-Prompt.md`** |
| Invariant meter↔feet/yard | 미커버 우선 | **비즈니스 분기는 커버됨** — return 계측·유틸만 잔존 |
| TC-A-05 | Track A 7/7 | Dual-Track **미작성** — BCE InputParser만 PASS |
| 응답 언어 | (user rule) | 한국어 |

---

## 검증 결과 요약 (본 세션 Turn 1)

### ctest (MinGW build_cov)

| 구분 | Passed | Failed |
|------|--------|--------|
| BCE Domain | 28/28 | 0 |
| BCE Data | 7/7 | 0 |
| Dual-Track | 13/14 | 1 (json RED) |
| Golden Master (#50) | 1/1 | 0 |
| **합계** | **49/50** | **1** |

### lcov (MinGW build_cov)

| 레이어 | Line | 목표 | 판정 |
|--------|------|------|------|
| Domain (`src/domain/*`) | 91.3% (84/92) | ≥ 95% | ❌ |
| Boundary (`src/boundary/*`) | 79.4% (85/107) | ≥ 85% | ❌ |
| `UnitConverter.cpp` (gcov) | 78.3% (18/23) | 참고 | 에러 분기 미커버 |

### Domain 파일별 (lcov --list)

| 파일 | Cover | Miss | Invariant |
|------|------:|-----:|-----------|
| `ConversionEngine.cpp` | 94.1% | 1 | meter↔feet/yard ✅ |
| `LegacyCliConversion.cpp` | 90.9% | 1 | CLI convertAll ✅ |
| `LengthQuantity.cpp` | 91.7% | 1 | POL-NEG ✅ |
| `UnitCatalog.cpp` | 90.0% | 5 | unknown ✅ · empty() ❌ |

### Boundary 파일별 (lcov --list)

| 파일 | Cover | Miss | Invariant |
|------|------:|-----:|-----------|
| `InputParser.cpp` | 80.0% | 10 | EmptyUnit·부분 파싱 ❌ |
| `TableFormatter.cpp` | 22.2% | 7 | formatLine ❌ |
| `CliErrorMessages.hpp` | 0.0% | 4 | CLI stderr ❌ |
| `ValueValidator.cpp` | 100% | 0 | POL-NEG ✅ |

---

## 커버리지 게이트 달성용 TC (Turn 1 산출 — 구현 없음)

### Domain ≥95% (+4 line)

1. `UnitCatalog::empty()` — `clear()` 후 `REQUIRE(catalog.empty())`
2. `UnitCatalog::add("x", NaN)` → `InvalidMetersPerUnitError`
3. `engine.convert("yard", 1.0, "meter")` ≈ 0.9144
4. `engine.convertAll("yard", 1.0)` — source 제외 2건

### Boundary ≥85% (+6 line)

1. `TableFormatter::formatLine("2.5","meter",8.2021,"feet")` → `"2.5 meter = 8.2 feet"`
2. CLI subprocess: `meter2.5` / `parsec:1.0` → exit 1 + stderr
3. InputParser: `:2.5`, `123:1.0`, `meter@:1`
4. Dual-Track TC-A-05: `service.convert("meter:abc")` → `invalid_argument`

---

## 문서 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 1.0 | 2026-05-21 | 최초 Export (Turn 1~3, 대응 보고서 `202605211_UnitConverter_C++_Verification_Report.md`) |
