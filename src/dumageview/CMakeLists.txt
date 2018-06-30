#
# libdumageview listfile
#

#
# library prep
#

# qt
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

#
# project source
#

file(GLOB sources *.cpp)
add_library(dumageview STATIC ${sources})

#
# dependencies
#

# qt
target_compile_definitions(dumageview PUBLIC QT_NO_KEYWORDS)
target_link_libraries(dumageview PUBLIC Qt5::Widgets)

# boost
find_package(
  Boost 1.65 REQUIRED COMPONENTS filesystem program_options)

target_link_libraries(
  dumageview PUBLIC Boost::filesystem Boost::program_options)

# fmt
target_link_libraries(dumageview PUBLIC fmt::fmt)

# glm
target_include_directories(
  dumageview PUBLIC "${PROJECT_SOURCE_DIR}/thirdparty/glm")

# spdlog
target_compile_definitions(dumageview PUBLIC SPDLOG_FMT_EXTERNAL)
target_include_directories(
  dumageview PUBLIC "${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include")

set(DUMAGEVIEW_ENABLE_DEBUG OFF CACHE BOOL "Enable debug logging")
set(DUMAGEVIEW_ENABLE_TRACE OFF CACHE BOOL "Enable trace logging")

if (${DUMAGEVIEW_ENABLE_DEBUG} OR ${DUMAGEVIEW_ENABLE_TRACE})
  target_compile_definitions(dumageview PUBLIC SPDLOG_DEBUG_ON)
endif ()

if (${DUMAGEVIEW_ENABLE_TRACE})
  target_compile_definitions(dumageview PUBLIC SPDLOG_TRACE_ON)
endif ()

# threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(dumageview PUBLIC Threads::Threads)

