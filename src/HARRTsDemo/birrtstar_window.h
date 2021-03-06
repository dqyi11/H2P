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

namespace birrts {

  class BIRRTstarConfig;
  class BIRRTstarSpatialRelationsConfig;

  class BIRRTstarWindow : public QMainWindow {
    Q_OBJECT
    
  public:
    BIRRTstarWindow(QWidget *parent = 0);
    ~BIRRTstarWindow();

    bool export_paths();
    void plan_path();
    void process();
    bool setup_planning(QString filename);

    BIRRTstarViz* mpViz;

  protected:
    void keyPressEvent(QKeyEvent * event);
    void update_status();

    BIRRTstarSpatialRelationsConfig* mpConfig;
    BIRRTstarConfig*   mpBIRRTstarConfig;
    BIRRTstar*         mpBIRRTstar;

    virtual void createMenuBar();
    virtual void createActions();
    void open_map(QString filename);

    QMessageBox*  mpMsgBox;
    QMenu*        mpFileMenu;
    QMenu*        mpAddMenu;
    QMenu*        mpAddSideofRelationMenu;
    QMenu*        mpManageMenu;
    QAction*      mpOpenAction;
    QAction*      mpSaveAction;
    QAction*      mpLoadAction;
    QAction*      mpExportAction;
    QAction*      mpLoadObjAction;
    QLabel*       mpStatusLabel;
    QProgressBar* mpStatusProgressBar;

    QAction*      mpAddStartAction;
    QAction*      mpAddGoalAction;
    QMenu*        mpContextMenu;

    QAction*      mpAddInbetweenSpatialRelationAction;
    QAction*      mpAddLeftofSpatialRelationAction;
    QAction*      mpAddRightofSpatialRelationAction;
    QAction*      mpAddTopofSpatialRelationAction;
    QAction*      mpAddBottomofSpatialRelationAction;

    QAction*      mpAddAvoidInbetweenSpatialRelationAction;
    QAction*      mpAddAvoidLeftofSpatialRelationAction;
    QAction*      mpAddAvoidRightofSpatialRelationAction;
    QAction*      mpAddAvoidTopofSpatialRelationAction;
    QAction*      mpAddAvoidBottomofSpatialRelationAction;

    QAction*      mpShowConfigAction;
    QAction*      mpProcessAction;
    QAction*      mpExecuteAction;

    QMenu*   mpToolMenu;
    QAction* mpSaveScreenAction;
    QAction* mpExportGrammarGraphAction;
    QAction* mpExportAllSimpleStringsAction;

    QPoint        mCursorPoint;

  protected slots:
    void contextMenuRequested( QPoint point );
    void onOpen();
    void onSave();
    void onLoadMap();
    void onExportPaths();
    void onAddStart();
    void onAddGoal();
    void onLoadObj();

    void onAddInbetweenSpatialRelation();
    void onAddLeftofSpatialRelation();
    void onAddRightofSpatialRelation();
    void onAddTopofSpatialRelation();
    void onAddBottomofSpatialRelation();

    void onAddAvoidInbetweenSpatialRelation();
    void onAddAvoidLeftofSpatialRelation();
    void onAddAvoidRightofSpatialRelation();
    void onAddAvoidTopofSpatialRelation();
    void onAddAvoidBottomofSpatialRelation();

    void onShowConfig();
    void onProcess();
    void onRun();
    void onSaveScreen();
  };

}

#endif // BIRRTSTARMAINWINDOW_H
