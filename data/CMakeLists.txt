include(GNUInstallDirs)

install(
  FILES dumageview.desktop
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

install(
  FILES dumageview.png
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps")
