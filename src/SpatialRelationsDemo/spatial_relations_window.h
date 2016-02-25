#ifndef SPATIAL_INFER_WINDOW_H
#define SPATIAL_INFER_WINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QPixmap>
#include <QMessageBox>
#include "spatial_relations_viz.h"

namespace h2p {
  
  class SpatialRelationsConfig;

  class SpatialRelationsWindow : public QMainWindow {
    Q_OBJECT
    
  public:
    SpatialRelationsWindow(QWidget *parent = 0);
    ~SpatialRelationsWindow();

    SpatialRelationsViz * mpViz;

  protected:
    virtual void createMenuBar();
    virtual void createActions();
    virtual void updateStatusBar();

    void keyPressEvent(QKeyEvent * e);

  
    QMessageBox*  mpMsgBox;
    QMenu*        mpFileMenu;
    QMenu*        mpAddMenu;
    QMenu*        mpAddSideofRelationMenu;
    QMenu*        mpManageMenu;
    QAction*      mpOpenAction;
    QAction*      mpSaveAction;
    QAction*      mpLoadAction;
    QLabel*       mpStatusLabel;

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

    SpatialRelationsConfig* mpConfig;

    QPoint        mCursorPoint;
  protected slots:
    void contextMenuRequested( QPoint point );
    void onOpen();
    void onSave();
    void onLoadMap();
    void onAddStart();
    void onAddGoal();

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
    void onExecute();
  };
}

#endif // SPATIAL_INFER_WINDOW_H
