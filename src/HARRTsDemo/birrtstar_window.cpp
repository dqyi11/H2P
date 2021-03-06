#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QKeyEvent>
#include <QStatusBar>
#include <QApplication>
#include "birrtstar_config.h"
#include "birrtstar_spatial_relations_config.h"
#include "birrtstar_window.h"
#include "img_load_util.h"
#include "si_viz_util.h"
#include "inbetween_relation_function.h"
#include "avoid_relation_function.h"
#include "sideof_relation_function.h"
#include "spatial_relations_config.h"

using namespace std;
using namespace birrts;

BIRRTstarWindow::BIRRTstarWindow(QWidget *parent) : QMainWindow(parent) {

  mpBIRRTstar = NULL;
  mpViz = new BIRRTstarViz();
  mpBIRRTstarConfig = new BIRRTstarConfig(this);
  mpBIRRTstarConfig->hide();
  mpConfig = new BIRRTstarSpatialRelationsConfig(this);
  mpConfig->hide();

  mpMsgBox = new QMessageBox();
  createActions();
  createMenuBar();

  setCentralWidget(mpViz);

  mpStatusLabel = new QLabel();
  mpStatusLabel->setFixedWidth(240);

  mpStatusProgressBar = new QProgressBar();
  statusBar()->addWidget(mpStatusLabel);
  statusBar()->addWidget(mpStatusProgressBar);
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
  if( mpMsgBox ) {
    delete mpMsgBox;
    mpMsgBox = NULL;
  }
  if( mpConfig ) {
    delete mpConfig;
    mpConfig = NULL;
  }
  if( mpViz ) {
    delete mpViz;
    mpViz = NULL;
  }
}

void BIRRTstarWindow::createMenuBar() {
  mpFileMenu = menuBar()->addMenu("&File");
  mpFileMenu->addAction( mpOpenAction );
  mpFileMenu->addAction( mpSaveAction );
  mpFileMenu->addAction( mpLoadAction );
  mpFileMenu->addAction( mpExportAction );

  mpAddMenu = menuBar()->addMenu("&Add");
  mpAddMenu->addAction( mpAddInbetweenSpatialRelationAction );
  mpAddMenu->addAction( mpAddAvoidInbetweenSpatialRelationAction );
  mpAddSideofRelationMenu = mpAddMenu->addMenu("&Side-of Relation");
  mpAddSideofRelationMenu->addAction( mpAddLeftofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddRightofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddTopofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddBottomofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddAvoidLeftofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddAvoidRightofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddAvoidTopofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddAvoidBottomofSpatialRelationAction );

  mpManageMenu = menuBar()->addMenu("&Manage");
  mpManageMenu->addAction( mpShowConfigAction );
  mpManageMenu->addAction( mpProcessAction );
  mpManageMenu->addAction( mpExecuteAction );

  mpToolMenu = menuBar()->addMenu("&Tool");
  mpToolMenu->addAction(mpLoadObjAction);
  mpToolMenu->addAction(mpSaveScreenAction);

  mpContextMenu = new QMenu();
  setContextMenuPolicy( Qt::CustomContextMenu );
  mpContextMenu->addAction( mpAddStartAction );
  mpContextMenu->addAction( mpAddGoalAction );
}

