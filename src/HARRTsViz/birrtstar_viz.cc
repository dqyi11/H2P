#include <QtGui>

#include "img_load_util.h"
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
using namespace birrts;

BIRRTstarViz::BIRRTstarViz( QWidget *parent ) {
  mp_tree = NULL;
  m_show_reference_frames = false;
  m_show_regions = false;
  m_finished_planning = false;

  mp_reference_frames = NULL;
  m_tree_show_type = BOTH_TREES_SHOW;

}

void BIRRTstarViz::set_tree( BIRRTstar* p_tree ) {
  mp_tree = p_tree;
}

bool BIRRTstarViz::load( QString filename ) {
  return false;
}

bool BIRRTstarViz::load_map( QString filename ) {

  QPixmap pix(filename);
  if( pix.isNull() == true ) {
    return false;
  }
  QPixmap emptyPix(pix.width(), pix.height());
  emptyPix.fill(QColor("white"));
  std::cout << " EMPTY PIX " << emptyPix.width() << " * " << emptyPix.height() << std::endl;
  //setPixmap(pix);
  setPixmap(emptyPix);
  init_world(filename);
  return true;
}

bool BIRRTstarViz::init_world(QString filename) {

  std::vector< std::vector<Point2D> > conts;
  //int map_width = 0, map_height = 0;
  if (mp_reference_frame_set) {
    delete mp_reference_frame_set;
    mp_reference_frame_set = NULL;
  }

  load_map_info( filename.toStdString(), m_world_width, m_world_height, conts );
  //std::cout << "CREATE WORLD " << map_width << " * " << map_height << std::endl;
  mp_reference_frame_set = new h2p::ReferenceFrameSet();
  mp_reference_frame_set->init( m_world_width, m_world_height, conts );
  mp_mgr = new StringClassMgr( mp_reference_frame_set->get_world_map() );
  return true;
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

  StringClass* p_str_cls = get_selected_string_class();
  if( p_str_cls ) {

    QPainter pathpainter(device);
    QPen pathpaintpen(QColor(255,140,0));
    pathpaintpen.setWidth(2);
    pathpainter.setPen(pathpaintpen);

    Path* p = p_str_cls->mp_path;
    if(p) {
      int point_num = p->m_way_points.size();

      if(point_num > 0) {
        for(int i=0;i<point_num-1;i++) {
          pathpainter.drawLine( QPoint(p->m_way_points[i][0], p->m_way_points[i][1]), QPoint(p->m_way_points[i+1][0], p->m_way_points[i+1][1]) );
        }
      }
    }
    else {
      cout << "NULL PATH" << endl;
    }

    pathpainter.end();
  }
  else{
    cout << "NULL STRING CLASS" << endl;
  }

}

