find_package(Qt5Test REQUIRED)

include(CTest)
enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
add_executable(s3_test s3_test.cpp ../src/AWSClient.cpp)
add_test(NAME s3_test COMMAND s3_test)
target_link_libraries(s3_test PRIVATE Qt5::Test Qt5::Core KF5::KIOCore ${AWSSDK_LINK_LIBRARIES})