void BIRRTstarWindow::createActions() {
  mpOpenAction = new QAction("Open", this);
  connect(mpOpenAction, SIGNAL(triggered()), this, SLOT(onOpen()));
  mpSaveAction = new QAction("Save", this);
  connect(mpSaveAction, SIGNAL(triggered()), this, SLOT(onSave()));
  mpLoadAction = new QAction("Load Map", this);
  connect(mpLoadAction, SIGNAL(triggered()), this, SLOT(onLoadMap()));
  mpExportAction = new QAction("Export", this);
  connect(mpExportAction, SIGNAL(triggered()), this, SLOT(onExportPaths()));


  mpAddStartAction = new QAction("Add Start", this);
  mpAddGoalAction = new QAction("Add Goal", this);
  connect( mpAddStartAction, SIGNAL(triggered()), this, SLOT(onAddStart()) );
  connect( mpAddGoalAction, SIGNAL(triggered()), this, SLOT(onAddGoal()) );

  mpAddInbetweenSpatialRelationAction = new QAction("In-between Relation", this);
  connect(mpAddInbetweenSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddInbetweenSpatialRelation()));
  mpAddLeftofSpatialRelationAction = new QAction("Left-of Relation", this);
  connect(mpAddLeftofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddLeftofSpatialRelation()));
  mpAddRightofSpatialRelationAction = new QAction("Right-of Relation", this);
  connect(mpAddRightofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddRightofSpatialRelation()));
  mpAddTopofSpatialRelationAction = new QAction("Top-of Relation", this);
  connect(mpAddTopofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddTopofSpatialRelation()));
  mpAddBottomofSpatialRelationAction = new QAction("Bottom-of Relation", this);
  connect(mpAddBottomofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddBottomofSpatialRelation()));

  mpAddAvoidInbetweenSpatialRelationAction = new QAction("Avoid In-between Relation", this);
  connect(mpAddAvoidInbetweenSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddAvoidInbetweenSpatialRelation()));
  mpAddAvoidLeftofSpatialRelationAction = new QAction("Avoid Left-of Relation", this);
  connect(mpAddAvoidLeftofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddAvoidLeftofSpatialRelation()));
  mpAddAvoidRightofSpatialRelationAction = new QAction("Avoid Right-of Relation", this);
  connect(mpAddAvoidRightofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddAvoidRightofSpatialRelation()));
  mpAddAvoidTopofSpatialRelationAction = new QAction("Avoid Top-of Relation", this);
  connect(mpAddAvoidTopofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddAvoidTopofSpatialRelation()));
  mpAddAvoidBottomofSpatialRelationAction = new QAction("Avoid Bottom-of Relation", this);
  connect(mpAddAvoidBottomofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddAvoidBottomofSpatialRelation()));

  mpShowConfigAction = new QAction("Show", this);
  connect(mpShowConfigAction, SIGNAL(triggered()), this, SLOT(onShowConfig()));
  mpLoadObjAction = new QAction("Config Objective", this);
  connect(mpLoadObjAction, SIGNAL(triggered()), this, SLOT(onLoadObj()));
  mpProcessAction = new QAction("Process", this);
  connect(mpProcessAction, SIGNAL(triggered()), this, SLOT(onProcess()));
  mpExecuteAction = new QAction("Execute", this);
  connect(mpExecuteAction, SIGNAL(triggered()), this, SLOT(onRun()));

  mpSaveScreenAction = new QAction("Save Screen", this);
  connect(mpSaveScreenAction, SIGNAL(triggered()), this, SLOT(onSaveScreen()));

  connect( this, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(contextMenuRequested(QPoint)) );
}

void BIRRTstarWindow::onLoadObj() {
  mpBIRRTstarConfig->exec();
}

bool BIRRTstarWindow::setup_planning(QString filename) {
  if(mpViz) {
    mpViz->m_PPInfo.load_from_file(filename);
    open_map(mpViz->m_PPInfo.m_map_fullpath);
    if(mpViz->get_string_class_mgr()) {
      if( mpViz->m_PPInfo.m_start.x() >= 0 && mpViz->m_PPInfo.m_start.y() ) {
        mpViz->get_string_class_mgr()->m_start_x = mpViz->m_PPInfo.m_start.x();
        mpViz->get_string_class_mgr()->m_start_y = mpViz->m_PPInfo.m_start.y();
      }
      if( mpViz->m_PPInfo.m_goal.x() >= 0 && mpViz->m_PPInfo.m_goal.y() ) {
        mpViz->get_string_class_mgr()->m_goal_x = mpViz->m_PPInfo.m_goal.x();
        mpViz->get_string_class_mgr()->m_goal_y = mpViz->m_PPInfo.m_goal.y();
      }

      for( unsigned int i = 0; i < mpViz->m_PPInfo.m_obs_info_list.size(); i++ ) {
        ObsInfo obs_info = mpViz->m_PPInfo.m_obs_info_list[i];
        h2p::Obstacle* p_obs = mpViz->get_world_map()->find_obstacle( Point2D( obs_info.center_x, obs_info.center_y ) );
        if( p_obs ) {
          p_obs->set_name( obs_info.name );
        }
      }

      cout << "SPATIAL REL NUM " << mpViz->m_PPInfo.mp_spatial_rel_info_list.size() << endl;
      for( unsigned int i = 0; i < mpViz->m_PPInfo.mp_spatial_rel_info_list.size(); i++ ) {
        SpatialRelationInfo* p_spatial_rel_info = mpViz->m_PPInfo.mp_spatial_rel_info_list[i];
        h2p::SpatialRelationFunction* p_spatial_rel_func = BIRRTstarPathPlanningInfo::spatial_relation_info_to_func( p_spatial_rel_info, mpViz->get_string_class_mgr() );
        if( p_spatial_rel_func ) {
          mpViz->get_string_class_mgr()->add_function( p_spatial_rel_func );
        }
      }
      cout << "SPATIAL FUNC NUM " << mpViz->get_string_class_mgr()->mp_functions.size() << endl;
    }
    if(mpBIRRTstarConfig) {
      mpBIRRTstarConfig->updateDisplay();
    }
    if(mpConfig) {
      mpConfig->updateDisplay();
    }
    repaint();
    return true;
  }
  return false;
}

