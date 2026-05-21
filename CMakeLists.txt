cmake_minimum_required(VERSION 3.16)
project(UnitConverter_12 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Build with gcov coverage flags" OFF)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)
endif()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_library(unit_converter_domain
  src/domain/UnitCatalog.cpp
  src/domain/ConversionEngine.cpp
  src/domain/LengthQuantity.cpp
  src/domain/LegacyCliConversion.cpp
)
target_include_directories(unit_converter_domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(unit_converter_boundary
  src/boundary/InputParser.cpp
  src/boundary/ValueValidator.cpp
  src/boundary/TableFormatter.cpp
  src/boundary/ConversionApplicationService.cpp
)
target_include_directories(unit_converter_boundary PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/helpers
)
target_link_libraries(unit_converter_boundary PUBLIC unit_converter_domain)

add_library(unit_converter_data
  src/data/ConfigLoader.cpp
)
target_include_directories(unit_converter_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(unit_converter_data PUBLIC unit_converter_domain)

add_library(unit_converter_logic
  src/logic/UnitConverterLogic.cpp
)
target_include_directories(unit_converter_logic PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/helpers
)
target_link_libraries(unit_converter_logic PUBLIC unit_converter_domain unit_converter_data)

function(add_unit_converter_test target)
  add_executable(${target} ${ARGN})
  target_compile_definitions(${target} PRIVATE
    UNIT_CONVERTER_TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/testdata")
  target_link_libraries(${target} PRIVATE
    unit_converter_domain
    unit_converter_boundary
    unit_converter_data
    Catch2::Catch2WithMain
  )
  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/helpers)
  catch_discover_tests(${target})
endfunction()

add_unit_converter_test(unit_converter_domain_tests
  tests/domain_normal_conversion_tests.cpp
  tests/domain_boundary_tests.cpp
  tests/domain_exception_tests.cpp
  tests/domain_register_tests.cpp
)

add_unit_converter_test(unit_converter_data_tests
  tests/data_config_tests.cpp
)

add_unit_converter_test(unit_converter_red_dual_track_tests
  tests/ui_boundary_red_tests.cpp
  tests/domain_logic_red_tests.cpp
)
target_link_libraries(unit_converter_red_dual_track_tests PRIVATE unit_converter_logic)

add_executable(unit_converter_cli UnitConverter.cpp)
target_link_libraries(unit_converter_cli PRIVATE
  unit_converter_domain
  unit_converter_boundary
  unit_converter_data
)

add_executable(UnitConverter_test tests/test_golden_master.cpp)
target_compile_definitions(UnitConverter_test PRIVATE
  UNIT_CONVERTER_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  UNIT_CONVERTER_BUILD_DIR="${CMAKE_BINARY_DIR}"
  UNIT_CONVERTER_TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/testdata")
target_link_libraries(UnitConverter_test PRIVATE
  unit_converter_domain
  unit_converter_boundary
  unit_converter_data
  Catch2::Catch2WithMain
)
target_include_directories(UnitConverter_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/helpers)
add_dependencies(UnitConverter_test unit_converter_cli)

add_test(NAME GoldenMaster COMMAND $<TARGET_FILE:UnitConverter_test>)
set_tests_properties(GoldenMaster PROPERTIES
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  LABELS "golden;regression;approval"
)
