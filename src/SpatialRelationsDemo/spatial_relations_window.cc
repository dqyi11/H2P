#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QKeyEvent>
#include <QStatusBar>
#include "spatial_relations_window.h"
#include "inbetween_relation_function.h"
#include "avoid_relation_function.h"
#include "sideof_relation_function.h"
#include "spatial_relations_config.h"

using namespace std;
using namespace h2p;

SpatialRelationsWindow::SpatialRelationsWindow( SpatialRelationsViz* p_viz, QWidget *parent )
    : QMainWindow( parent ) {
  mpViz = p_viz;
  mpMsgBox = new QMessageBox();
  createActions();
  createMenuBar();
  mpStatusLabel = new QLabel();
  statusBar()->addWidget(mpStatusLabel);
  setCentralWidget(mpViz);

  mpConfig = new SpatialRelationsConfig( this );
  mpConfig->hide();
}

SpatialRelationsWindow::~SpatialRelationsWindow() {
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

void SpatialRelationsWindow::createMenuBar() {
  mpFileMenu = menuBar()->addMenu("&File");
  mpFileMenu->addAction( mpOpenAction );
  mpFileMenu->addAction( mpSaveAction );
  mpFileMenu->addAction( mpLoadAction );

  mpAddMenu = menuBar()->addMenu("&Add");
  mpAddMenu->addAction( mpAddInbetweenSpatialRelationAction );
  mpAddSideofRelationMenu = mpAddMenu->addMenu("&Side-of Relation");
  mpAddSideofRelationMenu->addAction( mpAddLeftofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddRightofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddTopofSpatialRelationAction );
  mpAddSideofRelationMenu->addAction( mpAddBottomofSpatialRelationAction );
  mpAddMenu->addAction( mpAddAvoidSpatialRelationAction );

  mpManageMenu = menuBar()->addMenu("&Manage");
  mpManageMenu->addAction( mpShowConfigAction ); 
  mpManageMenu->addAction( mpProcessAction ); 
  mpManageMenu->addAction( mpExecuteAction ); 

  mpContextMenu = new QMenu();
  setContextMenuPolicy( Qt::CustomContextMenu );
  mpContextMenu->addAction( mpAddStartAction );
  mpContextMenu->addAction( mpAddGoalAction );
}

void SpatialRelationsWindow::createActions() {
  mpOpenAction = new QAction("Open", this);
  connect(mpOpenAction, SIGNAL(triggered()), this, SLOT(onOpen()));
  mpSaveAction = new QAction("Save", this);
  connect(mpSaveAction, SIGNAL(triggered()), this, SLOT(onSave()));
  mpLoadAction = new QAction("Load Map", this);
  connect(mpLoadAction, SIGNAL(triggered()), this, SLOT(onLoadMap()));

  mpAddStartAction = new QAction("Add Start", this);
  mpAddGoalAction = new QAction("Add Goal", this);
  connect( mpAddStartAction, SIGNAL(triggered()), this, SLOT(onAddStart()) );
  connect( mpAddGoalAction, SIGNAL(triggered()), this, SLOT(onAddGoal()) );
 
  mpAddInbetweenSpatialRelationAction = new QAction("In-between Relation", this);
  connect(mpAddInbetweenSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddInbetweenSpatialRelation()));
  mpAddAvoidSpatialRelationAction = new QAction("Avoid Relation", this);
  connect(mpAddAvoidSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddAvoidSpatialRelation()));
  mpAddLeftofSpatialRelationAction = new QAction("Left-of Relation", this);
  connect(mpAddLeftofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddLeftofSpatialRelation()));
  mpAddRightofSpatialRelationAction = new QAction("Right-of Relation", this);
  connect(mpAddRightofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddRightofSpatialRelation()));
  mpAddTopofSpatialRelationAction = new QAction("Top-of Relation", this);
  connect(mpAddTopofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddTopofSpatialRelation()));
  mpAddBottomofSpatialRelationAction = new QAction("Bottom-of Relation", this);
  connect(mpAddBottomofSpatialRelationAction, SIGNAL(triggered()), this, SLOT(onAddBottomofSpatialRelation()));

  mpShowConfigAction = new QAction("Show", this);
  connect(mpShowConfigAction, SIGNAL(triggered()), this, SLOT(onShowConfig()));
  mpProcessAction = new QAction("Process", this);
  connect(mpProcessAction, SIGNAL(triggered()), this, SLOT(onProcess()));
  mpExecuteAction = new QAction("Execute", this);
  connect(mpExecuteAction, SIGNAL(triggered()), this, SLOT(onExecute()));

  connect( this, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(contextMenuRequested(QPoint)) ); 
}