bool BIRRTstarWindow::export_paths() {
  if(mpViz) {
    QString txtFilename = mpViz->m_PPInfo.m_paths_output + ".txt";
    mpViz->m_PPInfo.export_paths(txtFilename);
    for( unsigned int i = 0; i < mpViz->m_PPInfo.mp_found_paths.size(); i++ ) {
      QString drawPathFilename = mpViz->m_PPInfo.m_paths_output + "-" + QString::number(i) + ".png";
      mpViz->draw_path(drawPathFilename, mpViz->m_PPInfo.mp_found_paths[i]);
    }
    return true;
  }
  return false;
}

void BIRRTstarWindow::onRun() {
  cout << "BIRRTstarWindow::onRun" << endl;
  if(mpViz) {
    if (mpViz->m_PPInfo.m_map_width <= 0 || mpViz->m_PPInfo.m_map_height <= 0) {
      QMessageBox msgBox;
      msgBox.setText("Map is not initialized.");
      msgBox.exec();
      return;
    }
    if(mpViz->m_PPInfo.m_start.x()<0 || mpViz->m_PPInfo.m_start.y()<0) {
      QMessageBox msgBox;
      msgBox.setText("Start is not set.");
      msgBox.exec();
      return;
    }
    if(mpViz->m_PPInfo.m_goal.x()<0 || mpViz->m_PPInfo.m_goal.y()<0) {
      QMessageBox msgBox;
      msgBox.setText("Goal is not set.");
      msgBox.exec();
      return;
    }

    plan_path();
    repaint();
  }

}

void BIRRTstarWindow::plan_path() {
  if(mpBIRRTstar) {
    delete mpBIRRTstar;
    mpBIRRTstar = NULL;
  }
  if (mpViz==NULL) {
      return;
  }

  for( std::vector<Path*>::iterator it = mpViz->m_PPInfo.mp_found_paths.begin();
       it != mpViz->m_PPInfo.mp_found_paths.end(); it ++ ) {
    Path * p_path = (*it);
    delete p_path;
    p_path = NULL;
  }
  mpViz->m_PPInfo.mp_found_paths.clear();

  mpViz->m_PPInfo.init_func_param();
  QString msg = "INIT RRTstar ... \n";
  msg += "SegmentLen( " + QString::number(mpViz->m_PPInfo.m_segment_length) + " ) \n";
  msg += "MaxIterationNum( " + QString::number(mpViz->m_PPInfo.m_max_iteration_num) + " ) \n";
  qDebug() << msg;

  h2p::ReferenceFrameSet* pfs = mpViz->get_reference_frame_set();
  h2p::WorldMap* p_map = pfs->get_world_map();
  mpBIRRTstar = new BIRRTstar(p_map->get_width(), p_map->get_height(), mpViz->m_PPInfo.m_segment_length);
  mpBIRRTstar->set_reference_frames( pfs );
  mpBIRRTstar->set_run_type( mpViz->m_PPInfo.m_run_type );
  POS2D start(mpViz->m_PPInfo.m_start.x(), mpViz->m_PPInfo.m_start.y());
  POS2D goal(mpViz->m_PPInfo.m_goal.x(), mpViz->m_PPInfo.m_goal.y());

  StringClassMgr* p_str_cls_mgr = mpViz->get_string_class_mgr();
  p_str_cls_mgr->set_grammar( pfs->get_string_grammar( h2p::toPoint2D(mpViz->m_PPInfo.m_start) , h2p::toPoint2D(mpViz->m_PPInfo.m_goal) ) );
  p_str_cls_mgr->get_string_classes( pfs );
  cout << "string class num " << mpViz->get_string_class_mgr()->mp_string_classes.size() << endl;

  mpBIRRTstar->init(start, goal, mpViz->m_PPInfo.mp_func, mpViz->m_PPInfo.mCostDistribution, p_str_cls_mgr );
  mpViz->m_PPInfo.get_obstacle_info(mpBIRRTstar->get_map_info());
  mpViz->set_tree(mpBIRRTstar);
  mpViz->set_finished_planning( false );

  //mpBIRRTstar->dump_distribution("dist.txt");
  while(mpBIRRTstar->get_current_iteration() <= mpViz->m_PPInfo.m_max_iteration_num) {
    QString msg = "CurrentIteration " + QString::number(mpBIRRTstar->get_current_iteration()) + " ";
    mpBIRRTstar->extend();
    msg += QString::number(mpBIRRTstar->get_string_class_mgr()->mp_string_classes.size());
    qDebug() << msg;

    update_status();
    QApplication::processEvents();
    repaint();
  }
  qDebug() << "START MERGE ";
  mpBIRRTstar->get_string_class_mgr()->merge();
  qDebug() << "END MERGE ";
  //Path* path = mpBIRRTstar->find_path();
  std::vector<Path*> p_paths = mpBIRRTstar->get_paths();
  mpViz->m_PPInfo.load_paths(p_paths);
  mpViz->set_finished_planning( true );
  repaint();
}

