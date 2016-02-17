#ifndef BIRRTSTARMAINWINDOW_H
#define BIRRTSTARMAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QPixmap>
#include <QProgressBar>
#include <QMessageBox>
#include <QPixmap>
#include "birrtstar_viz.h"
#include "reference_frames.h"
#include "spatial_relations_config.h"

namespace birrts {

  class BIRRTstarConfig;

  class BIRRTstarWindow : public QMainWindow {
    Q_OBJECT
    
  public:
    BIRRTstarWindow(QWidget *parent = 0);
    ~BIRRTstarWindow();

    bool export_paths();
    void plan_path();
    bool setup_planning(QString filename);

    BIRRTstarViz* mpViz;

  protected:
    void keyPressEvent(QKeyEvent * event);
    void update_status();

    BIRRTstarConfig*   mpBIRRTstarConfig;
    BIRRTstar*         mpBIRRTstar;

    virtual void createMenuBar();
    virtual void createActions();

    QMessageBox*  mpMsgBox;
    QMenu*        mpFileMenu;
    QMenu*        mpAddMenu;
    QMenu*        mpAddSideofRelationMenu;
    QMenu*        mpManageMenu;
    QAction*      mpOpenAction;
    QAction*      mpSaveAction;
    QAction*      mpLoadAction;
    QAction*      mpLoadObjAction;
    QLabel*       mpStatusLabel;
    QProgressBar* mpStatusProgressBar;

    QAction*      mpAddStartAction;
    QAction*      mpAddGoalAction;
    QMenu*        mpContextMenu;

    QAction*      mpAddInbetweenSpatialRelationAction;
    QAction*      mpAddAvoidSpatialRelationAction;
    QAction*      mpAddLeftofSpatialRelationAction;
    QAction*      mpAddRightofSpatialRelationAction;
    QAction*      mpAddTopofSpatialRelationAction;
    QAction*      mpAddBottomofSpatialRelationAction;

    QAction*      mpShowConfigAction;
    QAction*      mpProcessAction;
    QAction*      mpExecuteAction;

    QMenu*   mpToolMenu;
    QAction* mpSaveScreenAction;
    QAction* mpExportGrammarGraphAction;
    QAction* mpExportAllSimpleStringsAction;

    h2p::SpatialRelationsConfig* mpConfig;

    QPoint        mCursorPoint;

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
