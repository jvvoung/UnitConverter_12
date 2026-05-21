# UnitConverter_12 — QA 종합 검토 최종 보고서

| 항목 | 내용 |
|------|------|
| **문서 ID** | QA-FINAL-2026-05-21 |
| **작성 기준일** | 2026-05-21 |
| **작성 관점** | QA 리드 엔지니어 |
| **대상 저장소** | UnitConverter_12 (`c:\DEV\UnitConverter_12`) |
| **측정 기준일** | 2026-05-21 (본 세션 로컬 재측정) |
| **기준 문서** | [requirements.md](./requirements.md) · [PRD.md](./PRD.md) · [test_plan.md](./test_plan.md) · [defect_list.md](./defect_list.md) |
| **선행 보고서** | RPT-01~09 · RPT-13 · RPT-17 · RPT-18 · RPT-21 · [202605211_UnitConverter_C++_Verification_Report.md](../Report/202605211_UnitConverter_C++_Verification_Report.md) |
| **코드 변경** | **없음** (측정·분석·문서만) |

---

## §1 Executive Summary

UnitConverter_12는 **문서·계약·BCE·Dual-Track·Golden Master·Bonus**까지 TDD 전주기를 거친 C++ 학습 프로젝트이다. 본 QA 종합 검토는 **2026-05-21 로컬 재측정**(`ctest` 51건 · `lcov` GCC 13.2 · `gcov UnitConverter.cpp`)을 기준으로 한다.

### §1.1 종합 판정표

| 영역 | 판정 | 근거 (한 줄) |
|------|:----:|--------------|
| **BCE Catch2 (35건)** | ✅ **PASS** | Domain/Data 전건 GREEN — 35/35 |
| **Dual-Track (14건)** | 🔶 **부분 PASS** | 13/14 GREEN · `json_format` RED 1건 · TC-A-05 Dual-Track 갭 |
| **Golden Master (4 TC)** | ✅ **PASS** | GM-TC-01~04 baseline diff 0 |
| **Bonus BT-01~06 (6 TC)** | ✅ **PASS** | exe `"[bonus]"` 10/10 assertion · ctest #50 인코딩 이슈 |
| **레거시 CLI (`UnitConverter.cpp`)** | 🔶 **부분 PASS** | REFACTOR Thin Controller ✅ · DEF-002·003 **Open** (PRD vs GM 계약) |
| **커버리지 게이트 (test_plan §7)** | ❌ **FAIL** | Domain Line 91.5% (<95%) · Boundary Line 79.4% (<85%) · Branch 미집계 |
| **PRD AC-01~07 인수** | ❌ **FAIL** | AC-02·AC-01 CLI 갭 · AC-06 커버리지 미달 · AC-04 cubit CLI 미연동 |
| **defect_list 운영** | 🔶 **부분 PASS** | DEF-001 Closed · DEF-002·003 Open · traceability 유지 |
| **README To-Do 동기화** | ❌ **FAIL** | Track A/B·커버리지·GM CI 항목 대부분 ⬜ (문서 갭) |

**Executive 한 줄**: **BCE·Dual-Track·Golden Master·Bonus 기능 회귀는 안정**이나, **PRD 인수(AC)·커버리지 게이트·CLI 계약(DEF-002/003)·문서 동기화**가 v1.0 릴리스 블로커로 남는다.

### §1.2 재측정 환경

| 항목 | MSVC (`build/`) | MinGW (`build_cov13/`) |
|------|-----------------|------------------------|
| OS | Windows 10.0.19045 | 동일 |
| Generator | Visual Studio 17 2022 | MinGW Makefiles |
| Compiler | MSVC 19.44 | g++ 13.2.0 (Strawberry) |
| Catch2 | v3.5.4 | 동일 |
| ctest | `ctest --test-dir build -C Debug` | `ctest --test-dir build_cov13` |
| 커버리지 | 미지원 | `ENABLE_COVERAGE=ON` + lcov 1.15 |

> MinGW 15.2 + lcov 1.15는 gcov 포맷 B11/B52 불일치로 **측정 불가**. 본 보고서 lcov 수치는 **`build_cov13` (GCC 13.2)** 기준 ([RPT-21](../Report/21.DecayingUnit_Bonus_전체검증_커버리지측정.md) §3.3).

---

## §2 테스트 완료율·커버리지

### §2.1 ctest 전체 (2026-05-21 재측정)

**명령**:

