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

    bool export_paths();
    void plan_path();
    bool setup_planning(QString filename);

  protected:
    void keyPressEvent(QKeyEvent * e);
    void update_status();

    QAction* mpLoadObjAction;

    QMenu*   mpToolMenu;
    QAction* mpSaveScreenAction;
    QAction* mpExportGrammarGraphAction;
    QAction* mpExportAllSimpleStringsAction;

    BIRRTstarConfig*   mpBIRRTstarConfig;
    BIRRTstar*         mpBIRRTstar;

    QProgressBar* mpStatusProgressBar;

  protected slots:
    void onRun();
    void onSaveScreen();

    void onLoadMap();
    void onLoadObj();
    void onAddStart();
    void onAddGoal();

  };

}

#endif // BIRRTSTARMAINWINDOW_H
