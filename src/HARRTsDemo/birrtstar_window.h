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

  class BIRRTstarWindow : public SpatialRelationsWindow {
    Q_OBJECT
    
  public:
    BIRRTstarWindow(QWidget *parent = 0);
    ~BIRRTstarWindow();

    bool exportPaths();
    void planPath();
    bool setupPlanning(QString filename);
    BIRRTstarViz * mpViz;

    void keyPressEvent(QKeyEvent * e);

  protected:
    void createMenuBar();
    void createActions();
    bool openMap(QString filename);
    void updateStatus();

  private:
    void updateTitle();

    QAction* mpLoadObjAction;
    QAction* mpRunAction;

    QMenu*   mpToolMenu;
    QAction* mpSaveScreenAction;
    QAction* mpExportGrammarGraphAction;
    QAction* mpExportAllSimpleStringsAction;

    QLabel*       mpStatusLabel;
    QProgressBar* mpStatusProgressBar;

    QPixmap* mpMap;
    QPoint   mCursorPoint;

    BIRRTstarConfig*   mpBIRRTstarConfig;
    BIRRTstar*         mpBIRRTstar;
    h2p::ReferenceFrameSet* mpReferenceFrameSet;

  private slots:
    void contextMenuRequested(QPoint point);
    void onExport();
    void onLoadObj();
    void onRun();
    
    void onSaveScreen();
    void onExportGrammar();
    void onExportAllSimpleStrings();
  };

}

#endif // BIRRTSTARMAINWINDOW_H
