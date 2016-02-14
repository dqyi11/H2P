#ifndef BIRRTSTARMAINWINDOW_H
#define BIRRTSTARMAINWINDOW_H

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QPixmap>
#include <QProgressBar>
#include "birrtstar_viz.h"
#include "reference_frames.h"
#include "spatial_relations_window.h"

namespace birrts {

  class BIRRTstarConfig;

  class BIRRTstarWindow : public h2p::SpatialRelationsWindow {
    Q_OBJECT
    
  public:
    BIRRTstarWindow(BIRRTstarViz* p_viz = new BIRRTstarViz(), QWidget *parent = 0);
    ~BIRRTstarWindow();

    bool exportPaths();
    void planPath();
    bool setupPlanning(QString filename);

  protected:

    BIRRTstarConfig*   mpBIRRTstarConfig;
    BIRRTstar*         mpBIRRTstar;

  protected slots:
    void onRun();
    void onSaveScreen();

  };

}

#endif // BIRRTSTARMAINWINDOW_H