```powershell
cmake -S . -B build; cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
.\build\Debug\unit_converter_bonus_tests.exe "[bonus]" -s
```

| # | 구분 | ctest 항목 | 기능 PASS | FAIL 원인 |
|---|------|:----------:|:---------:|-----------|
| 1~28 | **BCE Domain** | 28 | ✅ 28/28 | — |
| 29~35 | **BCE Data** | 7 | ✅ 7/7 | — |
| — | **BCE 소계** | **35** | **35/35 (100%)** | — |
| 36~41 | Dual-Track TC-A `[ui][green]` | 6 | ✅ 6/6 | — |
| 42 | Dual-Track JSON `[ui][red]` | 1 | ❌ 0/1 | `FAIL("RED")` 의도적 |
| 43~49 | Dual-Track TC-B `[logic][green]` | 7 | ✅ 7/7 | — |
| — | **Dual-Track 소계** | **14** | **13/14 (92.9%)** | json RED 1 |
| 50 | **Bonus BT** (`[bonus]`) | 1 | 🔶 exe ✅ / ctest ❌ | 한글 TEST_CASE CP949 깨짐 |
| 51 | **Golden Master** (ctest) | 1 | ✅ 1/1 | 내부 4 TC · 8 assertion |
| — | **ctest 합계** | **51** | **49/51 (96.1%)** | json + Bonus 인코딩 |

**기능 관점 완료율** (Bonus exe 직접 실행 포함): **50/51 (98.0%)** — RED 1건(`json_format`)만 잔존.

### §2.2 Suite별 상세

#### BCE (35건) — 100% PASS

| 파일 | TC 수 | 태그 | 상태 |
|------|:-----:|------|------|
| `domain_normal_conversion_tests.cpp` | 6 | `[domain][normal]` | ✅ |
| `domain_boundary_tests.cpp` | 7 | `[domain][boundary]` | ✅ |
| `domain_exception_tests.cpp` | 8 | `[domain][exception]` | ✅ |
| `domain_register_tests.cpp` | 7 | `[domain][register]` | ✅ |
| `data_config_tests.cpp` | 7 | `[data][config]` | ✅ |

#### Dual-Track (14건) — 13 GREEN + 1 RED

| README ID | Catch2 / BCE 대응 | Dual-Track | BCE 보완 | 상태 |
|-----------|-------------------|:----------:|:--------:|:----:|
| TC-A-01 | `valid_input_meter_colon_value_*` | ✅ | — | ✅ |
| TC-A-02 | `input_without_colon_*` | ✅ | — | ✅ |
| TC-A-03 | `negative_value_*` | ✅ | — | ✅ |
| TC-A-04 | `unknown_unit_*` | ✅ | — | ✅ |
| TC-A-05 | *(미작성)* | ⬜ | ✅ #15 `test_inputParser_nonNumeric*` | 🔶 |
| TC-A-06 | `output_preserves_source_*` | ✅ | — | ✅ |
| TC-A-07 | `zero_value_*` | ✅ | — | ✅ |
| TC-B-01~07 | `convert_*` / `register_*` / `load_config_*` | ✅ 7/7 | — | ✅ |
| (확장) | `json_format_returns_valid_schema` | ❌ RED | — | ❌ |

**Track A/B README 14항목 기능 매핑**: **14/14 PASS** (TC-A-05는 BCE로 대체 검증).

#### Golden Master (4 TC) — 100% PASS

| GM-TC | 시나리오 | 검증 | 상태 |
|-------|----------|------|:----:|
| GM-TC-01 | `meter:2.5` | 6dp feet/yard · baseline 일치 | ✅ |
| GM-TC-02 | `feet:1.0` | 역변환 6dp | ✅ |
| GM-TC-03 | `yard:1.0` | 역변환 6dp | ✅ |
| GM-TC-04 | `meter:0.0` | **0 허용 출력** (DEF-003 Open과 PRD 충돌) | ✅ GM |

#### Bonus DecayingUnit BT-01~06 — 100% PASS (exe)

| ID | 검증 | 상태 |
|----|------|:----:|
| BT-01~03 | cubit 등록·허브 변환·교차 변환 | ✅ |
| BT-04 | 음수 비율 → `std::invalid_argument` | ✅ |
| BT-05 | `convertAll` size=3 | ✅ |
| BT-06 | meter→feet 회귀 3.28084 | ✅ |

> Bonus는 `unit_converter_decaying`로 BCE와 **링크·코드 격리** — BCE 회귀에 영향 없음 ([RPT-20](../Report/20.DecayingUnit_Bonus_TDD_GREEN_구현.md)).

