add_definitions(-DTRANSLATION_DOMAIN=\"kio_aws_s3\")

add_library(kio_aws_s3 AWSClientAbstract.cpp AWSClientAbstract.h S3FileSystem.cpp S3FileSystem.h)
set_target_properties(kio_aws_s3 PROPERTIES OUTPUT_NAME "s3")

target_sources(kio_aws_s3 PRIVATE
    kio_aws_s3.cpp
    AWSClientAbstract.cpp
    AWSClient.cpp
    S3FileSystem.cpp
)

ecm_qt_declare_logging_category(kio_aws_s3
    HEADER kio_aws_s3_log.h
    IDENTIFIER KIO_AWS_S3_LOG
    CATEGORY_NAME kf.kio.workers.kio_aws_s3
    DESCRIPTION "KIO worker (kio_aws_s3)"
    DEFAULT_SEVERITY Debug # Change "Debug" to "Info" once doing first releases
    EXPORT KIO_AWS_S3
)

target_link_libraries(kio_aws_s3
    KF5::KIOCore
#    KF5::I18n
    ${AWSSDK_LINK_LIBRARIES}
)

install(TARGETS kio_aws_s3 DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kio)
install(FILES kio_aws_s3.desktop DESTINATION ${KDE_INSTALL_DATADIR}/remoteview)
ecm_qt_install_logging_categories(
    EXPORT KIO_AWS_S3
    FILE kio_aws_s3.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

message("Select aws libraries to install")
FOREACH (x IN LISTS AWSSDK_LIBRARIES)
    message("${AWSSDK_LIB_DIR}/lib${x}.so -> ${CMAKE_INSTALL_FULL_LIBDIR}/")
    install(FILES "${AWSSDK_LIB_DIR}/lib${x}.so" DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
ENDFOREACH ()
