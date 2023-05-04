cmake_minimum_required(VERSION 3.2)
project(asio-coroutines)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    asio_lib
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(
    asio_lib
)
add_library(asio INTERFACE)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
target_include_directories(asio INTERFACE ${asio_lib_SOURCE_DIR}/asio/include)

add_executable(broadcast)
target_sources(broadcast PRIVATE src/broadcast.cc)
target_link_libraries(broadcast PRIVATE asio uring)
install(TARGETS broadcast)

add_executable(blocking-receive)
target_sources(blocking-receive PRIVATE src/blocking-receive.cc)
target_link_libraries(blocking-receive PRIVATE asio uring)
install(TARGETS blocking-receive)

add_executable(thread-per-core)
target_sources(thread-per-core PRIVATE src/thread-per-core.cc)
target_link_libraries(thread-per-core PRIVATE asio uring)
install(TARGETS thread-per-core)

add_executable(coroutines)
target_sources(coroutines PRIVATE src/coroutines.cc)
target_link_libraries(coroutines PRIVATE asio uring)
install(TARGETS coroutines)