void BIRRTstarWindow::onSaveScreen() {
  QString tempFilename = QFileDialog::getSaveFileName(this, tr("Save PNG File"), "./", tr("PNG Files (*.png)"));
  if(mpViz) {
    mpViz->save_current_viz( tempFilename );
  }
}

void BIRRTstarWindow::onAddStart() {
  if( mpViz ) {
    if( mpViz->get_string_class_mgr() ) {
      mpViz->get_string_class_mgr()->m_start_x = mCursorPoint.x();
      mpViz->get_string_class_mgr()->m_start_y = mCursorPoint.y();
      mpViz->m_PPInfo.m_start = mCursorPoint;
      repaint();
    }
  }
}

void BIRRTstarWindow::onAddGoal() {
  if( mpViz ) {
    if( mpViz->get_string_class_mgr() ) {
      mpViz->get_string_class_mgr()->m_goal_x = mCursorPoint.x();
      mpViz->get_string_class_mgr()->m_goal_y = mCursorPoint.y();
      mpViz->m_PPInfo.m_goal = mCursorPoint;
      repaint();
    }
  }
}

void BIRRTstarWindow::contextMenuRequested( QPoint point ) {
  mCursorPoint = point;
  mpContextMenu->popup( mapToGlobal( point ) );
}

void BIRRTstarWindow::onOpen() {
  QString tempFilename = QFileDialog::getOpenFileName(this,
          tr("Open File"), "./", tr("XML Files (*.xml)"));
  if( tempFilename.isEmpty() == false ) {
      //mpViz->load(tempFilename);
      if(setup_planning(tempFilename)) {
        repaint();
      }
  }
}

void BIRRTstarWindow::onSave() {
  QString tempFilename = QFileDialog::getSaveFileName(this,
          tr("Save File"), "./", tr("XML Files (*.xml)"));
  if( tempFilename.isEmpty() == false ) {
      //mpViz->save(tempFilename);
      mpViz->m_PPInfo.save_to_file(tempFilename);
  }
}

void BIRRTstarWindow::onLoadMap() {
  QString tempFilename = QFileDialog::getOpenFileName(this,
            tr("Open File"), "./", tr("Map Files (*.*)"));
  if( tempFilename.isEmpty() == false ) {
    if( mpViz ) {
      QFileInfo fileInfo(tempFilename);
      QString filename(fileInfo.fileName());
      //QPixmap pixmap(tempFilename);
      //mpViz->setPixmap(pixmap);
      mpViz->load_map(tempFilename);
      mpViz->m_PPInfo.m_map_filename = filename;
      mpViz->m_PPInfo.m_map_fullpath = tempFilename;
      mpViz->m_PPInfo.m_map_width = mpViz->get_width();
      mpViz->m_PPInfo.m_map_height = mpViz->get_height();

      mpViz->m_PPInfo.m_obs_info_list.clear();
      for( unsigned int i=0; i < mpViz->get_world_map()->get_obstacles().size(); i++ ) {
        h2p::Obstacle* p_obs = mpViz->get_world_map()->get_obstacles()[i];
        if( p_obs ) {
          ObsInfo obs_info;
          obs_info.name = p_obs->get_name();
          obs_info.center_x = (int)(CGAL::to_double(p_obs->get_centroid().x()));
          obs_info.center_y = (int)(CGAL::to_double(p_obs->get_centroid().y()));
          mpViz->m_PPInfo.m_obs_info_list.push_back(obs_info);
        }
      }
      update_status();
    }
  }
}