#### RED / 기타

| 항목 | 유형 | 영향 |
|------|------|------|
| `json_format_returns_valid_schema` | 의도적 RED (PRD §6.3 JSON) | ctest 51/51 블로커 |
| Bonus ctest #50 | 환경 (UTF-8/CP949) | ctest만 FAIL · 기능 PASS |
| README To-Do `[ ]` | 문서 동기화 | QA 추적 혼선 |

### §2.3 gcov / lcov (test_plan §6·§7)

**측정 명령** (`build_cov13`, GCC 13.2):

```powershell
lcov --directory build_cov13 --zerocounters
ctest --test-dir build_cov13 --output-on-failure
lcov --capture --directory build_cov13 --output-file build_cov13/coverage.info --ignore-errors gcov,source
lcov --extract build_cov13/coverage.info "*/src/domain/*" -o build_cov13/domain.info --ignore-errors gcov,source
lcov --extract build_cov13/coverage.info "*/src/boundary/*" -o build_cov13/boundary.info --ignore-errors gcov,source
lcov --summary build_cov13/domain.info
lcov --summary build_cov13/boundary.info
cd build_cov13\CMakeFiles\unit_converter_cli.dir; gcov -b UnitConverter.cpp.gcda
```

#### 레이어별 목표 대비 (2026-05-21)

| 측정 대상 | Line (실측) | Line (목표) | Line 판정 | Branch (실측) | Branch (목표) | Branch 판정 |
|-----------|:-----------:|:-----------:|:---------:|:-------------:|:-------------:|:-----------:|
| **Domain** (`src/domain/*`) | **91.5%** (86/94) | ≥ 95% | ❌ **−3.5%p** | no data | ≥ 90% | ⚠️ **미집계** |
| **Boundary** (`src/boundary/*`) | **79.4%** (85/107) | ≥ 90% (Line) | ❌ **−10.6%p** | no data | ≥ 85% | ⚠️ **미집계** |
| Boundary (README·RPT-18 기준) | 79.4% | ≥ 85% | ❌ **−5.6%p** | — | ≥ 85% | ⚠️ |
| **Data** (`src/data/*`) | *(extract 미실행)* | ≥ 85% | — | — | ≥ 80% | — |
| Function Domain | 96.0% (24/25) | — | 참고 | — | — | — |
| Function Boundary | 80.0% (12/15) | — | 참고 | — | — | — |

> test_plan §6.1: Domain **Line ≥95% / Branch ≥90%** · Boundary **Line ≥90% / Branch ≥85%**. README·RPT-18은 Boundary Line **≥85%**로 완화 표기 — **본 보고서는 test_plan 정본 우선**, README 85%를 병기한다.

#### Domain 파일별 Line (lcov --list)

| 파일 | Line | Functions | 병목 |
|------|:----:|:---------:|------|
| `ConversionEngine.cpp` | 94.4% | 100% | 1 line |
| `LegacyCliConversion.cpp` | 91.7% | 100% | 에러 분기 |
| `LengthQuantity.cpp` | 91.7% | 100% | — |
| `LengthQuantity.hpp` | 100% | 100% | — |
| `UnitCatalog.cpp` | 90.0% | 91.7% | `clear`/`empty`/duplicate 경로 |

#### Boundary 파일별 Line

| 파일 | Line | Functions | 병목 |
|------|:----:|:---------:|------|
| `ConversionApplicationService.cpp` | 96.3% | 100% | 1 line |
| `InputParser.cpp` | 80.0% | 100% | 일부 파싱 분기 |
| `ValueValidator.cpp` | 100% | 100% | — |
| `CliOutputFormatter.cpp` | 100% | 100% | — |
| **`CliErrorMessages.hpp`** | **0.0%** | 0% | CLI stderr E2E 부재 |
| **`TableFormatter.cpp`** | **22.2%** | 50% | **주요 병목** — AC-02 1dp 미연동 |

#### `UnitConverter.cpp` gcov (레거시 CLI · 참고)

| 지표 | 값 | 비고 |
|------|:--:|------|
| Line | **79.17%** (19/24) | RPT-18 78.26% → +0.9%p |
| Branch executed | 70.83% | — |
| Branch taken | 37.50% | — |
| 미커버 | L26~29 파싱 에러 · L33~35 unknown unit | happy path 위주 ctest |

