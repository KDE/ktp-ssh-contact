project(ktp-ssh-contact)

set (KTP_SSH_CONTACT_VERSION "0.4")

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

set(KDE_MIN_VERSION "4.7.0")
set(IS_KTP_INTERNAL_MODULE TRUE)
find_package(KDE4 REQUIRED)
find_package(TelepathyQt4 0.8.9 REQUIRED)
find_package(KTp REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

# Add the compiler flags
add_definitions (${QT_DEFINITIONS}
                 ${KDE4_DEFINITIONS}
                 -DQT_NO_CAST_FROM_ASCII
                 -DQT_NO_KEYWORDS
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${KDE4_INCLUDES}
                    ${TELEPATHY_QT4_INCLUDE_DIR}
                    ${KTP_INCLUDE_DIR}
)

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

add_subdirectory(src)

macro_display_feature_log()