bool BIRRTstarViz::draw_path(QString filename) {

  QPixmap pix(m_PPInfo.m_objective_file);

  cout << "DUMP PATH IMG " << pix.width() << " " << pix.height() << endl;

  QFile file(filename);
  if(file.open(QIODevice::WriteOnly)) {
    if( get_selected_string_class() ) {
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

  Path * p = get_selected_string_class()->mp_path;
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

StringClassMgr* BIRRTstarViz::get_string_class_mgr() {
  //h2p::SpatialRelationMgr* p_spatial_relation_mgr = get_spatial_relation_mgr();
  return static_cast<StringClassMgr*>(mp_mgr);
}

void BIRRTstarViz::prev_region() {
  if( mp_mgr->mp_worldmap ) {
    if ( m_region_idx >= 0 ) {
      m_region_idx--;
      m_subregion_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
    else {
      m_region_idx = static_cast<int>(mp_mgr->mp_worldmap->get_subregion_set().size())-1;
      m_subregion_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
  }
}

void BIRRTstarViz::next_region() {
  if( mp_mgr->mp_worldmap ) {
    if ( m_region_idx < static_cast<int>(mp_mgr->mp_worldmap->get_subregion_set().size())-1 ) {
      m_region_idx++;
      m_subregion_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
    else {
      m_region_idx = -1;
      m_subregion_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
  }
}

void BIRRTstarViz::prev_subregion() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_region_idx >= 0 && m_region_idx < static_cast<int>(mp_mgr->mp_worldmap->get_subregion_set().size()) ) {
      h2p::SubRegionSet* p_subregions = mp_mgr->mp_worldmap->get_subregion_set() [m_region_idx];
      int sub_num = static_cast<int>( p_subregions->m_subregions.size() );
      if ( m_subregion_idx > 0) {
        m_subregion_idx --;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
      else{
        m_subregion_idx = sub_num - 1;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
    }
  }
}

void BIRRTstarViz::next_subregion() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_region_idx >= 0 && m_region_idx < static_cast<int>(mp_mgr->mp_worldmap->get_subregion_set().size()) ) {
         h2p::SubRegionSet* p_subregions = mp_mgr->mp_worldmap->get_subregion_set() [m_region_idx];
      int sub_num = static_cast<int>( p_subregions->m_subregions.size() );
      if ( m_subregion_idx < sub_num-1) {
        m_subregion_idx ++;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
      else{
        m_subregion_idx = 0;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
    }
  }
}

void BIRRTstarViz::prev_line_subsegment_set() {
  if( mp_mgr->mp_worldmap ) {
    if ( m_subsegment_set_idx >= 0 ) {
      m_subsegment_set_idx--;
      m_subsegment_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
    else {
      m_subsegment_set_idx = static_cast<int>(mp_mgr->mp_worldmap->get_linesubsegment_set().size())-1;
      m_subsegment_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
  }
}

void BIRRTstarViz::next_line_subsegment_set() {
  if( mp_mgr->mp_worldmap ) {
    if ( m_subsegment_set_idx < static_cast<int>(mp_mgr->mp_worldmap->get_linesubsegment_set().size())-1 ) {
      m_subsegment_set_idx++;
      m_subsegment_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
    else {
      m_subsegment_set_idx = -1;
      m_subsegment_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
  }
}

void BIRRTstarViz::prev_string_class() {
  if( mp_mgr ) {
    StringClassMgr* p_cls_mgr = get_string_class_mgr();
    if( p_cls_mgr ) {
      if( m_string_class_idx >= 0 ) {
        m_string_class_idx --;
        update_viz_string_class();
      }
      else {
        m_string_class_idx = p_cls_mgr->mp_string_classes.size()-1;
        update_viz_string_class();
      }
    }
  }
}

void BIRRTstarViz::next_string_class() {
  if( mp_mgr ) {
    StringClassMgr* p_cls_mgr = get_string_class_mgr();
    if( p_cls_mgr ) {
      if( m_string_class_idx < p_cls_mgr->mp_string_classes.size()-1 ) {
        m_string_class_idx ++;
        update_viz_string_class();
      }
      else {
        m_string_class_idx = -1;
        update_viz_string_class();
      }
    }
  }
}

void BIRRTstarViz::prev_line_subsegment() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < static_cast<int>(mp_mgr->mp_worldmap->get_linesubsegment_set().size()) ) {
      h2p::LineSubSegmentSet* p_subsegment_set = mp_mgr->mp_worldmap->get_linesubsegment_set() [m_subsegment_set_idx];
      int sub_num = static_cast<int>( p_subsegment_set->m_subsegs.size() );
      if ( m_subsegment_idx > 0) {
        m_subsegment_idx --;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
      else{
        m_subsegment_idx = sub_num - 1;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
    }
  }
}

void BIRRTstarViz::next_line_subsegment() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < static_cast<int>(mp_mgr->mp_worldmap->get_linesubsegment_set().size()) ) {
      h2p::LineSubSegmentSet* p_subsegment_set = mp_mgr->mp_worldmap->get_linesubsegment_set() [m_subsegment_set_idx];
      int sub_num = static_cast<int>( p_subsegment_set->m_subsegs.size() );
      if ( m_subsegment_idx < sub_num-1) {
        m_subsegment_idx ++;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
      else{
        m_subsegment_idx = 0;
        update_viz_subregions();
        update_viz_line_subsegments();
      }
    }
  }
}

h2p::SubRegionSet* BIRRTstarViz::get_selected_region() {
  h2p::SubRegionSet* p_region = NULL;
  if ( mp_mgr->get_world_map() ) {
    if ( mp_mgr->get_world_map()->get_subregion_set().size() > 0 ) {
      if( m_region_idx >= 0 && m_region_idx < mp_mgr->get_world_map()->get_subregion_set().size() ) {
        return mp_mgr->get_world_map()->get_subregion_set()[ m_region_idx ];
      }
    }
  }
  return p_region;
}


h2p::SubRegion* BIRRTstarViz::get_selected_subregion() {
  h2p::SubRegion* p_subregion = NULL;
  h2p::SubRegionSet* p_region = get_selected_region();
  if( p_region ) {
    if( p_region->m_subregions.size() > 0 ) {
      if( m_subregion_idx >= 0 && m_subregion_idx < p_region->m_subregions.size() ) {
        return p_region->m_subregions[m_subregion_idx];
      }
    }
  }
  return p_subregion;
}

StringClass* BIRRTstarViz::get_selected_string_class() {
  if( mp_mgr ) {
    StringClassMgr* p_cls_mgr = get_string_class_mgr();
    if( p_cls_mgr ) {
        if( p_cls_mgr->mp_string_classes.size() > 0 ) {
          if( m_string_class_idx >= 0 && m_string_class_idx < p_cls_mgr->mp_string_classes.size() ) {
            return p_cls_mgr->mp_string_classes[m_string_class_idx];
          }
        }
    }
  }
  return NULL;
}

h2p::LineSubSegmentSet* BIRRTstarViz::get_selected_line_subsegment_set() {
  h2p::LineSubSegmentSet* p_subseg_set = NULL;
  if ( mp_mgr->get_world_map() ) {
    if ( mp_mgr->get_world_map()->get_linesubsegment_set().size() > 0 ) {
      if( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < mp_mgr->get_world_map()->get_linesubsegment_set().size() ) {
        return mp_mgr->get_world_map()->get_linesubsegment_set()[ m_subsegment_set_idx ];
      }
    }
  }
  return p_subseg_set;
}

h2p::LineSubSegment* BIRRTstarViz::get_selected_line_subsegment() {
  h2p::LineSubSegment* p_subseg= NULL;
  h2p::LineSubSegmentSet* p_subseg_set = get_selected_line_subsegment_set();
  if( p_subseg_set ) {
    if( p_subseg_set->m_subsegs.size() > 0 ) {
      if( m_subsegment_idx >= 0 && m_subsegment_idx < p_subseg_set->m_subsegs.size() ) {
        return p_subseg_set->m_subsegs[m_subsegment_idx];
      }
    }
  }
  return p_subseg;
}

void BIRRTstarViz::update_viz_subregions() {
  m_viz_subregions.clear();
  if( SUBREGION == m_mode ) {
    h2p::SubRegionSet* p_region = get_selected_region();
    if( p_region ) {
      h2p::SubRegion* p_subregion = get_selected_subregion();
      if (p_subregion) {
        m_viz_subregions.push_back( p_subregion );
      }
      else {
        for( unsigned int i=0; i < p_region->m_subregions.size(); i++ ) {
          h2p::SubRegion* p_subregion = p_region->m_subregions[i];
          m_viz_subregions.push_back( p_subregion );
        }
      }
    }
  }
  else if( LINE_SUBSEGMENT == m_mode ) {
    h2p::LineSubSegmentSet* p_subseg_set = get_selected_line_subsegment_set();
    if( p_subseg_set ) {
      h2p::LineSubSegment* p_subseg = get_selected_line_subsegment();
      if( p_subseg ) {
        for( unsigned int i=0; i < p_subseg->m_neighbors.size(); i++ ) {
          h2p::SubRegion* p_subregion = p_subseg->m_neighbors[i];
          m_viz_subregions.push_back( p_subregion );
        }
      }
    }
  }
}

void BIRRTstarViz::update_viz_line_subsegments() {
  m_viz_subsegments.clear();
  if( SUBREGION == m_mode ) {
    h2p::SubRegionSet* p_region = get_selected_region();
    if( p_region ) {
      h2p::SubRegion* p_subregion = get_selected_subregion();
      if (p_subregion) {
        for( unsigned int i=0; i < p_subregion->m_neighbors.size(); i++ ) {
          h2p::LineSubSegment* p_subseg = p_subregion->m_neighbors[i];
          if( p_subseg ) {
            m_viz_subsegments.push_back( p_subseg );
          }
        }
      }
      else {
        if( p_region->mp_line_segments_a ){
          for( unsigned int i=0; i < p_region->mp_line_segments_a->m_subsegs.size(); i++ ) {
            h2p::LineSubSegment* p_subseg = p_region->mp_line_segments_b->m_subsegs[i];
            if( p_subseg ) {
              m_viz_subsegments.push_back( p_subseg );
            }
          }
        }

        if( p_region->mp_line_segments_b ){
          for( unsigned int i=0; i < p_region->mp_line_segments_b->m_subsegs.size(); i++ ) {
            h2p::LineSubSegment* p_subseg = p_region->mp_line_segments_b->m_subsegs[i];
            if( p_subseg ) {
              m_viz_subsegments.push_back( p_subseg );
            }
          }
        }

      }
    }
  }
  else if( LINE_SUBSEGMENT == m_mode ) {
    h2p::LineSubSegmentSet* p_subseg_set = get_selected_line_subsegment_set();
    if( p_subseg_set ) {
      h2p::LineSubSegment* p_subseg = get_selected_line_subsegment();
      if( p_subseg ) {
        m_viz_subsegments.push_back( p_subseg );
      }
      else {
        for( unsigned int i=0; i < p_subseg_set->m_subsegs.size(); i++ ) {
          h2p::LineSubSegment* p_subseg = p_subseg_set->m_subsegs[i];
          if( p_subseg ){
            m_viz_subsegments.push_back( p_subseg );
          }
        }
      }
    }
  }

}

void BIRRTstarViz::update_viz_string_class() {
  mp_viz_string_class = get_selected_string_class();
}

void BIRRTstarViz::set_mode( BIRRTstarVizMode mode ) {
  m_mode = mode;
  m_region_idx = -1;
  m_subregion_idx = -1;
  m_subsegment_set_idx = -1;
  m_subsegment_idx = -1;
  update_viz_subregions();
  update_viz_line_subsegments();
}

void BIRRTstarViz::mousePressEvent( QMouseEvent * event ) {
  if( event->button() == Qt::LeftButton ) {
    Point2D clicked_point( event->x(), event->y() );
    h2p::Obstacle* p_selected_obstacle = mp_reference_frame_set->get_world_map()->find_obstacle( clicked_point );
    if( p_selected_obstacle ) {
      if( is_selected_obstacle( p_selected_obstacle ) ) {
        unselect_obstacle( p_selected_obstacle );
      }
      else{
        m_selected_obstacles.push_back( p_selected_obstacle );
      }
      repaint();
    }
  }
}

bool BIRRTstarViz::is_selected_obstacle( h2p::Obstacle* p_obstacle ) {
  for(unsigned int i=0; i < m_selected_obstacles.size(); i++ ) {
        h2p::Obstacle* p_current_obstacle = m_selected_obstacles[i];
    if( p_current_obstacle ) {
      if( p_current_obstacle == p_obstacle ) {
        return true;
      }
    }
  }
  return false;

}

bool BIRRTstarViz::unselect_obstacle( h2p::Obstacle* p_obstacle ) {
  for( std::vector< h2p::Obstacle* >::iterator it = m_selected_obstacles.begin();
       it != m_selected_obstacles.end(); it ++ ) {
    h2p::Obstacle* p_current_obstacle = (*it);
    if( p_current_obstacle ) {
      if( p_current_obstacle == p_obstacle ) {
        m_selected_obstacles.erase(it);
        return true;
      }
    }
  }
  return false;
}


void BIRRTstarViz::update_viz_reference_frames() {

  if( mp_mgr && mp_mgr->mp_rule ) {
    mp_mgr->mp_rule->get_reference_frames( m_viz_pos_refs, m_viz_neg_refs );
    cout << "update Viz Reference Frames: POS " << m_viz_pos_refs.size();
    cout << " NEG " << m_viz_neg_refs.size() << endl;
  }
  repaint();
}

void BIRRTstarViz::process_world( ) {

  mp_reference_frame_set->process( mp_mgr->get_primary_obstacle() );
  //std::cout << "NUM OF OBS " << conts.size() << std::endl;
  mp_mgr->mp_rule = mp_mgr->get_rule( mp_reference_frame_set );
  update_viz_reference_frames();
}

bool BIRRTstarViz::save( QString filename ) {
  if( mp_mgr->mp_worldmap ) {
    mp_mgr->mp_worldmap->to_xml(filename.toStdString(
));
    return true;
  }
  return false;
}
