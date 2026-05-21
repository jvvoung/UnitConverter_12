# UnitConverter_12 — TDD 전주기 검증 종합 보고서

| 항목 | 내용 |
|------|------|
| **문서 ID** | RPT-2026-05-21-202605211 |
| **파일명** | `Report/202605211_UnitConverter_C++_Verification_Report.md` |
| **작성 기준일** | 2026-05-21 |
| **대상 저장소** | UnitConverter_12 (`c:\DEV\UnitConverter_12`) |
| **브랜치** | `Refactoring` |
| **작업자** | KaneUng (`jwadeung98@gmail.com`) |
| **선행 보고서** | [06](./06.Catch2_TDD_GREEN_구현.md) · [09](./09.DualTrack_RED_테스트작성.md) · [12](./12.DualTrack_TDD_GREEN_증분구현.md) · [17](./17.DualTrack_REFACTOR_CLI_BCE분리.md) · [18](./18.REFACTOR_전체검증_커버리지측정.md) |
| **기준 문서** | [docs/PRD.md](../docs/PRD.md) · [docs/test_plan.md](../docs/test_plan.md) · [README.md](../README.md) · [docs/defect_list.md](../docs/defect_list.md) |
| **보고 범위** | Phase 6 실습 To-Do 기준 — RED → GREEN → REFACTOR → 커버리지 재측정까지 누적 현황 |

---

## 1. 작업 개요

### 1.1 세션 정보

| 항목 | 값 |
|------|-----|
| **브랜치** | `Refactoring` (최신: `3680280`) |
| **작업 기간** | 2026-05-20 ~ 2026-05-21 |
| **작업자** | KaneUng |
| **주요 마일스톤** | M0 문서 ✅ → BCE GREEN ✅ → Dual-Track GREEN ✅ → CLI REFACTOR ✅ → 전체 검증·커버리지 🔶 |
| **본 세션 작업** | MinGW `build_cov` 재빌드 · `ctest` 50건 · `lcov` Domain/Boundary · `gcov UnitConverter.cpp` · 미커버 Invariant 분기 분석 |

### 1.2 커밋 타임라인 (요약)

| 단계 | 커밋 범위 | 건수 | 대표 메시지 |
|------|-----------|------|-------------|
| BCE GREEN | 초기 ~ `37472a9` | — | `feat`: Domain/Boundary/Data Catch2 35건 |
| Dual-Track RED | `eef940e` ~ `7671910` | 2 | `test: add Dual-Track RED Catch2 suite` |
| Dual-Track GREEN | `217fabc` ~ `da11d69` | 9 | `feat(green): meter to feet` … `loadConfig with fallback` |
| Golden Master | `f2c01dd` ~ `5e48e46` | 2 | `test: add Golden Master approval regression tests` |
| CLI REFACTOR | `c3a1ae6` ~ `9e52056` | 7 | `refactor(domain): replace if-else with UnitRegistry` … |
| 검증·문서 | `1c22db1` ~ `3680280` | 4 | `docs: add GREEN/REFACTOR verification reports` |

### 1.3 검증 환경

| 항목 | MSVC (`build/`) | MinGW (`build_cov/`) |
|------|-----------------|----------------------|
| OS | Windows 10.0.19045 | 동일 |
| Generator | Visual Studio 17 2022 | MinGW Makefiles |
| Compiler | MSVC 19.44 | g++ 15.2.0 (MinGW64) |
| Catch2 | v3.5.4 (FetchContent) | 동일 |
| 커버리지 | 미지원 | `--coverage -O0 -g` |

---

## 2. 완료된 To-Do 항목 요약 (Phase 6 · README 기준)

> **Phase 6** = 6시간 실습 후반(TDD GREEN → REFACTOR → 검증) 및 README **RED 단계 To-Do** · **Golden Master To-Do** · **커버리지 목표**를 통합한 체크리스트.

### 2.1 Track A — UI / Boundary

