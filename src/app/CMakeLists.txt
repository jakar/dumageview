#
# dumageview app listfile
#

include(GNUInstallDirs)

file(GLOB sources *.cpp)
add_executable(dumageview_bin ${sources})

set_target_properties(dumageview_bin PROPERTIES OUTPUT_NAME dumageview)

target_link_libraries(dumageview_bin dumageview)

install(
  TARGETS dumageview_bin
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

