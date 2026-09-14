cmake_minimum_required(VERSION 3.16)

project(gensrc VERSION 1.0.0 LANGUAGES NONE)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gensrcConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/gensrcConfig.cmake
  INSTALL_DESTINATION lib/cmake/gensrc
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/gensrcConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES gensrc.py DESTINATION share/gensrc)

install(DIRECTORY gensrc DESTINATION share/gensrc)

install(
	FILES 
		${CMAKE_CURRENT_BINARY_DIR}/gensrcConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/gensrcConfigVersion.cmake
	DESTINATION
		lib/cmake/gensrc
)