void SpatialRelationsWindow::contextMenuRequested( QPoint point ) {
  mCursorPoint = point;
  mpContextMenu->popup( mapToGlobal( point ) );
}

void SpatialRelationsWindow::onOpen() {
  QString tempFilename = QFileDialog::getOpenFileName(this,
          tr("Save File"), "./", tr("XML Files (*.xml)"));
  if( tempFilename.isEmpty() == false ) {
      mpViz->load(tempFilename);
  }
}

void SpatialRelationsWindow::onSave() {
  QString tempFilename = QFileDialog::getSaveFileName(this,
          tr("Save File"), "./", tr("XML Files (*.xml)"));
  if( tempFilename.isEmpty() == false ) {
      mpViz->save(tempFilename);
  }
}

void SpatialRelationsWindow::onLoadMap() {
  QString tempFilename = QFileDialog::getOpenFileName(this,
            tr("Open File"), "./", tr("Map Files (*.*)"));
  if( tempFilename.isEmpty() == false ) {
    mpViz->load_map(tempFilename);
    updateStatusBar();
  }
}

void SpatialRelationsWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Space  ) {
    if(mpViz) {
      if(mpViz->get_mode() == SUBREGION ) {
        mpViz->set_mode( LINE_SUBSEGMENT );
      }
      else {
        mpViz->set_mode( SUBREGION );
      }
      updateStatusBar();
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
      updateStatusBar();
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
      updateStatusBar();
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
      updateStatusBar();
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
      updateStatusBar();
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
      updateStatusBar();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_PageDown ) {
    if(mpViz) {
      mpViz->next_string_class();
      repaint();
    }
  }
  else if(event->key() == Qt::Key_PageUp ) {
    if(mpViz) {
      mpViz->prev_string_class();
      repaint();
    }
  }

}

void SpatialRelationsWindow::updateStatusBar() {

  if(mpStatusLabel) {
    QString status = "";
    if( mpViz->get_selected_region() ) {
      status += "Region (" + QString::number(mpViz->get_region_idx()) + ")";
      if ( mpViz->get_selected_subregion() ) {
        status += "- (" + QString::number(mpViz->get_subregion_idx()) + ")";
        status += " = ";
        for( unsigned int i = 0; i < mpViz->get_selected_subregion()->m_neighbors.size(); i ++ ) {
          LineSubSegment* p_line_subseg = mpViz->get_selected_subregion()->m_neighbors[i];
          status += " [ " + QString::fromStdString(p_line_subseg->get_name())  + " ] ";
        } 
      }
      else {
        status += " = ";
        if ( mpViz->get_selected_region() ) {
          status += " [ " + QString::fromStdString(mpViz->get_selected_region()->mp_line_segments_a->get_name())  + " ] ";
          status += " [ " + QString::fromStdString(mpViz->get_selected_region()->mp_line_segments_b->get_name())  + " ] ";
        }
      }
    }
    mpStatusLabel->setText(status);
  }
  repaint();
}

void SpatialRelationsWindow::onAddStart() {
  if( mpViz ) {
    if( mpViz->get_spatial_relation_mgr() ) {
      mpViz->get_spatial_relation_mgr()->m_start_x = mCursorPoint.x();
      mpViz->get_spatial_relation_mgr()->m_start_y = mCursorPoint.y();
      repaint();
    }
  }
}

void SpatialRelationsWindow::onAddGoal() {
  if( mpViz ) {
    if( mpViz->get_spatial_relation_mgr() ) {
      mpViz->get_spatial_relation_mgr()->m_goal_x = mCursorPoint.x();
      mpViz->get_spatial_relation_mgr()->m_goal_y = mCursorPoint.y();
      repaint();
    }
  }
}
  
