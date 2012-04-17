#-------------------------------------------------
#
# Project created by QtCreator 2012-03-14T10:34:00
#
#-------------------------------------------------

QT += core gui

TARGET = Nao
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp \
           connecttonao.cpp \
           label.cpp \
           object_detection.cpp \
           motion.cpp \
    behaviorthread.cpp

HEADERS += mainwindow.h \
           connecttonao.h \
           label.h \
           object_detection.hpp \
           motion.hpp \
    behaviorthread.h

FORMS += mainwindow.ui \
         connecttonao.ui

INCLUDEPATH += $(ALDEBARAN_SDK)/include

LIBS += -L$(ALDEBARAN_SDK)/lib/ \
        -lalvision_d \
        -lcv200d \
        -lcvaux200d \
        -lcxcore200d \
        -lhighgui200d \
        -lml200 \
        -lalextractor_d \
        -lalcommon_d \
        -lalsoap_d \
        -lalvalue_d \
        -ltinyxml_d \
        -lrttools_d \
        -lalthread_d \
        -lalerror_d \
        -lpthreadVCE2 \
        -llibboost_signals-vc100-mt-gd-1_44 \
        -llibboost_program_options-vc100-mt-gd-1_44 \
        -lallogremote_d \
        -lqi_d \
        -llibboost_filesystem-vc100-mt-gd-1_44 \
        -llibboost_system-vc100-mt-gd-1_44 \
        -llibboost_thread-vc100-mt-gd-1_44 \
        -llibboost_date_time-vc100-mt-gd-1_44 \
        -lzmq_d
