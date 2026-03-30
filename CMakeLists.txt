cmake_minimum_required(VERSION 3.19)
project(lab3Inheritance_forStudents)

# --- Modern colored log helpers ---
if(CMAKE_COLOR_MAKEFILE)
    string(ASCII 27 Esc)
    set(CLR_RESET  "${Esc}[0m")
    set(CLR_BOLD   "${Esc}[1m")
    set(CLR_GREEN  "${Esc}[32m")
    set(CLR_YELLOW "${Esc}[33m")
    set(CLR_RED    "${Esc}[31m")
    set(CLR_BLUE   "${Esc}[34m")
else()
    set(CLR_RESET  "")
    set(CLR_BOLD   "")
    set(CLR_GREEN  "")
    set(CLR_YELLOW "")
    set(CLR_RED    "")
    set(CLR_BLUE   "")
endif()

function(_colorize_paths INPUT_TEXT OUTPUT_VAR)
    set(_msg "${INPUT_TEXT}")
    if(CMAKE_COLOR_MAKEFILE)
        string(REGEX REPLACE "(/[^ \t\n\r\"')]+)" "${CLR_BLUE}\\1${CLR_RESET}" _msg "${_msg}")
        string(REGEX REPLACE "(\\./[^ \t\n\r\"')]+)" "${CLR_BLUE}\\1${CLR_RESET}" _msg "${_msg}")
    endif()
    set(${OUTPUT_VAR} "${_msg}" PARENT_SCOPE)
endfunction()

function(log_ok MSG)
    _colorize_paths("${MSG}" _colored_msg)
    message(STATUS "${CLR_GREEN}${CLR_BOLD}[ OK ]${CLR_RESET} ${_colored_msg}")
endfunction()

function(log_warn MSG)
    _colorize_paths("${MSG}" _colored_msg)
    message(WARNING "${CLR_YELLOW}${CLR_BOLD}[WARN]${CLR_RESET} ${_colored_msg}")
endfunction()

function(log_err MSG)
    _colorize_paths("${MSG}" _colored_msg)
    message(FATAL_ERROR "${CLR_RED}${CLR_BOLD}[ERR ]${CLR_RESET} ${_colored_msg}")
endfunction()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest /Wall /WX")
    endif()
else()
    set(CMAKE_CXX_FLAGS "--std=c++${CMAKE_CXX_STANDARD} -Wall -Wextra -pedantic -Wno-error=deprecated-declarations")
    set(COMPILER_DEBUG_FLAGS "-D_DEBUG")
    set(COMPILER_RELEASE_FLAGS "-O3 -DNDEBUG")
    set(LINKER_FLAGS "--std=c++${CMAKE_CXX_STANDARD}")
endif()

set(CMAKE_BUILD_TYPE Debug)

# Na Linuxie dla kompilatorow g++ i Clang moga byc dostepne sanitizery:
if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    include(CheckLibraryExists)

    check_library_exists(asan __asan_init "" HAVE_LIBASAN)
    check_library_exists(ubsan __ubsan_handle_add_overflow "" HAVE_LIBUBSAN)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        log_ok("AppleClang: sanitizers dostępne podczas kompilacji")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
    elseif (HAVE_LIBASAN AND HAVE_LIBUBSAN)
        log_ok("Sanitizers (AddressSanitizer + UndefinedBehaviorSanitizer) są dostępne")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
    else()
        log_ok("Brak libasan/libubsan: pomijam sanitizery na tym środowisku")
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # for cppcheck To generate compile_commands.json

set(ENABLE_JSON_COMPILATION_OUTPUT_GCC OFF CACHE BOOL "Enable JSON output from GCC compiler")

if (CMAKE_COMPILER_IS_GNUCC AND ENABLE_JSON_COMPILATION_OUTPUT_GCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-format=json")
endif()


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MAIN_PROJECT_NAME "${PROJECT_NAME}")
set(PROJECT_BINARY_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}")


file(GLOB SOURCES *.cpp *.cc *.c)
file(GLOB HEADERS *.h *.hpp)
set(SOURCES_WITHOUT_MAIN ${SOURCES})
list(REMOVE_ITEM SOURCES_WITHOUT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
log_ok("Katalog do zbudowanego projektu: ${PROJECT_BINARY_DIR}")
log_ok(" + Znaleziono nagłówki: ${HEADERS}")
log_ok(" + Znaleziono źródła: ${SOURCES}")
log_ok(" + Znaleziono źródła bez main.cpp: ${SOURCES_WITHOUT_MAIN}")


############ dodanie binarki z mainem i danymi studenta
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
   add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
else()
   add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
endif()


############ KOMENDA: make run
add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)


############ KOMENDA: make valgrind_${PROJECT_NAME}
find_program(VALGRIND NAMES valgrind)
if (VALGRIND)
    log_ok("valgrind found: ${VALGRIND}")
    add_custom_target(valgrind_${PROJECT_NAME} DEPENDS ${PROJECT_NAME}
        COMMAND ${VALGRIND} --leak-check=full ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})
endif()


############ KOMENDA: make docs
find_program(DOXYGEN NAMES doxygen)
if (DOXYGEN)
    log_ok("doxygen found: ${DOXYGEN}")
    add_custom_target(docs
            COMMAND ${DOXYGEN} ${CMAKE_CURRENT_SOURCE_DIR} && make -C ${CMAKE_CURRENT_SOURCE_DIR}/latex && cp latex/refman.pdf ${CMAKE_CURRENT_BINARY_DIR}/trescWygenerowana.pdf
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()


############ KOMENDA: make zip
get_filename_component(directoryBasename ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(zip
        COMMAND zip -r ${CMAKE_BINARY_DIR}/${directoryBasename}.zip ./
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


############ KOMENDA: make format
#idealnym standardem dla C++ jest https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines, niestety nie jest on jeszcze wspierany przez narzędzie clang-format, dlatego używam innego popularnego stylu.
add_custom_target(format
        COMMAND clang-format -i -style=Google ${SOURCES})

############ KOMENDA: make ctags
add_custom_target(ctags
        COMMAND ctags -x --c++-types=f --extras --format=1 ${HEADERS} ${SOURCES_WITHOUT_MAIN})

############ KOMENDA: make cppcheck_${PROJECT_NAME}
find_program(CPPCHECK NAMES cppcheck)
if (CPPCHECK)
    log_ok("cppcheck found: ${CPPCHECK}")
    add_custom_target(cppcheck_${PROJECT_NAME} DEPENDS ${PROJECT_NAME}
        COMMAND cppcheck --project=${CMAKE_BINARY_DIR}/compile_commands.json --force --enable=warning,performance,unusedFunction,style,information --template=gcc --inline-suppr --library=gnu.cfg --library=std.cfg --library=cppcheck-lib.cfg --library=motif.cfg --library=qt.cfg --library=windows.cfg --library=gnu.cfg --library=posix.cfg --library=avr.cfg --suppress=missingIncludeSystem -i ${HEADERS} ${SOURCES_WITHOUT_TESTS} -i ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

############ dodanie podprojektu z testami:
add_subdirectory(tests)

############ dodanie podprojektu z gra (wymagana biblioteka QT do kompilacji):
add_subdirectory(game)
