#include "test.h"
#include <QApplication>
#include <QTest>
//#include "include/gui/mergedGUI/MergedMainWindow.h"
//#include "include/gui/mergedGUI/MergedMainWindow.ui"

// Includes
//--------------


#include <iostream>
using namespace std;

//-- Qt
#include <QtWidgets>

//-- Core
#include <core/Core.h>
#include <core/Project.h>
#include <core/LoadFactory.h>

//-- Generate
#include <generate/GeneratorFactory.h>
#include <genverilog/VerilogGenerator.h>
#include <genverilog/SimvisionMmapGenerator.h>

//-- Gui
#include <gui/FSMDesignerApplication.h>
#include <gui/mergedGUI/MergedMainWindow.h>

void LineEditTest::testGui() {
  MergedMainWindow w;
  w.show();
//  QTest::mouseMoveEvent(& 
}

QTEST_MAIN( LineEditTest )