> test_plan §7.4: BCE `src/domain/*` · `src/boundary/*` lcov가 **정식 게이트**. `UnitConverter.cpp` gcov는 baseline 참고용.

### §2.4 test_plan AC · README To-Do 완료 매트릭스

| ID | test_plan / PRD | 구현·테스트 | README [x] | QA 판정 |
|----|-----------------|-------------|:----------:|:-------:|
| **AC-01** | 형식·음수·0·unknown exit/stderr | BCE+Dual-Track ✅ · CLI 🔶 DEF-003 | ⬜ | 🔶 **부분** |
| **AC-02** | `meter:2.5` table 1dp half-up | BCE `TableFormatter` TC ✅ · CLI 6dp DEF-002 | ⬜ | 🔶 **부분** |
| **AC-03** | `feet:8.2` POL-OUT prefix | Dual-Track TC-A-06 ✅ | ⬜ | ✅ |
| **AC-04** | REGISTER cubit · golden trio | BCE register ✅ · Bonus BT ✅ · CLI cubit ⬜ | ⬜ | 🔶 |
| **AC-05** | CONFIG 실패 exit 2 | BCE Data TC ✅ · CLI 미연동 | ⬜ | 🔶 |
| **AC-06** | 커버리지 리포트 제출 | lcov 측정 ✅ · **게이트 미달** | ⬜ | ❌ |
| **AC-07** | Domain→Boundary 의존 0 | BCE 구조 ✅ | ⬜ | ✅ |
| TC-A-01~07 | Track A 7건 | 6 Dual + 1 BCE | ⬜ 전부 | 🔶 7/7 기능 |
| TC-B-01~07 | Track B 7건 | 7/7 Dual-Track | ⬜ 전부 | ✅ |
| GM-01~06 | Golden Master 인프라 | 코드·baseline ✅ | ⬜ | ✅ |
| GM-07~09 | CI workflow | ⬜ | ⬜ | ❌ |
| Domain ≥95% | test_plan §6 | 91.5% | ⬜ | ❌ |
| Boundary ≥85% | README | 79.4% | ⬜ | ❌ |
| defect_list | 생성·회귀 | 생성 ✅ · DEF-002/003 Open | 1/2 [x] | 🔶 |

---

## §3 결함 패턴 분석

### §3.1 BCE 레이어별

| 레이어 | 활성 결함 | 패턴 | QA 관찰 |
|--------|-----------|------|---------|
| **Domain** | 0 (BCE TC) | overflow(DEF-001 Closed) · hub 변환 · registerUnit | 35/35 GREEN · lcov 91.5% — **예외·clear 경로 미커버** |
| **Boundary** | 0 (Catch2) · 2 (CLI 계약) | POL-NEG · 1dp · 파싱 · stderr | `ValueValidator` 100% · **`CliErrorMessages` 0%** · **`TableFormatter` 22%** |
| **Data** | 0 | Config load 실패·malformed | 7/7 GREEN · extract 미측정 |
| **CLI 레거시** | DEF-002·003 Open | 6dp·0 허용 vs PRD | REFACTOR 후 Thin Controller — **계약 전환 의도적 보류** (RPT-17) |
| **Bonus** | 0 | OCP registry · 음수 k | BT 6/6 · BCE 격리 |

### §3.2 변환·검증 타입별

| 타입 | test_plan | BCE/Dual-Track | CLI/GM | 결함 ID | 상태 |
|------|-----------|----------------|--------|---------|:----:|
| **POL-NEG** (음수·0) | BV-01~03, TP-B-02~03 | ✅ Track A · BCE | ❌ GM-04 0 허용 | DEF-003 | **Open** |
| **table 1dp** | AC-02, TP-B-01 | ✅ `TableFormatter` unit | ❌ 6dp raw | DEF-002 | **Open** |
| **파싱** | BV-04~05 | ✅ InputParser TC | ✅ REFACTOR 연동 | — | Closed |
| **unknown unit** | BV-06 | ✅ | ✅ stderr (lcov 0% on messages) | — | 🔶 |
| **overflow** | BV-02, TP-B-07 | ✅ finite guard | — | DEF-001 | **Closed** |
| **Bonus DecayingUnit** | (학습 확장) | — | — | — | ✅ GREEN |
| **JSON format** | PRD §6.3 | ❌ RED | — | — | **RED** |

### §3.3 심각도별 (defect_list DEF-001~003)