| ID | 항목 | 구현·테스트 | README [x] | 비고 |
|----|------|-------------|:----------:|------|
| TC-A-01 | `meter:2.5` Happy Path | ✅ `[ui][green]` PASS | ⬜ | Service 6dp feet 1줄 반환 |
| TC-A-02 | `:` 없음 → 예외 | ✅ PASS | ⬜ | |
| TC-A-03 | 음수 → 예외 | ✅ PASS | ⬜ | |
| TC-A-04 | unknown unit → 예외 | ✅ PASS | ⬜ | |
| TC-A-05 | `meter:abc` → 예외 | 🔶 BCE만 PASS | ⬜ | Dual-Track TC **미작성** |
| TC-A-06 | POL-OUT prefix 보존 | ✅ PASS | ⬜ | |
| TC-A-07 | `meter:0` 거부 | ✅ PASS | ⬜ | |

### 2.2 Track B — Domain / Logic

| ID | 항목 | 구현·테스트 | README [x] | 비고 |
|----|------|-------------|:----------:|------|
| TC-B-01 | meter→feet | ✅ PASS | ⬜ | |
| TC-B-02 | meter→yard | ✅ PASS | ⬜ | |
| TC-B-03 | feet→meter | ✅ PASS | ⬜ | |
| TC-B-04 | convertAll | ✅ PASS | ⬜ | |
| TC-B-05 | registerUnit | ✅ PASS | ⬜ | |
| TC-B-06 | loadConfig 성공 | ✅ PASS | ⬜ | |
| TC-B-07 | loadConfig 실패 fallback | ✅ PASS | ⬜ | |

### 2.3 Golden Master · 커버리지 · 결함

| 구분 | 항목 | 상태 |
|------|------|------|
| Golden Master | GM-TC-01~04 + ctest #50 | ✅ **4/4 PASS** (코드·baseline 완료) |
| Golden Master CI | GM-07~09 (workflow·required check) | ⬜ 미착수 |
| 커버리지 | Domain ≥95% | ❌ **91.3%** |
| 커버리지 | Boundary ≥85% | ❌ **79.4%** |
| 결함 목록 | `docs/defect_list.md` 생성 | ✅ [x] |
| 결함 수정 | DEF-002·003 회귀 확인 | ⬜ Open 유지 |

### 2.4 RPT-18 6항목 아키텍처 체크리스트

| # | 항목 | 결과 |
|---|------|------|
| 1 | Dual-Track 13/14 GREEN | ✅ (TC-A-05 Dual-Track 갭) |
| 2 | Golden Master 4/4 | ✅ |
| 3 | if-else → UnitCatalog | ✅ |
| 4 | 매직 넘버 인라인 0 | ✅ |
| 5 | Domain/Boundary 분리 | ✅ |
| 6 | lcov 게이트 | ❌ |

---

## 3. RED 단계 결과

### 3.1 BCE Catch2 (test_plan P0 — 선행 GREEN 세션에서 RED 설계 후 즉시 GREEN)

| 파일 | TEST_CASE 수 | 태그 | RED 확인 |
|------|:-----------:|------|----------|
| `domain_normal_conversion_tests.cpp` | 6 | `[domain][normal]` | N/A (GREEN 직행) |
| `domain_boundary_tests.cpp` | 7 | `[domain][boundary]` | N/A |
| `domain_exception_tests.cpp` | 8 | `[domain][exception]` | N/A |
| `domain_register_tests.cpp` | 7 | `[domain][register]` | N/A |
| `data_config_tests.cpp` | 7 | `[domain][data]` | N/A |
| **소계** | **35** | — | ✅ **35/35 PASS** |

### 3.2 Dual-Track RED (의도적 FAIL)

| 파일 | TEST_CASE | 태그 | RED 확인 |
|------|-----------|------|----------|
| `ui_boundary_red_tests.cpp` | TC-A-01~06 + JSON RED | `[ui][red]` → 이후 `[green]` | ✅ 초기 LNK2019 / `FAIL("RED")` |
| `domain_logic_red_tests.cpp` | TC-B-01~07 | `[logic][red]` → `[green]` | ✅ 초기 LNK2019 |
| **소계** | **14** | — | 🔶 **13 GREEN + 1 RED** |

**RED 잔존 1건**

