#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QKeyEvent>
#include <QStatusBar>
#include "birrtstar_config.h"
#include "birrtstar_window.h"
#include "img_load_util.h"
//#include "expanding_tree.h"

using namespace std;
using namespace h2p;
using namespace birrts;

BIRRTstarWindow::BIRRTstarWindow(BIRRTstarViz* p_viz, QWidget *parent)
    : SpatialRelationsWindow((SpatialRelationsViz*)p_viz, parent) {

  mpBIRRTstar = NULL;

  mpBIRRTstarConfig = new BIRRTstarConfig(this);
  mpBIRRTstarConfig->hide();

  setCentralWidget(mpViz);

  mpStatusLabel = new QLabel();
  mpStatusLabel->setFixedWidth(120);
  /*
  mpStatusProgressBar = new QProgressBar();
    
  statusBar()->addWidget(mpStatusLabel);
  statusBar()->addWidget(mpStatusProgressBar);
  */
  disconnect(mpLoadAction, SIGNAL(triggered()), 0, 0);
  connect(mpLoadAction, SIGNAL(triggered()), this, SLOT(onLoadMap()));
  mpLoadObjAction = new QAction("Config Objective", this);
  connect(mpLoadObjAction, SIGNAL(triggered()), this, SLOT(onLoadObj()));

  disconnect(mpExecuteAction, SIGNAL(triggered()), 0, 0);
  connect(mpExecuteAction, SIGNAL(triggered()), this, SLOT(onRun()));
  mpSaveScreenAction = new QAction("Save Screen", this);
  connect(mpSaveScreenAction, SIGNAL(triggered()), this, SLOT(onSaveScreen()));

  mpToolMenu = menuBar()->addMenu("&Tool");
  mpToolMenu->addAction(mpLoadObjAction);
  mpToolMenu->addAction(mpSaveScreenAction);

  disconnect(mpAddStartAction, SIGNAL(triggered()), 0, 0);
  disconnect(mpAddGoalAction, SIGNAL(triggered()), 0, 0);
  connect(mpAddStartAction, SIGNAL(triggered()), this, SLOT(onAddStart()));
  connect(mpAddGoalAction, SIGNAL(triggered()), this, SLOT(onAddGoal()));
}

BIRRTstarWindow::~BIRRTstarWindow() {
  if(mpBIRRTstarConfig) {
    delete mpBIRRTstarConfig;
    mpBIRRTstarConfig = NULL;
  }
  if(mpBIRRTstar) {
    delete mpBIRRTstar;
    mpBIRRTstar = NULL;
  }
  if(mpViz) {
    delete mpViz;
    mpViz = NULL;
  }
}

void BIRRTstarWindow::onLoadObj() {
  mpBIRRTstarConfig->exec();
}

bool BIRRTstarWindow::setupPlanning(QString filename) {
  if(mpViz) {
    BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
    p_viz->m_PPInfo.load_from_file(filename);
    //openMap(p_viz->m_PPInfo.m_map_fullpath);
    if(mpBIRRTstarConfig) {
      mpBIRRTstarConfig->updateDisplay();
    }
    return true;
  }
  return false;
}

bool BIRRTstarWindow::exportPaths() {
  if(mpViz) {
    BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
    bool success = false;
    success = p_viz->m_PPInfo.export_paths(p_viz->m_PPInfo.m_paths_output);
    success = p_viz->draw_path(p_viz->m_PPInfo.m_paths_output+".png");
    return success;
  }
  return false;
}

void BIRRTstarWindow::onRun() {
  cout << "BIRRTstarWindow::onRun" << endl;
  if(mpViz) {
    BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
    if (p_viz->m_PPInfo.m_map_width <= 0 || p_viz->m_PPInfo.m_map_height <= 0) {
      QMessageBox msgBox;
      msgBox.setText("Map is not initialized.");
      msgBox.exec();
      return;
    }
    if(p_viz->m_PPInfo.m_start.x()<0 || p_viz->m_PPInfo.m_start.y()<0) {
      QMessageBox msgBox;
      msgBox.setText("Start is not set.");
      msgBox.exec();
      return;
    }
    if(p_viz->m_PPInfo.m_goal.x()<0 || p_viz->m_PPInfo.m_goal.y()<0) {
      QMessageBox msgBox;
      msgBox.setText("Goal is not set.");
      msgBox.exec();
      return;
    }

    planPath();
    repaint();
  }

}