void BIRRTstarWindow::onExportPaths() {
   QString tempFilename = QFileDialog::getSaveFileName(this,
              tr("Export Paths to File"), "./", tr("Path Files (*.*)"));
   if( tempFilename.isEmpty() == false ) {
     if( mpViz ) {
       mpViz->m_PPInfo.m_paths_output = tempFilename;
       export_paths();
     }
   }
}

void BIRRTstarWindow::onAddInbetweenSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 2 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Inbetween Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_IN_BETWEEN, selected_obstacles );
    mpViz->get_string_class_mgr()->add_function( p_func );
    SpatialRelationInfo* p_spatial_rel_info = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddAvoidInbetweenSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 2 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Avoid Inbetween Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_child_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_IN_BETWEEN, selected_obstacles );
    h2p::SpatialRelationFunction*  p_avoid_func = mpViz->get_string_class_mgr()->add_avoid_function( p_child_func );
    SpatialRelationInfo* p_spatial_rel_info  = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_avoid_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
  }
  mpViz->clear_selected_obstacles();
  mpViz->clear_selected_obstacles();
  repaint();

  if( mpConfig ) {
    mpConfig->updateDisplay();
  }
}

void BIRRTstarWindow::onAddLeftofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Left-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_LEFT_OF, selected_obstacles );
    mpViz->get_string_class_mgr()->add_function( p_func );
    SpatialRelationInfo* p_spatial_rel_info = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddAvoidLeftofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Left-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_child_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_LEFT_OF, selected_obstacles );
    h2p::SpatialRelationFunction*  p_avoid_func = mpViz->get_string_class_mgr()->add_avoid_function( p_child_func );
    SpatialRelationInfo* p_spatial_rel_info  = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_avoid_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );

  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddRightofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Right-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_RIGHT_OF, selected_obstacles );
    mpViz->get_string_class_mgr()->add_function( p_func );
    SpatialRelationInfo* p_spatial_rel_info = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddAvoidRightofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Left-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_child_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_RIGHT_OF, selected_obstacles );
    h2p::SpatialRelationFunction*  p_avoid_func = mpViz->get_string_class_mgr()->add_avoid_function( p_child_func );
    SpatialRelationInfo* p_spatial_rel_info  = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_avoid_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );

  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddTopofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Top-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_TOP_OF, selected_obstacles );
    mpViz->get_string_class_mgr()->add_function( p_func );
    SpatialRelationInfo* p_spatial_rel_info = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddAvoidTopofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Left-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_child_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_TOP_OF, selected_obstacles );
    h2p::SpatialRelationFunction*  p_avoid_func = mpViz->get_string_class_mgr()->add_avoid_function( p_child_func );
    SpatialRelationInfo* p_spatial_rel_info  = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_avoid_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );

  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddBottomofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Bottom-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_BOTTOM_OF, selected_obstacles );
    mpViz->get_string_class_mgr()->add_function( p_func );
    SpatialRelationInfo* p_spatial_rel_info = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onAddAvoidBottomofSpatialRelation() {
  vector<h2p::Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Left-of Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    }
    return;
  }
  if( mpViz ) {
    h2p::SpatialRelationFunction* p_child_func = mpViz->get_string_class_mgr()->create_function( h2p::SPATIAL_REL_BOTTOM_OF, selected_obstacles );
    h2p::SpatialRelationFunction*  p_avoid_func = mpViz->get_string_class_mgr()->add_avoid_function( p_child_func );
    SpatialRelationInfo* p_spatial_rel_info  = BIRRTstarPathPlanningInfo::spatial_relation_func_to_info( p_avoid_func );
    mpViz->m_PPInfo.mp_spatial_rel_info_list.push_back( p_spatial_rel_info );

  }
  mpViz->clear_selected_obstacles();
  repaint();
}