```text
json_format_returns_valid_schema — ui_boundary_red_tests.cpp:40 — FAIL("RED") (PRD §6.3 JSON 미구현)
```

### 3.3 Golden Master RED (approve 패턴)

| TC | 입력 | RED→GREEN |
|----|------|-----------|
| GM-TC-01 | `meter:2.5` | baseline 생성 후 diff 0 |
| GM-TC-02 | `feet:1.0` | ✅ |
| GM-TC-03 | `yard:1.0` | ✅ |
| GM-TC-04 | `meter:0.0` | ✅ (DEF-003 Open — GM baseline 의도적 유지) |

---

## 4. GREEN 단계 결과

### 4.1 ctest 전체 (MinGW `build_cov`, 2026-05-21 재측정)

| 구분 | Passed | Failed | 비율 |
|------|--------|--------|------|
| BCE Domain | 28/28 | 0 | 100% |
| BCE Data | 7/7 | 0 | 100% |
| Dual-Track | 13/14 | 1 | 93% |
| Golden Master (#50) | 1/1 | 0 | 100% |
| **합계** | **49/50** | **1** | **98%** |

### 4.2 Dual-Track GREEN 9커밋 — 커밋 메시지

| # | TC | 커밋 | 메시지 |
|---|-----|------|--------|
| 1 | TC-B-01 | `217fabc` | `feat(green): meter to feet` |
| 2 | TC-A-02 | `98a86d3` | `feat(green): validate missing colon` |
| 3 | TC-B-02 | `723f553` | `feat(green): meter to yard` |
| 4 | TC-A-03 | `7710c15` | `feat(green): validate negative value` |
| 5 | TC-B-03 | `34f7211` | `feat(green): feet to meter reverse` |
| 6 | TC-A-04 | `adebb22` | `feat(green): validate unknown unit` |
| 7 | TC-B-04~05 | `5d952fa` | `feat(green): convertAll and registerUnit` |
| 8 | TC-A-01,06,07 | `1157f55` | `feat(green): boundary happy path` |
| 9 | TC-B-06~07 | `da11d69` | `feat(green): loadConfig with fallback` |

### 4.3 BCE GREEN 핵심 Invariant 검증 (대표 TC)

| Invariant | 대표 TEST_CASE | 결과 |
|-----------|----------------|------|
| meter→feet (3.28084) | `test_meter_to_feet_positiveValue_returnsFeetProduct` | ✅ |
| meter→yard (1.09361) | `test_meter_to_yard_positiveValue_returnsYardProduct` | ✅ |
| feet→meter 역변환 | `test_feet_to_meter_reverseConversion_returnsMeterQuotient` | ✅ |
| feet↔yard meter 허브 | `test_feet_to_yard_hubPath_matchesMeterIntermediate` | ✅ |
| POL-NEG Domain | `test_lengthQuantity_negativeValue_rejectsCreation` | ✅ |
| Unknown unit | `test_convert_unknownUnit_throwsUnknownUnit` | ✅ |
| REGISTER | `test_registerUnit_enablesConversion` | ✅ |

---

## 5. Refactoring 결과

### 5.1 7커밋 REFACTOR (선택 항목 — CLI Thin Controller)

| # | Track | 변경 | 커밋 | 회귀 |
|---|-------|------|------|------|
| 1 | Domain | if-else → `UnitCatalog` | `c3a1ae6` | GM 4/4 ✅ |
| 2 | Domain | `conversion_ratios.hpp` 추출 | `4874775` | ✅ |
| 3 | Domain | `ConversionEngine::convert()` meter 허브 | `8633394` | ✅ |
| 4 | Domain | `legacyCliConvertAll()` | `95696c8` | ✅ |
| 5 | Boundary | `InputParser` 분리 | `949b5e8` | ✅ |
| 6 | Boundary | `CliErrorMessages` | `e042dda` | ✅ |
| 7 | Boundary | `CliOutputFormatter` | `9e52056` | ✅ |

### 5.2 변경 파일 (REFACTOR 핵심)

| 파일 | REFACTOR 전 | REFACTOR 후 |
|------|-------------|-------------|
| `UnitConverter.cpp` | 53줄 — 파싱·if-else·인라인 상수·출력 | **47줄 Thin Controller** |
| `src/domain/conversion_ratios.hpp` | — | `METER_TO_FEET` / `METER_TO_YARD` |
| `src/domain/LegacyCliConversion.{hpp,cpp}` | — | CLI 전용 `convertAll` |
| `src/domain/UnitCatalog.cpp` | — | `bootstrap_legacy_cli_catalog()` |
| `src/boundary/CliOutputFormatter.{hpp,cpp}` | — | 6dp stdout 포맷 |
| `src/boundary/CliErrorMessages.hpp` | — | stderr 메시지 상수 |

### 5.3 회귀 테스트

| 대상 | REFACTOR 전 | REFACTOR 후 |
|------|-------------|-------------|
| Golden Master GM-TC-01~04 | 4/4 PASS | ✅ **4/4 유지** |
| BCE 35건 | 35/35 | ✅ **35/35 유지** |
| Dual-Track GREEN | 13/14 | ✅ **동일** |
| ctest 전체 | 49/50 | ✅ **49/50 유지** |

> **의도적 미변경**: DEF-002(6dp·3행 CLI) · DEF-003(0·음수 CLI 허용) — GM baseline·PRD 계약 전환은 후속 M2.

---

## 6. 커버리지 현황

**측정 명령** (MinGW `build_cov`):

```powershell
lcov --directory build_cov --zerocounters
ctest --test-dir build_cov --output-on-failure
lcov --capture --directory build_cov --output-file build_cov/coverage.info --ignore-errors gcov
lcov --extract build_cov/coverage.info "*/src/domain/*" -o build_cov/domain.info
lcov --extract build_cov/coverage.info "*/src/boundary/*" -o build_cov/boundary.info
echo "meter:2.5" | .\build_cov\unit_converter_cli.exe
gcov -b UnitConverter.cpp.gcda  # unit_converter_cli.dir
```

### 6.1 레이어별 lcov 요약

| 측정 대상 | Stmts | Hit | Miss | Cover | 목표 | 판정 |
|-----------|------:|----:|-----:|------:|------|------|
| **Domain** (`src/domain/*`) | 92 | 84 | 8 | **91.3%** | ≥95% | ❌ |
| **Boundary** (`src/boundary/*`) | 107 | 85 | 22 | **79.4%** | ≥85% | ❌ |
| **UnitConverter.cpp** (참고) | 23 | 18 | 5 | **78.3%** | — | CLI happy path 위주 |

### 6.2 Domain 파일별

| 파일 | Stmts | Miss | Cover | 미커버 Invariant 관련 |
|------|------:|-----:|------:|----------------------|
| `ConversionEngine.cpp` | 17 | 1 | 94.1% | meter↔feet/yard **커버됨** |
| `LegacyCliConversion.cpp` | 11 | 1 | 90.9% | CLI `legacyCliConvertAll` **커버됨** |
| `LengthQuantity.cpp` | 12 | 1 | 91.7% | 음수/0 **커버됨** |
| `LengthQuantity.hpp` | 2 | 0 | 100% | — |
| `UnitCatalog.cpp` | 50 | 5 | 90.0% | `empty()` 미호출; unknown unit throw **커버됨** |

### 6.3 Boundary 파일별

| 파일 | Stmts | Miss | Cover | 미커버 Invariant 관련 |
|------|------:|-----:|------:|----------------------|
| `InputParser.cpp` | 50 | 10 | 80.0% | 빈 unit·특수문자·부분 파싱 미실행 |
| `TableFormatter.cpp` | 9 | 7 | 22.2% | `formatLine()` **0%** (1dp POL-OUT) |
| `CliErrorMessages.hpp` | 4 | 4 | 0.0% | CLI stderr 에러 경로 미실행 |
| `ConversionApplicationService.cpp` | 27 | 1 | 96.3% | `meter:abc` → `return {}` (L52) |
| `CliOutputFormatter.cpp` | 6 | 0 | 100% | — |
| `ValueValidator.cpp` | 11 | 0 | 100% | POL-NEG **커버됨** |

### 6.4 Invariant 미커버 분기 (우선순위)

| Invariant | 상태 | 미커버 위치 |
|-----------|------|-------------|
| meter↔feet/yard 비율 | ✅ 양호 | 비즈니스 분기 아닌 return 계측만 잔존 |
| 음수 입력 | ⚠️ 부분 | CLI `main()` L26–29; Service L52 |
| 없는 단위 | ⚠️ 부분 | CLI L33–35; `InputParser` EmptyUnit L56–58 |

### 6.5 게이트 달성용 추가 테스트 제안 (구현 코드 추가 없음)

**Domain ≥95% (+4 line)**

1. `UnitCatalog::empty()` — `clear()` 후 검증
2. `UnitCatalog::add(NaN)` → `InvalidMetersPerUnitError`
3. `engine.convert("yard", 1.0, "meter")` 역변환
4. `engine.convertAll("yard", 1.0)` source 제외 2건

**Boundary ≥85% (+6 line)**

1. `TableFormatter::formatLine("2.5","meter",8.2021,"feet")` → `"2.5 meter = 8.2 feet"`
2. CLI subprocess: `meter2.5` / `parsec:1.0` → exit 1 + stderr
3. InputParser: `:2.5`, `123:1.0`, `meter@:1`
4. Dual-Track TC-A-05: `service.convert("meter:abc")` → `invalid_argument`

---

## 7. 미완료 항목 및 다음 단계 제안

| 우선순위 | 항목 | 기대 효과 |
|----------|------|-----------|
| **P0** | TC-A-05 Dual-Track GREEN (`meter:abc`) | README Track A 7/7 · Service L52 커버 |
| **P0** | `TableFormatter::formatLine` BCE TC | Boundary Line +7%p · DEF-002 해소 기반 |
| **P1** | CLI E2E subprocess (stderr 4종) | `CliErrorMessages` 0% → 커버 · DEF-003 |
| **P1** | `UnitCatalog::empty()` · InputParser 엣지 TC | Domain ≥95% |
| **P2** | `json_format_returns_valid_schema` GREEN | ctest 50/50 · M3 JSON |
| **P2** | README Track A/B [x] 동기화 | 문서-구현 정합 |
| **P3** | GM CI workflow (GM-07~09) | PR merge gate |
| **P3** | DEF-002·003 — GM baseline vs PRD 1dp·POL-NEG 재승인 | 계약 정합 |

---

## 8. 발견된 이슈 및 해결 방법

| ID | 이슈 | 심각도 | 해결 |
|----|------|--------|------|
| DEF-001 | `1e308`×비율 → `Inf` | Major | ✅ TC 입력 `1e50`/`1e100` 조정 (**Closed**) |
| DEF-002 | CLI 6dp·3행 vs AC-02 1dp 2행 | Minor | ⬜ Open — REFACTOR 후에도 GM baseline 유지 (의도) |
| DEF-003 | CLI 0·음수 허용 vs POL-NEG | Major | ⬜ Open — Service/Validator는 거부, CLI는 미연동 |
| I-01 | Dual-Track TC-A-05 갭 | Medium | BCE `test_inputParser_nonNumericToken`만 PASS |
| I-02 | `CliErrorMessages` lcov 0% | Medium | CLI stderr E2E TC 부재 |
| I-03 | `TableFormatter` 22.2% | High | `formatLine` 미호출 — Boundary 게이트 블로커 |
| I-04 | `legacyCliCatalog` vs `bootstrap_default_three_units` 이중 bootstrap | Medium | REFACTOR 의도적 분리 — 문서화 유지 |
| I-05 | MSVC gcov 미지원 | Low | MinGW `build_cov` 별도 빌드로 우회 ✅ |
| I-06 | README ⬜ vs 실제 GREEN 13/14 | Low | README [x] 미갱신 (의도적 후속) |

---

## 9. 생성형 AI 활용 회고

### 9.1 도움이 된 순간

| 순간 | AI 기여 |
|------|---------|
| **Dual-Track RED→GREEN 9커밋 순서** | TC-A/B 교차 증분 커밋 표·Conventional Commit 메시지 일관성 |
| **REFACTOR 안전망** | Golden Master 4/4를 커밋마다 재실행하도록 프롬프트 고정 → 회귀 0건 |
| **커버리지 분석** | `lcov --list` + `.gcov` `#####` 라인 대조로 Invariant 미커버 분기를 Domain/Boundary로 분리 |
| **defect_list 정식화** | QA RPT-07 초안 → 8열 표 + traceability 자동 이식 |
| **BCE 테스트 명명** | `test_[타입]_[조건]_[기대]` + Given/When/Then 주석 패턴 유지 |

### 9.2 한계

| 한계 | 대응 |
|------|------|
| README·TODO.md **진행 상태 표가 M1 RED로 고정** | 실제는 BCE GREEN·REFACTOR 완료 — 문서 갱신은 수동 확인 필요 |
| AI가 **레거시 gcov만으로 95% 달성** 착각 가능 | test_plan §7.4: `src/domain/*` lcov extract가 정식 게이트 |
| **DEF-002·003 vs GM baseline** 충돌 | AI가 REFACTOR 중 계약 변경을 제안할 수 있음 → GM PASS 우선 규칙으로 차단 |
| MinGW **Branch 커버리지 no data** | Line 게이트만 현실적 판정 기준으로 사용 |

### 9.3 TC 작성 팁 (Cursor + Catch2)

1. **Invariant를 테스트 이름에 노출** — `test_meter_to_feet_positiveValue_returnsFeetProduct`처럼 비율·조건·기대를 3단 분리.
2. **RED는 `FAIL("RED")` 또는 링크 실패로 고정** — GREEN 전 `.cpp` 구현 금지를 프롬프트에 명시.
3. **커버리지 TC는 `.gcov` 역추적** — `TableFormatter::formatLine` 0%처럼 “테스트는 round만, formatLine은 미호출” 갭을 숫자로 확인.
4. **CLI E2E는 subprocess + stderr 스냅샷** — `CliErrorMessages` inline 헤더는 Catch2 단위 테스트만으로는 커버되지 않음.
5. **Golden Master를 REFACTOR 전에 고정** — 구조 변경 후에도 GM-TC-01~04 diff 0이 최종 안전망.
6. **Dual-Track TC-A-05 갭을 BCE와 혼동하지 말 것** — `InputParser` PASS ≠ `ConversionApplicationService` PASS.

---

## 10. 결론

UnitConverter_12는 **M0 문서 → BCE 35건 GREEN → Dual-Track 13/14 GREEN → CLI 7커밋 REFACTOR → Golden Master 4/4**까지 기능 회귀 없이 진행되었다. 아키텍처 목표(if-else 제거 · 상수 추출 · Thin Controller)는 **충족**하였으나, test_plan §7 **커버리지 게이트**(Domain 91.3% · Boundary 79.4%)와 README To-Do **[x] 동기화**, JSON RED 1건, DEF-002·003 Open은 **후속 M2~M3** 작업으로 남는다.

---

## 11. 참고 문서

| 문서 | 용도 |
|------|------|
| [18.REFACTOR_전체검증_커버리지측정.md](./18.REFACTOR_전체검증_커버리지측정.md) | REFACTOR 후 1차 검증 (본 보고와 동일 측정값) |
| [12.DualTrack_TDD_GREEN_증분구현.md](./12.DualTrack_TDD_GREEN_증분구현.md) | 9커밋 GREEN 상세 |
| [17.DualTrack_REFACTOR_CLI_BCE분리.md](./17.DualTrack_REFACTOR_CLI_BCE분리.md) | 7커밋 REFACTOR 상세 |
| [docs/test_plan.md](../docs/test_plan.md) | §7 gcov/lcov 게이트 |

---

## 변경 이력

| 버전 | 날짜 | 변경 내용 |
|------|------|-----------|
| 1.0 | 2026-05-21 | TDD 전주기 검증 종합 보고서 최초 작성 (커버리지 재측정 포함) |
