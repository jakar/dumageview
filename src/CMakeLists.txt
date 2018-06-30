#
# dumageview src listfile
#

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler flags
# TODO: add more; separate clang and gcc
set(
  CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
  -Wall \
  -Wextra \
  -Wshadow \
  -Wformat-nonliteral \
  -Wformat-security \
  -Wnon-virtual-dtor \
  -Wno-unused-parameter \
  "
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

set(sanitizer_flags "-fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${sanitizer_flags}")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} ${sanitizer_flags}")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${sanitizer_flags}")

#
# source subdirs
#

include_directories(
  BEFORE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
)

add_subdirectory(dumageview)
add_subdirectory(app)