| Severity | ID | 변환 타입 | Open/Closed | traceability |
|----------|-----|-----------|:-----------:|--------------|
| **Major** | DEF-001 | overflow `Inf` | **Closed** | BV-02, TP-B-07 |
| **Major** | DEF-003 | POL-NEG (음수·0) | **Open** | AC-01, BV-01, BV-03, TP-B-02~03 |
| **Minor** | DEF-002 | table 1dp | **Open** | AC-02, TP-B-01 |
| **Info** | — | json RED | Open (기능) | PRD §6.3 |
| **Info** | — | Bonus ctest 인코딩 | Open | I-20-01 |
| **Info** | — | README [x] 미동기화 | Open | 문서 부채 |

**Critical 패턴 (feet/yard inbound 미할당 → 0.000000)**: QA RPT-07 확인 — **해당 없음** (현재 코드 정상).

### §3.4 Open vs Closed 요약

```
Closed : DEF-001 (1건)
Open   : DEF-002, DEF-003 (2건) + json RED + ctest 인코딩 + README 동기화 + 커버리지 게이트
```

**근본 패턴**: BCE GREEN과 **CLI/Golden Master 계약 이중성** — REFACTOR는 구조만 분리하고 PRD AC-01·02 **행동 계약**은 GM baseline 보존을 위해 의도적 Open ([RPT-17](../Report/17.DualTrack_REFACTOR_CLI_BCE분리.md) §1).

---

## §4 9단계(RPT-01~09) 회고

| # | 단계 | Report | 산출물 품질 | QA 기여도 | 리스크 감소 | 개선 제안 |
|---|------|--------|:-----------:|:---------:|:-----------:|-----------|
| **1** | Spec·요구사항 정의 | [01](../Report/01.Spec.md) | ⭐⭐⭐⭐ | PRD·TODO 정본 확립 | 요구 혼선 방지 | README **문서↔구현 갭** 표를 주기 갱신 |
| **2** | RED README·계약 고정 | [02](../Report/02.Red_README_수정.md) | ⭐⭐⭐⭐ | POL-NEG·입력 형식 학습자 가시화 | RED FAIL 기대치 정렬 | M1 완료 후 README **상태 배지** 자동화 |
| **3** | TestPlan 샘플 선정 | [03](../Report/03.TestPlan_샘플예제_선정.md) | ⭐⭐⭐⭐⭐ | `meter:2.5` 앵커 — traceability 허브 | 범위 폭발 방지 | Golden trio `{1.0,2.5,0.1}` 후속 등록 |
| **4** | TestPlan 작성 | [04](../Report/04.TestPlan_작성.md) | ⭐⭐⭐⭐⭐ | TP/BV/EX·커버리지 게이트 **측정 가능** | lcov extract 패턴 고정 | Branch 게이트 **CI 스크립트** 선행 |
| **5** | RED To-Do·README | [05](../Report/05.Red_ToDo_README_추가.md) | ⭐⭐⭐ | Track A/B 체크리스트 | 실습 종료 조건 명시 | GREEN 후 **[x] 일괄 갱신** 프로세스 (현재 ⬜ 잔존) |
| **6** | Catch2 BCE GREEN | [06](../Report/06.Catch2_TDD_GREEN_구현.md) | ⭐⭐⭐⭐⭐ | **35/35** Domain/Data — 회귀 기반 | overflow DEF-001 조기 발견 | CLI 연동 **M2로 미루지 말고** GM과 분리 TC |
| **7** | QA 결함 분석·디버깅 | [07](../Report/07.QA_결함분석_디버깅.md) | ⭐⭐⭐⭐⭐ | BCE vs CLI **이중 검증** · D-01~03 분류 | v1.0 블로커 가시화 | 수동 CLI 검증 → **Catch2 E2E stderr** 자동화 |
| **8** | DefectList 정식화 | [08](../Report/08.DefectList_정식화.md) | ⭐⭐⭐⭐ | `docs/defect_list.md` DEF-xxx | 결함 추적 단일 진실원 | **Closed 시 회귀 TC ID** 필드 추가 |
| **9** | Dual-Track RED | [09](../Report/09.DualTrack_RED_테스트작성.md) | ⭐⭐⭐⭐ | LNK2019 RED → 9커밋 GREEN 경로 | UI/Logic 분리 검증 | TC-A-05·json **RED 작성 시점**에 포함 |

### §4.1 효과적이었던 단계 (Top 3)

