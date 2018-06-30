set(
  DUMAGEVIEW_BUILD_PLUGINS OFF CACHE BOOL
  "Build plugins as part of default target")

file(GLOB subdirs LIST_DIRECTORIES true *)

foreach (subdir IN LISTS subdirs)
  if (NOT IS_DIRECTORY "${subdir}")
    continue()
  endif ()

  if (${DUMAGEVIEW_BUILD_PLUGINS})
    add_subdirectory("${subdir}")
  else ()
    add_subdirectory("${subdir}" EXCLUDE_FROM_ALL)
  endif ()
endforeach ()
