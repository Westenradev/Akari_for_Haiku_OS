add_executable(akari_core_tests test_core.cpp)
target_sources(akari_core_tests PRIVATE
	${CMAKE_SOURCE_DIR}/src/core/ColorMath.cpp
	${CMAKE_SOURCE_DIR}/src/core/Schedule.cpp
)
target_include_directories(akari_core_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_features(akari_core_tests PRIVATE cxx_std_17)

add_test(NAME akari_core_tests COMMAND akari_core_tests)
