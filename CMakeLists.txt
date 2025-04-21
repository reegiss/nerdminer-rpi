cmake_minimum_required(VERSION 3.10)

project(nerdminer-rpi VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir diretórios de cabeçalho
include_directories(${PROJECT_SOURCE_DIR}/include)

# Definir onde o executável será gerado
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Verificar se o diretório de testes existe e adicionar
if(EXISTS ${PROJECT_SOURCE_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
else()
    message(WARNING "Diretório tests não encontrado.")
endif()

# Adicionar o executável principal
add_executable(nerdminer
    src/main.cpp
)

# Se o Raspberry Pi for detectado, adicionar flags específicas
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    message(STATUS "Configuração para Raspberry Pi 4 (arm64)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=armv8-a")
endif()

# Definir macros de compilação
target_compile_definitions(nerdminer PRIVATE -DPROJECT_NAME="nerdminer-rpi")

# Adicionar o teste
add_test(NAME TestNerdMiner COMMAND nerdminer)