void BIRRTstarWindow::planPath() {
  if(mpBIRRTstar) {
    delete mpBIRRTstar;
    mpBIRRTstar = NULL;
  }
  if (mpViz==NULL) {
      return;
  }

  BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
  for( std::vector<Path*>::iterator it = p_viz->m_PPInfo.mp_found_paths.begin();
       it != p_viz->m_PPInfo.mp_found_paths.end(); it ++ ) {
    Path * p_path = (*it);
    delete p_path;
    p_path = NULL;
  }
  p_viz->m_PPInfo.mp_found_paths.clear();

  p_viz->m_PPInfo.init_func_param();
  QString msg = "INIT RRTstar ... \n";
  msg += "SegmentLen( " + QString::number(p_viz->m_PPInfo.m_segment_length) + " ) \n";
  msg += "MaxIterationNum( " + QString::number(p_viz->m_PPInfo.m_max_iteration_num) + " ) \n";
  qDebug() << msg;

  ReferenceFrameSet* pfs = p_viz->get_reference_frame_set();
  WorldMap* p_map = pfs->get_world_map();
  mpBIRRTstar = new BIRRTstar(p_map->get_width(), p_map->get_height(), p_viz->m_PPInfo.m_segment_length);
  mpBIRRTstar->set_reference_frames( pfs );
  mpBIRRTstar->set_run_type( p_viz->m_PPInfo.m_run_type );
  POS2D start(p_viz->m_PPInfo.m_start.x(), p_viz->m_PPInfo.m_start.y());
  POS2D goal(p_viz->m_PPInfo.m_goal.x(), p_viz->m_PPInfo.m_goal.y());

  p_viz->get_spatial_relation_mgr()->get_string_classes( p_viz->get_reference_frame_set() );
    
  mpBIRRTstar->init(start, goal, p_viz->m_PPInfo.mp_func, p_viz->m_PPInfo.mCostDistribution, p_viz->m_PPInfo.m_grammar_type);
  p_viz->m_PPInfo.get_obstacle_info(mpBIRRTstar->get_map_info());
  p_viz->set_tree(mpBIRRTstar);
  p_viz->set_finished_planning( false );
    

  //mpBIRRTstar->dump_distribution("dist.txt");
  while(mpBIRRTstar->get_current_iteration() <= p_viz->m_PPInfo.m_max_iteration_num) {
    QString msg = "CurrentIteration " + QString::number(mpBIRRTstar->get_current_iteration()) + " ";
    mpBIRRTstar->extend();
    msg += QString::number(mpBIRRTstar->get_string_class_mgr()->get_string_classes().size()); 
    qDebug() << msg;

    repaint();
  }
  qDebug() << "START MERGE ";
  mpBIRRTstar->get_string_class_mgr()->merge();
  qDebug() << "END MERGE ";
  //Path* path = mpBIRRTstar->find_path();
  std::vector<Path*> p_paths = mpBIRRTstar->get_paths();
  p_viz->m_PPInfo.load_paths(p_paths);
  p_viz->set_finished_planning( true );
  repaint();
}

void BIRRTstarWindow::onSaveScreen() {
  QString tempFilename = QFileDialog::getSaveFileName(this, tr("Save PNG File"), "./", tr("PNG Files (*.png)"));
  BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
  p_viz->save_current_viz( tempFilename );
}

void BIRRTstarWindow::onAddStart() {
  if( mpViz ) {
    BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
    if( p_viz->get_spatial_relation_mgr() ) {
      p_viz->get_spatial_relation_mgr()->m_start_x = mCursorPoint.x();
      p_viz->get_spatial_relation_mgr()->m_start_y = mCursorPoint.y();
      p_viz->m_PPInfo.m_start = mCursorPoint;
      repaint();
    }
  }
}

void BIRRTstarWindow::onAddGoal() {
  if( mpViz ) {
    BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
    if( p_viz->get_spatial_relation_mgr() ) {
      p_viz->get_spatial_relation_mgr()->m_goal_x = mCursorPoint.x();
      p_viz->get_spatial_relation_mgr()->m_goal_y = mCursorPoint.y();
      p_viz->m_PPInfo.m_goal = mCursorPoint;
      repaint();
    }
  }
}

void BIRRTstarWindow::onLoadMap() {
  QString tempFilename = QFileDialog::getOpenFileName(this,
            tr("Open File"), "./", tr("Map Files (*.*)"));
  if( tempFilename.isEmpty() == false ) {
    if( mpViz ) {
      BIRRTstarViz* p_viz = dynamic_cast<BIRRTstarViz*>(mpViz);
      QFileInfo fileInfo(tempFilename);
      QString filename(fileInfo.fileName());
      p_viz->load_map(tempFilename);
      p_viz->m_PPInfo.m_map_filename = filename;
      p_viz->m_PPInfo.m_map_fullpath = tempFilename;
      p_viz->m_PPInfo.m_map_width = p_viz->get_width();
      p_viz->m_PPInfo.m_map_height = p_viz->get_height();
      updateStatusBar();
    }
  }
}
