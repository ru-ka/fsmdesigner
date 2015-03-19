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
#include <QSignalSpy>

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
  // Connect signals
  //connect( this, SIGNAL(signal_select_choose(bool) ), w.action_Choose,
  //    SLOT( setChecked(bool) ) );
  //emit signal_select_choose( true );
  QSignalSpy spy( w.action_Choose, SIGNAL(triggered() ) );



  //w.action_State->trigger();
  QTest::qWait(3000);
  //w.action_New->trigger();
  //QTest::qWait(1000);
  //QTest::mouseMove( &w, QPointF() ); // Move to center?
  //QTest::mouseMove(w.centralwidget, w.centralwidget->geometry().center() );
  QAction * chooseAction = w.action_Choose;
  auto choose_action_rect = w.menu_Objects->actionGeometry( chooseAction );
  QTest::mouseMove( &w, choose_action_rect.center() );
  QTest::mousePress( &w, Qt::LeftButton, 0, choose_action_rect.center() );
  QTest::mouseRelease( &w, Qt::LeftButton, 0, choose_action_rect.center() );
  QTest::qWait( 10 );
  std::cout << "spy.count << " << std::endl;
  std::cout << spy.count() << std::endl ;
  QTest::qWait(2000);
  //QTest::mousePress(&w, Qt::LeftButton, 0, w.centralwidget->geometry().center() );
  //QTest::qWait(10);
  //QTest::mouseRelease(&w, Qt::LeftButton, 0, w.centralwidget->geometry().center() );
  //QTest::qWait(1000);
  //QTest::mouseMove( &w, w.action_ZoomIn->geometry().center() );
  //QTest::keyClicks( &w, QString("Hallo"), Qt::NoModifier, 1304);
  //QTest::mouseMoveEvent(& 
}

QTEST_MAIN( LineEditTest )
