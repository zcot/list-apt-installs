cmake_minimum_required(VERSION 3.5)
#cmake_minimum_required(VERSION 2.6)


#set(CMAKE_C_COMPILER "gcc-4.2")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.2")



project(list-apt-installs)

# binary
add_executable(list-apt-installs source/list-apt-installs.c)
install(TARGETS list-apt-installs DESTINATION bin)
# alias links
macro(install_symlink filepath sympath)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${filepath} ${sympath})")
    install(CODE "message(\"-- Created symlink: ${sympath} -> ${filepath}\")")
endmacro(install_symlink)
install_symlink(${CMAKE_PROJECT_NAME} lai)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lai DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# manual
install(FILES man/list-apt-installs.1 DESTINATION /usr/local/share/man/man1)
# opt files
install(FILES README.md DESTINATION /usr/share/doc/${CMAKE_PROJECT_NAME})
install(FILES COPYING DESTINATION /usr/share/doc/${CMAKE_PROJECT_NAME})
install(FILES LICENSE DESTINATION /usr/share/doc/${CMAKE_PROJECT_NAME})
install(FILES INSTALL DESTINATION /usr/share/doc/${CMAKE_PROJECT_NAME})


# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

