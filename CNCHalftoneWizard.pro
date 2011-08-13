CONFIG += qt
CONFIG += uitools debug_and_release
QT += webkit
UI_HEADERS_DIR = ./ui_inc
MOC_DIR = ./moc
RCC_DIR = ./qrc
OBJECTS_DIR = ./release
JAM_BUILD_TYPE = release
DESTDIR = ./release
CONFIG(debug,debug|release) {
	OBJECTS_DIR = ./debug
	DESTDIR = ./debug
}
TARGET = CNCHalftoneWizard

win32 {
	SLASH = \\
}
linux-g++ {
	SLASH = /
}

INCLUDEPATH += src
INCLUDEPATH += $${UI_HEADERS_DIR}


TEMPLATE = app

FORMS = ui/ConfigureDlg.ui \
		ui/MainWindow.ui 
						
RESOURCES = ui/res/HTCNC.qrc

SOURCES += \
			src/HTCNCConfigureDlg.cpp \
			src/HTCNCConsole.cpp \
			src/HTCNCHalftoner.cpp \
			src/HTCNCMain.cpp \
			src/HTCNCMainWindow.cpp 

HEADERS += \
			src/HTCNCConfigureDlg.h \
			src/HTCNCConsole.h \
			src/HTCNCHalftoner.h \
			src/HTCNCMainWindow.h 

			
