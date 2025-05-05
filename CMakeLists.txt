#[[
    @file CMakeLists.txt
    @brief Główny plik konfiguracyjny CMake dla projektu AirQualityMonitor
    @author Piotr Trzeciak
    @date 2025
    @version 1.0
]]

# Minimalna wymagana wersja CMake
cmake_minimum_required(VERSION 3.16)

# Konfiguracja projektu
project(AirQualityMonitor
    VERSION 1.0
    DESCRIPTION "Aplikacja monitorująca jakość powietrza"
    LANGUAGES CXX
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#============================================
# Ustawienia podstawowe
#============================================
# Włączenie automatycznych narzędzi Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Ustawienia standardu C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ścieżka do instalacji Qt (dostosuj do swojej ścieżki)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")

#============================================
# Wyszukiwanie wymaganych komponentów Qt
#============================================

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    LinguistTools
    Network
    Charts
    Gui
    Positioning
    Location
    Quick
    Test
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Widgets
    LinguistTools
    Network
    Charts
    Gui
    Positioning
    Location
    Quick
    Test
)
#============================================
# Konfiguracja głównej aplikacji
#============================================

# Lista plików źródłowych
set(SOURCES
    main.cpp
    mainwindow.cpp
)

# Lista plików nagłówkowych
set(HEADERS
    mainwindow.h
)

# Lista plików interfejsu użytkownika
set(FORMS
    mainwindow.ui
)

# Dodanie głównego celu budowania
add_executable(AirQualityMonitor ${SOURCES} ${HEADERS} ${FORMS})

# Linkowanie bibliotek
target_link_libraries(AirQualityMonitor PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Positioning
    Qt${QT_VERSION_MAJOR}::Location
    Qt${QT_VERSION_MAJOR}::Quick
)

# Dodatkowe flagi kompilacji dla Windows
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# Ścieżka do biblioteki nlohmann/json
include_directories(C:/dev/nlohmann/include)

# Instalacja
install(TARGETS AirQualityMonitor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalizacja (dla Qt6)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AirQualityMonitor)
endif()

#============================================
# Konfiguracja Doxygen
#============================================

find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "AirQualityMonitor")
    set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_INLINE_SOURCES YES)
    set(DOXYGEN_STRIP_CODE_COMMENTS NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)

    doxygen_add_docs(
        doxygen-docs
        ${SOURCES}
        ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generowanie dokumentacji Doxygen"
    )
endif()
# =============================================
# Sekcja testów jednostkowych
# =============================================
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()

    # Utwórz bibliotekę z kodu źródłowego aplikacji do testów
    add_library(AirQualityMonitor_lib STATIC
        ${SOURCES}
        ${HEADERS}
        ${FORMS}
    )

    target_link_libraries(AirQualityMonitor_lib PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Charts
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Positioning
        Qt${QT_VERSION_MAJOR}::Location
        Qt${QT_VERSION_MAJOR}::Quick
    )

    # Dodaj plik z testami
    add_executable(AirQualityMonitorTests tests.cpp)

    # Linkowanie testów z biblioteką i wymaganymi komponentami
    target_link_libraries(AirQualityMonitorTests PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        AirQualityMonitor_lib
    )

    # Dodaj ścieżki do nagłówków
    target_include_directories(AirQualityMonitorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Dodaj test do CTest
    add_test(NAME AirQualityMonitorTests COMMAND AirQualityMonitorTests)

    # Finalizacja dla Qt6
    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(AirQualityMonitorTests)
    endif()
endif()

#============================================
# Informacje podsumowujące
#============================================

message(STATUS "============================================")
message(STATUS "Konfiguracja projektu AirQualityMonitor")
message(STATUS "Wersja Qt: ${QT_VERSION_MAJOR}")
message(STATUS "Ścieżka do Qt: ${CMAKE_PREFIX_PATH}")
message(STATUS "Doxygen: ${DOXYGEN_FOUND}")
message(STATUS "============================================")
