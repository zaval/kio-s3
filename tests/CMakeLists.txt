include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/HEAD.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        s3_test
        s3_test.cc
        ../src/S3FileSystem.cpp
        ../src/AWSClientAbstract.cpp
        ../src/AWSClient.cpp
)
target_link_libraries(
        s3_test
        GTest::gtest_main
        GTest::gmock
        KF5::KIOCore
        ${AWSSDK_LINK_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(s3_test)