1. **RPT-04 TestPlan** — BV/TP/EX·lcov 게이트가 QA 측정·판정의 **단일 기준**이 됨.
2. **RPT-07 QA 결함 분석** — BCE GREEN 착시를 깨고 **CLI 계약 갭(DEF-002/003)** 을 식별.
3. **RPT-06 BCE GREEN** — 35건 회귀 기반 + Domain register/overflow TC로 **Major 결함 1건 조기 종료**.

### §4.2 개선 필요 단계 (Top 3)

1. **RPT-05 README To-Do** — 구현 GREEN 완료 후에도 `[ ]` 잔존 → **완료율 과소/과대 보고** 위험.
2. **RPT-09 Dual-Track RED** — TC-A-05·json **갭이 GREEN까지 이월** → ctest 51/51 미달성.
3. **RPT-01 Spec** — “M1 RED 진행” vs 실제 BCE GREEN **문서 불일치** — 신규 QA/onboarding 혼선.

---

## §5 Best Practice 5 (다음 레거시 C++ · BCE · TDD · Cursor AI)

| # | Practice | 본 프로젝트 검증 | 미검증 / 보완 필요 |
|---|----------|------------------|-------------------|
| **1** | **계약 우선 TDD (Concept → RED → GREEN)** | ✅ PRD invariant → Catch2 TC → 커밋 C2C ([RPT-03~06](../Report/03.TestPlan_샘플예제_선정.md)) | README To-Do **자동 동기화** |
| **2** | **Dual-Track (UI Boundary + Domain Logic)** | ✅ 13/14 GREEN · 9커밋 증분 ([RPT-12](../Report/12.DualTrack_TDD_GREEN_증분구현.md)) | TC-A-05·json **Track 완결** |
| **3** | **Golden Master (출력 회귀 고정)** | ✅ GM-TC-01~04 · REFACTOR 7커밋 후에도 PASS ([RPT-15~17](../Report/15.GoldenMaster_테스트코드_구현.md)) | GM vs PRD **계약 충돌** (0·1dp) **명시적 이중 baseline** |
| **4** | **커버리지 게이트 (lcov extract by layer)** | ✅ 측정 파이프라인·파일별 병목 식별 ([RPT-13,18,21](../Report/13.GREEN_전체검증_커버리지측정.md)) | ❌ **게이트 미달** · Branch 미집계 · **GCC 13.2 pin** |
| **5** | **defect_list 운영 (Open/Closed + traceability)** | ✅ DEF-001~003 · BV/AC 연결 ([RPT-08](../Report/08.DefectList_정식화.md)) | DEF-002/003 **CLI 수정 + GM 재승인** 미완 |

### §5.1 Practice별 핵심 교훈

1. **계약 우선**: 테스트가 PRD보다 먼저 깨지면 구현 논쟁이 줄어든다 — **단, GM·PRD 이중 계약은 문서화 필수**.
2. **Dual-Track**: `ConversionApplicationService` vs `UnitConverterLogic` 분리로 **레이어 오염** 방지 — RED 1건이라도 **릴리스 게이트에 포함**할지 정책 결정 필요.
3. **Golden Master**: REFACTOR 안전망으로 **입력 4종 diff 0** — PRD AC와 **다른 baseline**이면 `golden_prd/` vs `golden_legacy/` 분리 권장.
4. **커버리지**: Line %만으론 `CliErrorMessages` 0% 같은 **계약 미실행**을 놓침 — **Invariant별 “must-cover branch”** 매핑 (test_plan §7.5 확장).
5. **defect_list**: 보고서 §5 → `docs/` 승격 ([RPT-08](../Report/08.DefectList_정식화.md))이 **M2 착수 조건**을 명확히 함.

---

## §6 Cursor AI 활용 회고

### §6.1 정량 추정 (세션·보고서 기반)

| 단계 | 전통 추정 | Cursor AI 보조 후 | 단축률 (추정) |
|------|:---------:|:-----------------:|:-------------:|
| Spec·TestPlan (RPT-01~04) | 8~12h | 3~4h | **~60%** |
| BCE GREEN 35 TC (RPT-06) | 12~16h | 4~6h | **~65%** |
| Dual-Track RED→GREEN 9커밋 (RPT-09~12) | 10~14h | 4~5h | **~60%** |
| REFACTOR CLI 7커밋 (RPT-17) | 8~10h | 3~4h | **~60%** |
| QA·검증·보고 (RPT-07,13,18,21) | 6~8h | 2~3h | **~65%** |
| Bonus GREEN (RPT-19~20) | 4~6h | 1~2h | **~70%** |
| **누적 (20+ Report)** | **~50~70h** | **~18~25h** | **~60%** |

