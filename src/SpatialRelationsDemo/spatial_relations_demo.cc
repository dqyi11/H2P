//#include <QtGui/QApplication>
#include <QApplication>
#include "spatial_relations_window.h"

using namespace h2p;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  SpatialRelationsWindow w;
  w.show();
    
  return a.exec();
}
