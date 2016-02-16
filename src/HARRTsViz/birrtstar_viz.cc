#include <QtGui>

#include "birrtstar_viz.h"

#define START_TREE_COLOR        QColor(160,160,0)
#define START_TREE_COLOR_ALPHA  QColor(160,160,0,100)
#define GOAL_TREE_COLOR         QColor(0,160,160)
#define GOAL_TREE_COLOR_ALPHA   QColor(0,160,160,100)
#define START_COLOR             QColor(255,0,0)
#define GOAL_COLOR              QColor(0,0,255)
#define REFERENCE_FRAME_COLOR   QColor(0,255,0)
#define PATH_COLOR              QColor(255,153,21)
#define DRAWING_LINE_COLOR      QColor(153,76,0)
#define LINE_WIDTH              2

using namespace std;
using namespace h2p;
using namespace birrts;

BIRRTstarViz::BIRRTstarViz( QWidget *parent ) :
    SpatialRelationsViz(parent) {
  mp_tree = NULL;
  m_show_reference_frames = false;
  m_show_regions = false;
  m_finished_planning = false;
  m_reference_frame_index = -1;
  m_found_path_index = -1;
  m_region_index = -1;
  m_subregion_index = -1;
  mp_reference_frames = NULL;
  m_tree_show_type = BOTH_TREES_SHOW;
  m_colors.clear();
}

void BIRRTstarViz::set_tree( BIRRTstar* p_tree ) {
  mp_tree = p_tree;
}

void BIRRTstarViz::paintEvent( QPaintEvent * e ) {
  QLabel::paintEvent(e);

  paint(this);
}

void BIRRTstarViz::paint(QPaintDevice * device) {

  if(mp_tree) {
    if(m_tree_show_type == START_TREE_SHOW || m_tree_show_type == BOTH_TREES_SHOW ) {
      QPainter st_tree_painter(device);
      QPen st_tree_paintpen;
      if(m_finished_planning) {
        st_tree_paintpen.setColor(START_TREE_COLOR_ALPHA);
      }
      else{
        st_tree_paintpen.setColor(START_TREE_COLOR);
      }
      st_tree_paintpen.setWidth(1);
      if(m_finished_planning) {
        st_tree_painter.setOpacity(0.4);
      }
      st_tree_painter.setPen(st_tree_paintpen);
      for( list<BIRRTNode*>::iterator it= mp_tree->get_st_nodes().begin(); it!=mp_tree->get_st_nodes().end();it++ ) {
        BIRRTNode* p_node = (*it);
        if(p_node) {
          if(p_node->mp_parent) {
            st_tree_painter.drawLine(QPoint(p_node->m_pos[0], p_node->m_pos[1]), QPoint(p_node->mp_parent->m_pos[0], p_node->mp_parent->m_pos[1]));
          }
        }
      }
      st_tree_painter.end();
    }
    if(m_tree_show_type == GOAL_TREE_SHOW || m_tree_show_type == BOTH_TREES_SHOW) {
      QPainter gt_tree_painter(device);
      QPen gt_tree_paintpen;
      if(m_finished_planning) {
        gt_tree_paintpen.setColor(GOAL_TREE_COLOR_ALPHA);
      }
      else{
        gt_tree_paintpen.setColor(GOAL_TREE_COLOR);
      }
      gt_tree_paintpen.setWidth(1);
      if(m_finished_planning) {
        gt_tree_painter.setOpacity(0.4);
      }
      gt_tree_painter.setPen(gt_tree_paintpen);
      for( list<BIRRTNode*>::iterator it= mp_tree->get_gt_nodes().begin(); it!=mp_tree->get_gt_nodes().end();it++ ) {
        BIRRTNode* p_node = (*it);
        if(p_node) {
          if(p_node->mp_parent) {
            gt_tree_painter.drawLine(QPoint(p_node->m_pos[0], p_node->m_pos[1]), QPoint(p_node->mp_parent->m_pos[0], p_node->mp_parent->m_pos[1]));
          }
        }
      }
      gt_tree_painter.end();
    }
  } 

  if( mp_viz_string_class ) {

    QPainter pathpainter(device);
    QPen pathpaintpen(QColor(255,140,0));
    pathpaintpen.setWidth(2);
    pathpainter.setPen(pathpaintpen);

    if( mp_viz_string_class ) {
      StringClass* p_str_cls = dynamic_cast<StringClass*>( mp_viz_string_class );
      if( p_str_cls ) {
        Path* p = p_str_cls->mp_path;
        if(p) {
          int point_num = p->m_way_points.size();

          if(point_num > 0) {
            for(int i=0;i<point_num-1;i++) {
              pathpainter.drawLine( QPoint(p->m_way_points[i][0], p->m_way_points[i][1]), QPoint(p->m_way_points[i+1][0], p->m_way_points[i+1][1]) );
            }
          }
        }
      }
    }
    pathpainter.end();
  }

  SpatialRelationsViz::paint(device);
}

bool BIRRTstarViz::draw_path(QString filename) {

  QPixmap pix(m_PPInfo.m_objective_file);

  cout << "DUMP PATH IMG " << pix.width() << " " << pix.height() << endl;

  QFile file(filename);
  if(file.open(QIODevice::WriteOnly)) {
    if(m_PPInfo.mp_found_paths[ m_found_path_index ]) {
      draw_path_on_map(pix);
    }
    pix.save(&file, "PNG");
    return true;
  }
  return false;
}

bool BIRRTstarViz::save_current_viz(QString filename) {
  QPixmap pix(m_PPInfo.m_map_fullpath);
  QFile file(filename);
  if(file.open(QIODevice::WriteOnly)) {
    paint( dynamic_cast<QPaintDevice*>(&pix) );
    pix.save(&file, "PNG");
    return true;
  }
  return false;
}

void BIRRTstarViz::draw_path_on_map(QPixmap& map) {

  Path * p = m_PPInfo.mp_found_paths[ m_found_path_index ];
  QPainter painter(&map);
  QPen paintpen(QColor(255,140,0));
  paintpen.setWidth(2);
  painter.setPen(paintpen);

  int point_num = p->m_way_points.size();

  if(point_num > 0) {
    for(int i=0;i<point_num-1;i++) {
      painter.drawLine( QPoint(p->m_way_points[i][0], p->m_way_points[i][1]), QPoint(p->m_way_points[i+1][0], p->m_way_points[i+1][1]) );
    }
  }

  painter.end();

  QPainter startPainter(&map);
  QPen paintpen1(QColor(255,0,0));
  paintpen.setWidth(10);
  startPainter.setPen(paintpen1);
  startPainter.end();

  startPainter.drawPoint( QPoint(p->m_way_points[0][0], p->m_way_points[0][1]) );
  int lastIdx = p->m_way_points.size() - 1;
  QPainter endPainter(&map);
  QPen paintpen2(QColor(0,0,255));
  paintpen.setWidth(10);
  endPainter.setPen(paintpen2);
  endPainter.drawPoint( QPoint(p->m_way_points[lastIdx][0], p->m_way_points[lastIdx][1]) );
  endPainter.end();
        
}

void BIRRTstarViz::switch_tree_show_type() {

  switch(m_tree_show_type) {
  case NONE_TREE_SHOW:
    m_tree_show_type = START_TREE_SHOW;
    break;
  case START_TREE_SHOW:
    m_tree_show_type = GOAL_TREE_SHOW;
    break;
  case GOAL_TREE_SHOW:
    m_tree_show_type = BOTH_TREES_SHOW;
    break;
  case BOTH_TREES_SHOW:
    m_tree_show_type = NONE_TREE_SHOW;
    break;
  }
}