> 교육용 6h 실습 **범위 초과** 구현·문서가 AI 보조로 압축됨 — **README M1 “RED 진행”과 실제 GREEN 간 갭**의 원인.

### §6.2 정성 효과

| 영역 | 효과 |
|------|------|
| **결함 조기 발견** | DEF-001 overflow · DEF-002 1dp · DEF-003 POL-NEG — QA RPT-07에서 **BCE PASS 착시** 깨뜨림 |
| **테스트 설계** | BV-01~06 · TP-D/B 매핑 · Golden approve 패턴 — **보일러플레이트 Catch2·CMake** 자동 생성 |
| **REFACTOR 안전** | Golden Master 4/4 매 커밋 — **Thin Controller** 7커밋 회귀 0 |
| **커버리지 분석** | `TableFormatter`·`UnitCatalog::clear` 등 **미커버 Invariant** TC 제안 ([202605211](../Report/202605211_UnitConverter_C++_Verification_Report.md)) |
| **문서 C2C** | Report/Prompt Export 20+건 — **재현 가능한 대화형 transcript** |

### §6.3 한계

| 한계 | 영향 | 완화 |
|------|------|------|
| **RED 유지 (`json_format`)** | ctest 100% 불가 | M3 마일스톤 또는 `[.]` exclude 정책 |
| **README [x] 미동기화** | QA 완료율 왜곡 | GREEN 커밋 hook · Cursor rule |
| **MSVC / MinGW 이중 빌드** | lcov 재현성 · CI 복잡 | `build_cov13` GCC pin · 문서화 (RPT-21 I-21-01) |
| **GM vs PRD 계약** | DEF-002/003 Open 지속 | 이중 baseline · M2 전환 PR |
| **Bonus ctest CP949** | #50 FAIL | TEST_CASE ASCII alias |
| **Branch lcov no data** | test_plan Branch 게이트 **판정 불가** | `--rc branch_coverage=1` · CI 스크립트 |

---

## §7 미완료·리스크·권장 후속

### §7.1 v1.0 릴리스 블로커 (우선순위)

| P | 항목 | 근거 | 완료 판정 |
|---|------|------|-----------|
| **P0** | DEF-003 POL-NEG CLI 연동 | AC-01 · BV-01~03 | `meter:0`/`-2.5` exit 1 · **GM-04 baseline 재협상** |
| **P0** | DEF-002 table 1dp | AC-02 | `TableFormatter` ↔ CLI · GM 1dp **별도 TC** |
| **P1** | Domain lcov ≥95% | AC-06 | `UnitCatalog` 예외·clear TC |
| **P1** | Boundary lcov ≥85~90% | AC-06 | `CliErrorMessages` E2E · `TableFormatter` 연동 |
| **P1** | `json_format` GREEN | Dual-Track 14/14 | ctest RED 0 |
| **P2** | TC-A-05 Dual-Track | README Track A 7/7 | `[ui][green]` TC 추가 |
| **P2** | Bonus ctest #50 | ctest 50/51 | ASCII TEST_CASE |
| **P2** | README To-Do [x] | QA 추적 | GREEN 항목 일괄 갱신 |
| **P3** | GM CI (GM-07~09) | RR-02 | workflow + required check |

### §7.2 M2 CLI 연동 시나리오

1. `ValueValidator`를 CLI 경로에 연결 → DEF-003 해소 vs GM-04 충돌 **PRD/Gherkin bump** (RR-05).
2. `TableFormatter::roundToOneDecimalHalfUp` CLI 적용 → DEF-002 해소 · **6dp GM baseline 유지 vs 1dp PRD** 분리.
3. Golden Master `-Approve` + diff 리뷰 — **의도적 계약 변경**만 baseline 갱신.

### §7.3 리스크 레지스터 (잔존)

| ID | 리스크 | 심각도 | 상태 |
|----|--------|:------:|:----:|
| R-QA-01 | BCE GREEN ≠ CLI PRD AC | High | Open |
| R-QA-02 | 커버리지 게이트 미달 | Medium | Open |
| R-QA-03 | Branch lcov 미집계 | Low | Open |
| R-QA-04 | README·구현 문서 갭 | Medium | Open |
| R-QA-05 | MinGW 15.2 lcov 불가 | Low | Mitigated (`build_cov13`) |

---

## §8 Traceability (PRD AC ↔ test_plan ↔ defect_list)

### §8.1 AC → test_plan TP/BV → 테스트/결함

