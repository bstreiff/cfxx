#=================================
# List all the source files for CFXXTEST
#=================================
set( CFXXTEST_SOURCE_FILES
    "${CFXX_SOURCE_DIR}/tests/StringTests.cpp"
    "${CFXX_SOURCE_DIR}/tests/TypeCheckingTests.cpp"
   )

#=================================
# Setup where to search for header files
#=================================
set( CFXXTEST_INCLUDE_DIRECTORIES
   "${CFXX_SOURCE_DIR}"
   "${gtest_SOURCE_DIR}/include"
   "${gtest_SOURCE_DIR}"
   )

#=================================
# Setup the libraries CFXXTEST links to
#=================================
set( CFXXTEST_LINK_LIBRARIES
    ${gtest_LIBRARIES}
   )

#=================================
# Setup OSX build settings
#=================================
if( APPLE )
   find_library( COREFOUNDATION_FRAMEWORK CoreFoundation )
   list( APPEND CFXXTEST_LINK_LIBRARIES ${COREFOUNDATION_FRAMEWORK} )
endif()

#=================================
# Define the build target for CFXXTEST
#=================================

include_directories(
   ${CFXXTEST_INCLUDE_DIRECTORIES} )

add_executable(
   CfxxTests
   ${CFXXTEST_SOURCE_FILES}
   ${CFXXTEST_HEADER_FILES} )

target_link_libraries(
   CfxxTests
   gtest
   gtest_main
   ${CFXXTEST_LINK_LIBRARIES} )

# Define a target that will run the component test binary
# as part of the build process.
add_custom_target(
   CfxxTestRunner ALL
   COMMAND ${CFXX_BINARY_DIR}/bin/CfxxTests
   )
add_dependencies(CfxxTestRunner CfxxTests)
