cmake_minimum_required(VERSION 3.16)

project(AirQualityMonitor VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")


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
)

set(SOURCES
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    mainwindow.h
)

set(FORMS
    mainwindow.ui
)

add_executable(AirQualityMonitor ${SOURCES} ${HEADERS} ${FORMS}
)

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
if(WIN32)
    target_compile_definitions(AirQualityMonitor PRIVATE
        -DUNICODE
        -D_UNICODE
    )
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
