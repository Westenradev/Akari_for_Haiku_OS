cmake_minimum_required(VERSION 3.16)

project(Akari CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AKARI_BUILD_TESTS "Build host-side unit tests for src/core" ON)

if(HAIKU)
	add_executable(Akari
		src/main.cpp
		src/App.cpp
		src/AkariWindow.cpp
		src/ReplicantView.cpp
		src/TintController.cpp
		src/OverlayWindow.cpp
		src/Settings.cpp
		src/core/ColorMath.cpp
		src/core/Schedule.cpp
	)
	target_include_directories(Akari PRIVATE src)
	target_compile_options(Akari PRIVATE -Wall -Wextra)
	target_link_libraries(Akari be translation tracker)

	find_program(RC rc)
	find_program(XRES xres)
	if(RC AND XRES)
		set(AKARI_RSRC ${CMAKE_BINARY_DIR}/Akari.rsrc)
		add_custom_command(
			OUTPUT ${AKARI_RSRC}
			COMMAND ${RC} -o ${AKARI_RSRC} ${CMAKE_SOURCE_DIR}/resources/app.rdef
			DEPENDS ${CMAKE_SOURCE_DIR}/resources/app.rdef
			COMMENT "Compiling resources/app.rdef")
		add_custom_command(
			TARGET Akari POST_BUILD
			COMMAND ${XRES} -o $<TARGET_FILE:Akari> ${AKARI_RSRC}
			COMMENT "Attaching resources to Akari")
	endif()
endif()

if(AKARI_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
