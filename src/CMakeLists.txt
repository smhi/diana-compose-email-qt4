
FIND_PACKAGE(Qt4 REQUIRED COMPONENTS QtGui QtCore)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(-W -Wall ${QT_DEFINITIONS})

SET(CE_SRCS
  attached_files_model.cc
  compose_email.cc
  main.cc
)

SET(CE_MOC_HDRS
  compose_email.h
)
QT4_WRAP_CPP(CE_MOC_SRCS ${CE_MOC_HDRS})

SET(CE_TRANSLATIONS
  compose_email_de.ts
  compose_email_es.ts
  compose_email_nb.ts
  compose_email_ru.ts
  compose_email_sv.ts
)
QT4_ADD_TRANSLATION(CE_QM ${CE_TRANSLATIONS})

SET(CE_RESOURCES compose_email.qrc)
SET(CE_GEN_RESOURCES compose_email_generated.qrc)
CONFIGURE_FILE(${CE_GEN_RESOURCES} ${CE_GEN_RESOURCES} COPYONLY)
qt4_add_resources(CE_QRC_SRCS ${CE_RESOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${CE_GEN_RESOURCES})

#QT4_WRAP_UI(CE_UI_HEADERS ${CE_UIS})

ADD_EXECUTABLE(diana_compose_email
  ${CE_SRCS}
  ${CE_MOC_SRCS}
  ${CE_QM}
  ${CE_QRC_SRCS}
  ${CE_UIS}
)

TARGET_LINK_LIBRARIES(diana_compose_email
  ${QT_LIBRARIES}
)

INSTALL(TARGETS diana_compose_email DESTINATION bin)