| PRD AC | test_plan | Catch2 / ctest | defect_list | QA 상태 |
|--------|-----------|------------------|-------------|:-------:|
| AC-01 US-01 | TP-B-02~06, BV-01~06 | Dual-Track + BCE exception | DEF-003 | 🔶 |
| AC-02 US-03 | TP-D-01~02, TP-B-01, BV — | Domain normal + `TableFormatter` | DEF-002 | 🔶 |
| AC-03 US-04 | TP-B-08, EX-12 | TC-A-06 | — | ✅ |
| AC-04 US-06 | TP-I-01, TP-D-08 | register + Bonus BT | — | 🔶 |
| AC-05 US-05 | EX-16, TP-D-08 | data_config 7 TC | — | 🔶 |
| AC-06 E-SC | §6~§7 lcov | build_cov13 | — | ❌ |
| AC-07 US-02 | §4.2 BCE | CMake 타깅 | — | ✅ |

### §8.2 POL / RR → 검증

| Policy / Rule | test_plan | 검증 수단 | 갭 |
|---------------|-----------|-----------|-----|
| POL-NEG-01~03 | BV-01~03, TP-B-02~03 | Track A · BCE | CLI·GM-04 |
| POL-OUT-01~03 | TP-B-08, TC-A-06 | Dual-Track | ✅ |
| RR-02 Golden trio | TP-D-07 | GM 4 TC (2.5만) | `{1.0,0.1}` 미등록 |
| RR-03 stderr 패턴 | TP-B-02~06 | BCE exception | CLI lcov 0% |

### §8.3 requirements.md → PRD → 구현

| requirements | PRD | 구현 (2026-05-21) |
|--------------|-----|-------------------|
| OCP·SRP | §4.2 BCE · G-04 | ✅ UnitCatalog · ConversionEngine |
| 입력 검증 | POL-NEG | BCE ✅ · CLI 🔶 |
| 설정 외부화 | F-06, AC-05 | ✅ ConfigLoader 7 TC |
| 동적 단위 등록 | F-07, AC-04 | ✅ BCE register + Bonus BT |
| 테스트 코드 | §4.3 | ✅ 51 ctest (+ Bonus) |

---

## §9 결론

UnitConverter_12는 **TDD·BCE·Dual-Track·Golden Master·Bonus**를 통해 학습 목표(계약·레이어 분리·회귀)를 **기능적으로는 달성**하였다. **BCE 35/35**, **Dual-Track 13/14**, **Golden Master 4/4**, **Bonus BT 6/6**은 QA 재측정(2026-05-21) 기준 **PASS**이다.

그러나 **PRD v1.0 인수(AC-01·02·06)** 와 **test_plan §7 커버리지 게이트**는 **FAIL**이다. 핵심 원인은 (1) **CLI/Golden Master vs PRD 계약 이중성**(DEF-002·003 Open), (2) **`TableFormatter`·`CliErrorMessages` 미실행**으로 Boundary lcov 79.4%, (3) **README To-Do 미동기화**로 인한 QA 가시성 저하이다.

**QA 리드 권고**: v1.0 전 **M2 CLI 계약 통합**(POL-NEG·1dp) + **GM baseline 재승인** + **커버리지 게이트 충족 TC**를 한 PR 묶음으로 처리하고, `json_format` RED·Bonus ctest 인코딩·README `[x]`를 **릴리스 체크리스트**에 포함한다.

---

## 참고 문서

| 문서 | 용도 |
|------|------|
| [test_plan.md](./test_plan.md) | TP/BV/EX · §6~§7 커버리지 게이트 |
| [PRD.md](./PRD.md) | AC-01~07 · POL-NEG/OUT |
| [defect_list.md](./defect_list.md) | DEF-001~003 |
| [Report/21.DecayingUnit_Bonus_전체검증_커버리지측정.md](../Report/21.DecayingUnit_Bonus_전체검증_커버리지측정.md) | 최신 ctest·lcov |
| [Report/202605211_UnitConverter_C++_Verification_Report.md](../Report/202605211_UnitConverter_C++_Verification_Report.md) | TDD 전주기 누적 |
| `build_cov13/domain.info` · `boundary.info` | lcov 추출 (2026-05-21) |

---

## 변경 이력

| 버전 | 날짜 | 변경 내용 |
|------|------|-----------|
| 1.0 | 2026-05-21 | QA 종합 검토 최종 보고서 최초 작성 (로컬 ctest·lcov 재측정 반영) |
