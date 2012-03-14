set(KTP_SSH_CONTACT_SRCS
    mainwindow.cpp
    main.cpp
)

set(KTP_SSH_CONTACT_UI
    mainwindow.ui)

kde4_add_ui_files(KTP_SSH_CONTACT_SRCS ${KTP_SSH_CONTACT_UI})

kde4_add_executable(ktp-ssh-contact ${KTP_SSH_CONTACT_SRCS})

target_link_libraries(ktp-ssh-contact
                      ${TELEPATHY_QT4_LIBRARIES}
                      ${KTP_LIBRARIES}
                      ${KTP_MODELS_LIBRARIES}
                      ${KTP_WIDGETS_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
)

install(TARGETS ktp-ssh-contact ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ktp-ssh-contact.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
