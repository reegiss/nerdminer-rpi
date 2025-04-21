# tests/CMakeLists.txt

add_executable(test_miner test_main.cpp)

# (NÃO precisa de target_link_libraries aqui por enquanto)

add_test(NAME TestNerdMiner COMMAND test_miner)
