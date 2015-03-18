#include <QtWidgets>
#include <QtTest/QtTest>

class LineEditTest: public QObject
{
  Q_OBJECT

  private slots:
    void testGui();
//    void testOther();

  signals:
    void signal_select_choose(bool);
};