void BIRRTstarWindow::onShowConfig() {
  if( mpConfig ) {
    mpConfig->updateDisplay();
    mpConfig->exec();
  }
}

void BIRRTstarWindow::onProcess() {
  if( mpViz ) {
    mpViz->process_world();
  }
  repaint();
}

void BIRRTstarWindow::update_status() {
  if(mpViz==NULL) {
    return;
  }
  BIRRTstarViz* p_viz = static_cast<BIRRTstarViz*>(mpViz);
  if(mpStatusProgressBar) {
    if(mpBIRRTstar) {
      mpStatusProgressBar->setMinimum(0);
      mpStatusProgressBar->setMaximum(p_viz->m_PPInfo.m_max_iteration_num);
      mpStatusProgressBar->setValue(mpBIRRTstar->get_current_iteration());
    }
  }
  if(mpStatusLabel) {
    QString status = "";
    if (p_viz->is_finished_planning() == false) {
      h2p::SubRegion* p_rgn = p_viz->get_selected_subregion();
      if(p_rgn) {
        status += QString::fromStdString(p_rgn->get_name());
      }
      status += " || ";
      h2p::LineSubSegment* p_sgm = p_viz->get_selected_line_subsegment();
      if(p_sgm) {
        status += QString::fromStdString(p_sgm->get_name());
      }
    }
    else {
      StringClass* p_cls = p_viz->get_selected_string_class();
      if( p_cls ) {
        status += QString::fromStdString( p_cls->get_name() );
        status += " = ";
        status += QString::number( p_cls->m_cost );
      }
    }
    mpStatusLabel->setText(status);
  }
  repaint();
}

void BIRRTstarWindow::keyPressEvent(QKeyEvent * event) {
  if (event->key() == Qt::Key_Space  ) {
    if(mpViz) {
      if(mpViz->get_mode() == SUBREGION ) {
        mpViz->set_mode( LINE_SUBSEGMENT );
      }
      else {
        mpViz->set_mode( SUBREGION );
      }
      update_status();
      repaint();
    }
  }
  else if (event->key() == Qt::Key_S  ) {
    if(mpViz) {
      if(mpViz->m_show_subsegment == true) {
        mpViz->m_show_subsegment = false;
      }
      else {
        mpViz->m_show_subsegment = true;
      }
      update_status();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_Up ) {
    if(mpViz) {
      if( mpViz->get_mode() == SUBREGION ) {
        mpViz->next_region();
      }
      else {
        mpViz->next_line_subsegment_set();
      }
      update_status();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_Down ) {
    if(mpViz) {
      if( mpViz->get_mode() == SUBREGION ) {
        mpViz->prev_region();
      }
      else {
        mpViz->prev_line_subsegment_set();
      }
      update_status();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_Right ) {
    if(mpViz) {
      if( mpViz->get_mode() == SUBREGION ) {
        mpViz->next_subregion();
      }
      else {
        mpViz->next_line_subsegment();
      }
      update_status();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_Left ) {
    if(mpViz) {
      if( mpViz->get_mode() == SUBREGION ) {
        mpViz->prev_subregion();
      }
      else {
        mpViz->prev_line_subsegment();
      }
      update_status();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_PageDown ) {
    if(mpViz) {
      mpViz->next_string_class();
      update_status();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_PageUp ) {
    if(mpViz) {
      mpViz->prev_string_class();
      update_status();
      repaint();
    }
  }
  else if ( event->key() == Qt::Key_T ) {
    if(mpViz) {
      mpViz->switch_tree_show_type();
      cout << "TREE DISP " <<mpViz->get_tree_show_type() << endl;
    }
    update_status();
    repaint();
  }

}

void BIRRTstarWindow::open_map(QString filename) {
  QPixmap pixmap( filename );
  if(mpViz) {
    mpViz->m_PPInfo.m_map_width = pixmap.width();
    mpViz->m_PPInfo.m_map_height = pixmap.height();
    //mpViz->setPixmap(pixmap);
    cout << "open_map " << filename.toStdString() << endl;
    //mpViz->init_world(filename);
    mpViz->load_map(filename);
  }
}

void BIRRTstarWindow::process() {
  if(mpViz) {
    mpViz->process_world();
  }
}
