cmake_minimum_required(VERSION 3.20)
project(asio-coroutines)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_COMPILE_WARNING_AS_ERROR TRUE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined,leak")
set(CMAKE_CXX_FLAGS_TSAN "${CMAKE_CXX_FLAGS_TSAN} -fsanitize=thread")

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(
  -Wno-unused-variable
  -Wno-unused-parameter
  -Wno-unused-but-set-variable
)

include(FetchContent)
set(BOOST_INCLUDE_LIBRARIES
  asio # https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio.html
)
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.gz
  EXCLUDE_FROM_ALL
  SYSTEM
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(Boost)

add_executable(broadcast)
target_sources(broadcast PRIVATE src/broadcast.cc)
target_link_libraries(broadcast PRIVATE Boost::asio)
install(TARGETS broadcast)

add_executable(blocking-receive)
target_sources(blocking-receive PRIVATE src/blocking-receive.cc)
target_link_libraries(blocking-receive PRIVATE Boost::asio)
install(TARGETS blocking-receive)

add_executable(thread-per-core)
target_sources(thread-per-core PRIVATE src/thread-per-core.cc)
target_link_libraries(thread-per-core PRIVATE Boost::asio)
install(TARGETS thread-per-core)

add_executable(coroutines)
target_sources(coroutines PRIVATE src/coroutines.cc)
target_link_libraries(coroutines PRIVATE Boost::asio)
install(TARGETS coroutines)