void SpatialRelationsWindow::onAddInbetweenSpatialRelation() {
  vector<Obstacle*> selected_obstacles;
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
  InBetweenRelationFunction* p_func = new InBetweenRelationFunction();
  for( unsigned int i=0; i < selected_obstacles.size(); i++ ) {
    Obstacle* p_obs = selected_obstacles[i];
    p_func->mp_obstacles.push_back( p_obs );
  }
  if( mpViz ) {
    mpViz->get_spatial_relation_mgr()->mp_functions.push_back( p_func );
  } 
  mpViz->clear_selected_obstacles();
  repaint(); 
}

void SpatialRelationsWindow::onAddAvoidSpatialRelation() {
  vector<Obstacle*> selected_obstacles;
  if( mpViz) {
    selected_obstacles = mpViz->get_selected_obstacles();
  }
  if( selected_obstacles.size() != 1 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Add Avoid Spatial : Number of obstacles mismatch " );
      mpMsgBox->show();
    } 
    return;
  }
  AvoidRelationFunction* p_func = new AvoidRelationFunction();
  p_func->mp_obstacle = selected_obstacles[0];
  if( mpViz ) {
    mpViz->get_spatial_relation_mgr()->mp_functions.push_back( p_func );
  } 
  mpViz->clear_selected_obstacles();
  repaint();
  
  if( mpConfig ) {
    mpConfig->updateDisplay();
  }
}

void SpatialRelationsWindow::onAddLeftofSpatialRelation() {
  vector<Obstacle*> selected_obstacles;
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
  SideOfRelationFunction* p_func = new SideOfRelationFunction( SIDE_TYPE_LEFT );
  p_func->mp_obstacle = selected_obstacles[0];
  if( mpViz ) {
    mpViz->get_spatial_relation_mgr()->mp_functions.push_back( p_func );
  } 
  mpViz->clear_selected_obstacles();
  repaint(); 
}

void SpatialRelationsWindow::onAddRightofSpatialRelation() {
  vector<Obstacle*> selected_obstacles;
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
  SideOfRelationFunction* p_func = new SideOfRelationFunction( SIDE_TYPE_RIGHT );
  p_func->mp_obstacle = selected_obstacles[0];
  if( mpViz ) {
    mpViz->get_spatial_relation_mgr()->mp_functions.push_back( p_func );
  } 
  mpViz->clear_selected_obstacles();
  repaint(); 
}

void SpatialRelationsWindow::onAddTopofSpatialRelation() {
  vector<Obstacle*> selected_obstacles;
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
  SideOfRelationFunction* p_func = new SideOfRelationFunction( SIDE_TYPE_TOP );
  p_func->mp_obstacle = selected_obstacles[0];
  if( mpViz ) {
    mpViz->get_spatial_relation_mgr()->mp_functions.push_back( p_func );
  } 
  mpViz->clear_selected_obstacles();
  repaint(); 
}

void SpatialRelationsWindow::onAddBottomofSpatialRelation() {
  vector<Obstacle*> selected_obstacles;
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
  SideOfRelationFunction* p_func = new SideOfRelationFunction( SIDE_TYPE_BOTTOM );
  p_func->mp_obstacle = selected_obstacles[0];
  if( mpViz ) {
    mpViz->get_spatial_relation_mgr()->mp_functions.push_back( p_func );
  }
  mpViz->clear_selected_obstacles();
  repaint(); 
}

void SpatialRelationsWindow::onShowConfig() {
  if( mpConfig ) {
    mpConfig->updateDisplay();
    mpConfig->exec();
  }
}

void SpatialRelationsWindow::onProcess() {
  if( mpViz ) {
    mpViz->process_world();
  }
  repaint();
}

void SpatialRelationsWindow::onExecute() {
  cout << "SpatialRelationsWindow::onExecute" << endl;
  if( mpViz->get_spatial_relation_mgr()->m_start_x < 0 ||
      mpViz->get_spatial_relation_mgr()->m_start_y < 0 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Start position not set" );
      mpMsgBox->show();
    }
  } 

  if( mpViz->get_spatial_relation_mgr()->m_goal_x < 0 ||
      mpViz->get_spatial_relation_mgr()->m_goal_y < 0 ) {
    if( mpMsgBox ) {
      mpMsgBox->setText( "Goal position not set" );
      mpMsgBox->show();
    } 
  }

  mpViz->get_spatial_relation_mgr()->get_string_classes( mpViz->get_reference_frame_set() ); 
  repaint();
}
