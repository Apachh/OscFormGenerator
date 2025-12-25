cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME FormGenerator)

project(${PROJECT_NAME} LANGUAGES CXX C)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(CMAKE_C_COMPILER gcc-14)
set(CMAKE_CXX_COMPILER g++-14)
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lstdc++ -lsupc++ -Wl,--end-group")

# Создаем исполняемый объектный файл проекта
add_executable(${PROJECT_NAME})

# Заполнение src файлов 
file(GLOB SOURCE_FILES 
    *.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

# Добавляем пути до заголовочных файлов
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

