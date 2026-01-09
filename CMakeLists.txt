cmake_minimum_required(VERSION 3.5)

project(TaskManager LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE STRING "" FORCE)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
)

add_executable(TaskManager
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    DatabaseManager.h
    DatabaseManager.cpp
    TaskDialog.h
    TaskDialog.cpp
    TaskDialog.ui
    DeadlineChecker.h
    DeadlineChecker.cpp
)

#Добавить ВСЕ необходимые Qt модули
target_link_libraries(TaskManager PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
)

# Копировать ресурсы, если есть
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Цель для развертывания (deploy)
if(WIN32)
    # Найти windeployqt
    get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_target(deploy
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --release
                    --no-translations
                    --dir "${CMAKE_CURRENT_BINARY_DIR}/deploy"
                    "$<TARGET_FILE:TaskManager>"
            COMMENT "Развертывание приложения с зависимостями Qt"
        )
    endif()
endif()
