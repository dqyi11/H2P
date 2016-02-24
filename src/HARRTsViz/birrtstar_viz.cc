#include <QtGui>

#include "img_load_util.h"
#include "birrtstar_viz.h"
#include "si_viz_util.h"

#define START_TREE_COLOR        QColor(160,160,0)
#define START_TREE_COLOR_ALPHA  QColor(160,160,0,100)
#define GOAL_TREE_COLOR         QColor(0,160,160)
#define GOAL_TREE_COLOR_ALPHA   QColor(0,160,160,100)
#define REFERENCE_FRAME_COLOR   QColor(0,255,0)
#define PATH_COLOR              QColor(255,153,21)
#define DRAWING_LINE_COLOR      QColor(153,76,0)

#define LINE_WIDTH              1
#define SELECTED_LINE_WIDTH     2
#define LINE_WIDTH_HIGHLIGHTED  5
#define POINT_SIZE              4
#define ALPHA_COLOR             QColor(0,0,255)
#define BETA_COLOR              QColor(0,255,0)
#define CENTER_POINT_COLOR      QColor(255,0,0)
#define BK_COLOR                QColor(255,140,0)
#define INTERSECTION_COLOR      QColor(160,160,160)
#define TEXT_COLOR              QColor(0,0,0)
#define OBSTACLE_COLOR          QColor(125,125,125)
#define ASSOCIATED_OBSTACLE_COLOR QColor(255,0,0)
#define SELECTED_OBSTACLE_COLOR QColor(0,255,0)
#define LINE_HIGHLIGHTED_COLOR  QColor(204,204,0)
#define DRAWING_LINE_COLOR      QColor(153,76,0)
#define SUBREGION_COLOR         QColor(204,229,255)
#define START_COLOR             QColor(255,0,0)
#define GOAL_COLOR              QColor(0,0,255)

#define RULE_LINE_WIDTH         4
#define RULE_POS_COLOR          QColor(0,255,0)
#define RULE_NEG_COLOR          QColor(255,0,0)
#define STRING_CLASS_POINT_SIZE 4
#define STRING_CLASS_POINT_COLOR QColor(255,127,80,100)


using namespace std;
using namespace birrts;

BIRRTstarViz::BIRRTstarViz( QWidget *parent ) {
  mp_tree = NULL;
  m_show_reference_frames = false;
  m_show_regions = false;
  m_finished_planning = false;

  mp_reference_frame_set = NULL;
  mp_mgr = NULL;
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
  mp_mgr = new StringClassMgr( get_world_map() );
  return true;
}

void BIRRTstarViz::paintEvent( QPaintEvent * e ) {
  QLabel::paintEvent(e);

  paint(this);
}

void BIRRTstarViz::paint(QPaintDevice * device) {

  if (get_world_map()) {

    QPainter region_painter(device);
    region_painter.setRenderHint(QPainter::Antialiasing);
    QBrush region_brush( SUBREGION_COLOR );
    region_painter.setPen(Qt::NoPen);
    for( std::vector<h2p::SubRegion*>::iterator itr = m_viz_subregions.begin();
         itr != m_viz_subregions.end(); itr++ ) {

      h2p::SubRegion* p_subreg = (*itr);
      if (p_subreg) {
        QPolygon poly;
        for( unsigned int j=0; j < p_subreg->m_points.size(); j++ ) {
          poly << h2p::toQPoint( p_subreg->m_points[j] );
        }
        QPainterPath tmpPath;
        tmpPath.addPolygon(poly);
        region_painter.fillPath(tmpPath, region_brush);
      }
    }
    region_painter.end();

    QPainter line_hl_painter(device);
    QPen line_hl_pen( LINE_HIGHLIGHTED_COLOR );
    line_hl_pen.setWidth( LINE_WIDTH_HIGHLIGHTED );
    line_hl_painter.setPen( line_hl_pen );

    for( std::vector< h2p::LineSubSegment* >::iterator itLSS = m_viz_subsegments.begin();
         itLSS != m_viz_subsegments.end(); itLSS++ ) {
      h2p::LineSubSegment* p_line_subsegment = (*itLSS);
      if( p_line_subsegment ) {
        line_hl_painter.drawLine( h2p::toQPoint( p_line_subsegment->m_subseg.source() ),
                                  h2p::toQPoint( p_line_subsegment->m_subseg.target() ) );
      }
    }
    line_hl_painter.end();

    std::vector<h2p::Obstacle*> obstacles = get_world_map()->get_obstacles();

    QPainter obstacle_painter(device);
    obstacle_painter.setRenderHint(QPainter::Antialiasing);
    QPen obstacle_pen( OBSTACLE_COLOR );
    obstacle_painter.setPen(obstacle_pen);
    for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
         it != obstacles.end(); it++ ) {
      h2p::Obstacle* p_obstacle = (*it);
      if (p_obstacle) {
        QPolygon poly;
        for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
             itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
          Point2D p = (*itP);
          poly << h2p::toQPoint( p );
        }
        obstacle_painter.drawPolygon(poly);
      }
    }
    obstacle_painter.end();

    QPainter hl_obs_painter(device);
    hl_obs_painter.setRenderHint(QPainter::Antialiasing);
    QPen hl_obs_pen( ASSOCIATED_OBSTACLE_COLOR );
    hl_obs_pen.setWidth( SELECTED_LINE_WIDTH );
    hl_obs_painter.setPen(hl_obs_pen);
    h2p::LineSubSegment* p_subseg = get_selected_line_subsegment();
    if( p_subseg ) {
      for( std::vector<h2p::Obstacle*>::iterator it = p_subseg->m_connected_obstacles.begin();
           it != p_subseg->m_connected_obstacles.end(); it++ ) {
        h2p::Obstacle* p_obstacle = (*it);
        if (p_obstacle) {
          QPolygon poly;
          for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
               itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
            Point2D p = (*itP);
            poly << h2p::toQPoint( p );
          }
          hl_obs_painter.drawPolygon(poly);
        }
      }
    }
    hl_obs_painter.end();

    QPainter sl_obs_painter(device);
    sl_obs_painter.setRenderHint(QPainter::Antialiasing);
    QPen sl_obs_pen( SELECTED_OBSTACLE_COLOR );
    sl_obs_pen.setWidth( SELECTED_LINE_WIDTH );
    sl_obs_painter.setPen(sl_obs_pen);
    for( std::vector<h2p::Obstacle*>::iterator it = m_selected_obstacles.begin();
         it != m_selected_obstacles.end(); it++ ) {
      h2p::Obstacle* p_obstacle = (*it);
      if (p_obstacle) {
        QPolygon poly;
        for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
             itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
          Point2D p = (*itP);
          poly << h2p::toQPoint( p );
        }
        sl_obs_painter.drawPolygon(poly);
      }
    }
    sl_obs_painter.end();

    if ( m_show_subsegment == true ) {
      QPainter a_subseg_painter(device);
      QPen a_subseg_pen( ALPHA_COLOR );
      a_subseg_pen.setWidth( LINE_WIDTH );
      a_subseg_painter.setPen( a_subseg_pen );
      for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        h2p::Obstacle* p_obstacle = (*it);
        if ( p_obstacle && p_obstacle->mp_alpha_seg ) {
          //std::cout << "OBS " << p_obstacle->get_index() << " ALPHA:" << p_obstacle->mp_alpha_seg->m_subsegs.size() << std::endl;
          for( std::vector< h2p::LineSubSegment* >::iterator itap = p_obstacle->mp_alpha_seg->m_subsegs.begin();
               itap != p_obstacle->mp_alpha_seg->m_subsegs.end(); itap++ ) {
            h2p::LineSubSegment* p_subseg_a = (*itap);
            a_subseg_painter.drawLine( h2p::toQPoint( p_subseg_a->m_subseg.source() ),
                                       h2p::toQPoint( p_subseg_a->m_subseg.target() ));
          }
        }
      }
      a_subseg_painter.end();

      QPainter b_subseg_painter(device);
      QPen b_subseg_pen( BETA_COLOR );
      b_subseg_pen.setWidth( LINE_WIDTH );
      b_subseg_painter.setPen( b_subseg_pen );
      for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        h2p::Obstacle* p_obstacle = (*it);
        if ( p_obstacle && p_obstacle->mp_beta_seg ) {
          for( std::vector< h2p::LineSubSegment* >::iterator itbp = p_obstacle->mp_beta_seg->m_subsegs.begin();
               itbp != p_obstacle->mp_beta_seg->m_subsegs.end(); itbp++ ) {
            h2p::LineSubSegment* p_subseg_b = (*itbp);
            b_subseg_painter.drawLine( h2p::toQPoint( p_subseg_b->m_subseg.source() ),
                                       h2p::toQPoint( p_subseg_b->m_subseg.target() ));
          }
        }
      }
      b_subseg_painter.end();
    }

    QPainter cp_painter(device);
    QPen cp_pen( CENTER_POINT_COLOR );
    cp_pen.setWidth( POINT_SIZE );
    cp_painter.setPen( cp_pen );
    cp_painter.drawPoint( h2p::toQPoint( get_world_map()->get_central_point() ) );
    cp_painter.end();

    QPainter bk_painter(device);
    QPen bk_pen( BK_COLOR );
    bk_pen.setWidth( POINT_SIZE );
    bk_painter.setPen( bk_pen );
    for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
         it != obstacles.end(); it++ ) {
      h2p::Obstacle* p_obstacle = (*it);
      if ( p_obstacle ) {
        bk_painter.drawPoint( h2p::toQPoint( p_obstacle->m_bk ) );
      }
    }
    bk_painter.end();

    QPainter intsec_painter(device);
    QPen intsec_pen( INTERSECTION_COLOR );
    intsec_pen.setWidth( POINT_SIZE );
    intsec_painter.setPen( intsec_pen );
    for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
         it != obstacles.end(); it++ ) {
      h2p::Obstacle* p_obstacle = (*it);
      if ( p_obstacle ) {
        for( std::vector< h2p::IntersectionPoint >::iterator itap = p_obstacle->m_alpha_intersection_points.begin();
             itap != p_obstacle->m_alpha_intersection_points.end(); itap++ ) {
          h2p::IntersectionPoint alpha_intsec = (*itap);
          intsec_painter.drawPoint( h2p::toQPoint( alpha_intsec.m_point ) );
        }
        for( std::vector< h2p::IntersectionPoint >::iterator itbp = p_obstacle->m_beta_intersection_points.begin();
             itbp != p_obstacle->m_beta_intersection_points.end(); itbp++ ) {
          h2p::IntersectionPoint beta_intsec = (*itbp);
          intsec_painter.drawPoint( h2p::toQPoint( beta_intsec.m_point ) );
        }
      }
    }
    intsec_painter.end();

    QPainter text_painter(device);
    QPen text_pen( TEXT_COLOR );
    text_painter.setPen(text_pen);
    for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
         it != obstacles.end(); it++ ) {
      h2p::Obstacle* p_obstacle = (*it);
      if( p_obstacle ) {
        int c_x = (p_obstacle->m_pgn.bbox().xmax() + p_obstacle->m_pgn.bbox().xmin() )/2;
        int c_y = (p_obstacle->m_pgn.bbox().ymax() + p_obstacle->m_pgn.bbox().ymin() )/2;
        text_painter.drawText( c_x, c_y, QString::fromStdString(p_obstacle->get_name()) );
      }
    }
    text_painter.end();

    QPainter pos_ref_painter(device);
    QPen pos_ref_paintpen( RULE_POS_COLOR );
    pos_ref_paintpen.setWidth( RULE_LINE_WIDTH );
    pos_ref_painter.setPen( pos_ref_paintpen );

    for( vector< h2p::ReferenceFrame* >::iterator it =  m_viz_pos_refs.begin();
         it != m_viz_pos_refs.end(); it++ ) {
      h2p::ReferenceFrame* p_pos_ref = (*it);
      pos_ref_painter.drawLine( h2p::toQPoint( p_pos_ref->m_segment.source() ),
                                h2p::toQPoint( p_pos_ref->m_segment.target() ));
    }
    pos_ref_painter.end();

    QPainter neg_ref_painter(device);
    QPen neg_ref_paintpen( RULE_NEG_COLOR );
    neg_ref_paintpen.setWidth( RULE_LINE_WIDTH );
    neg_ref_painter.setPen( neg_ref_paintpen );

    for( vector< h2p::ReferenceFrame* >::iterator it =  m_viz_neg_refs.begin();
         it != m_viz_neg_refs.end(); it++ ) {
      h2p::ReferenceFrame* p_neg_ref = (*it);
      neg_ref_painter.drawLine( h2p::toQPoint( p_neg_ref->m_segment.source() ),
                                h2p::toQPoint( p_neg_ref->m_segment.target() ));
    }
    neg_ref_painter.end();

    /*
    if(m_show_string_class_reference_path) {
      if( mp_viz_string_class ) {
        QPainter st_cls_painter(device);
        QPen st_cls_paintpen( STRING_CLASS_POINT_COLOR );
        st_cls_paintpen.setWidth( STRING_CLASS_POINT_SIZE );
        st_cls_painter.setPen( st_cls_paintpen );

        StringClassMgr* p_cls_mgr = get_string_class_mgr();
        if(p_cls_mgr) {
          if( mp_viz_string_class->mp_reference_frames.size() > 0 ) {
            st_cls_painter.drawLine( QPoint( p_cls_mgr->m_start_x, p_cls_mgr->m_start_y ),
                                     h2p::toQPoint( mp_viz_string_class->mp_reference_frames[0]->m_mid_point ) );
            for( unsigned int i=0; i < mp_viz_string_class->mp_reference_frames.size()-1; i++ ) {
              h2p::ReferenceFrame* p_curr_rf_str_cls = mp_viz_string_class->mp_reference_frames[i];
              h2p::ReferenceFrame* p_next_rf_str_cls = mp_viz_string_class->mp_reference_frames[i+1];
              if( p_curr_rf_str_cls && p_next_rf_str_cls ) {
                st_cls_painter.drawLine( h2p::toQPoint( p_curr_rf_str_cls->m_mid_point ),
                                         h2p::toQPoint( p_next_rf_str_cls->m_mid_point ) );
              }
            }
            st_cls_painter.drawLine( h2p::toQPoint( mp_viz_string_class->mp_reference_frames.back()->m_mid_point ),
                                     QPoint( p_cls_mgr->m_goal_x, p_cls_mgr->m_goal_y ) );
          }
        }
        st_cls_painter.end();
      }
    } */
  }

  if( mp_tree ) {
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
    QPen pathpaintpen(PATH_COLOR);
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
      cout << p_str_cls->get_name() << endl;
      //cout << "NULL PATH" << endl;
    }

    pathpainter.end();
  }
  else{
    //cout << "NULL STRING CLASS" << endl;
  }

  StringClassMgr* p_cls_mgr = get_string_class_mgr();
  if( p_cls_mgr ) {
    if( p_cls_mgr->m_start_x >= 0 && p_cls_mgr->m_start_y >= 0 ) {
      QPainter st_painter(device);
      QPen st_paintpen( START_COLOR );
      st_paintpen.setWidth( POINT_SIZE );
      st_painter.setPen( st_paintpen );
      st_painter.drawEllipse( QPoint( p_cls_mgr->m_start_x, p_cls_mgr->m_start_y ), 4, 4 );
      st_painter.end();
    }

    if( p_cls_mgr->m_goal_x >= 0 && p_cls_mgr->m_goal_y >= 0 ) {
      QPainter gt_painter(device);
      QPen gt_paintpen( GOAL_COLOR );
      gt_paintpen.setWidth( POINT_SIZE );
      gt_painter.setPen( gt_paintpen );
      gt_painter.drawEllipse( QPoint( p_cls_mgr->m_goal_x, p_cls_mgr->m_goal_y ), 4, 4 );
      gt_painter.end();
    }
  }

}

bool BIRRTstarViz::draw_path(QString filename, Path* p_path) {


  QPixmap pix(m_PPInfo.m_map_width, m_PPInfo.m_map_height);
  if( m_PPInfo.m_objective_file == "" ) {
    pix.fill(QColor(255,255,255));
  }
  else {
    pix = QPixmap( m_PPInfo.m_objective_file );
  }
  cout << "DUMP PATH IMG " << pix.width() << " " << pix.height() << endl;

  QFile file(filename);
  if(file.open(QIODevice::WriteOnly)) {
    if( p_path ) {
      draw_path_on_map(pix, p_path);
    }
    pix.save(&file, "PNG");
    return true;
  }
  return false;
}

bool BIRRTstarViz::save_current_viz(QString filename) {
  QFile file(filename);
  QPixmap emptyPix(m_PPInfo.m_map_width, m_PPInfo.m_map_height);
  emptyPix.fill(QColor("white"));

  if(file.open(QIODevice::WriteOnly)) {
    paint( dynamic_cast<QPaintDevice*>(&emptyPix) );
    emptyPix.save(&file, "PNG");
    return true;
  }
  return false;
}

void BIRRTstarViz::draw_path_on_map(QPixmap& map, Path* p_path) {

  if(p_path) {

    if( get_world_map() ) {
      std::vector<h2p::Obstacle*> obstacles = get_world_map()->get_obstacles();

      QPainter obstacle_painter(&map);
      obstacle_painter.setRenderHint(QPainter::Antialiasing);
      QPen obstacle_pen( OBSTACLE_COLOR );
      obstacle_painter.setPen(obstacle_pen);
      for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        h2p::Obstacle* p_obstacle = (*it);
        if (p_obstacle) {
          QPolygon poly;
          for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
               itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
            Point2D p = (*itP);
            poly << h2p::toQPoint( p );
          }
          obstacle_painter.drawPolygon(poly);
        }
      }
      obstacle_painter.end();

      QPainter text_painter(&map);
      QPen text_pen( TEXT_COLOR );
      text_painter.setPen(text_pen);
      for( std::vector<h2p::Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        h2p::Obstacle* p_obstacle = (*it);
        if( p_obstacle ) {
          int c_x = (p_obstacle->m_pgn.bbox().xmax() + p_obstacle->m_pgn.bbox().xmin() )/2;
          int c_y = (p_obstacle->m_pgn.bbox().ymax() + p_obstacle->m_pgn.bbox().ymin() )/2;
          text_painter.drawText( c_x, c_y, QString::fromStdString(p_obstacle->get_name()) );
        }
      }
      text_painter.end();
    }

    QPainter painter(&map);
    QPen paintpen(QColor(255,140,0));
    paintpen.setWidth(2);
    painter.setPen(paintpen);

    int point_num = p_path->m_way_points.size();

    if(point_num > 0) {
      for(int i=0;i<point_num-1;i++) {
        painter.drawLine( QPoint(p_path->m_way_points[i][0], p_path->m_way_points[i][1]), QPoint(p_path->m_way_points[i+1][0], p_path->m_way_points[i+1][1]) );
      }
    }

    painter.end();

    QPainter startPainter(&map);
    QPen paintpen1(QColor(255,0,0));
    paintpen1.setWidth(POINT_SIZE);
    startPainter.setPen(paintpen1);
    startPainter.drawEllipse( QPoint(p_path->m_way_points[0][0], p_path->m_way_points[0][1]), 4, 4 );
    startPainter.end();

    int lastIdx = p_path->m_way_points.size() - 1;
    QPainter endPainter(&map);
    QPen paintpen2(QColor(0,0,255));
    paintpen2.setWidth(POINT_SIZE);
    endPainter.setPen(paintpen2);
    endPainter.drawEllipse( QPoint(p_path->m_way_points[lastIdx][0], p_path->m_way_points[lastIdx][1]), 4, 4 );
    endPainter.end();

  }
        
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

void BIRRTstarViz::prev_region() {
  if( get_world_map() ) {
    if ( m_region_idx >= 0 ) {
      m_region_idx--;
      m_subregion_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
    else {
      m_region_idx = static_cast<int>(get_world_map()->get_subregion_set().size())-1;
      m_subregion_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
  }
}

void BIRRTstarViz::next_region() {
  if( get_world_map() ) {
    if ( m_region_idx < static_cast<int>(get_world_map()->get_subregion_set().size())-1 ) {
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
    if (NULL == mp_tree) {
        return;
    }
  if ( get_world_map()  ) {
    if ( m_region_idx >= 0 && m_region_idx < static_cast<int>(get_world_map()->get_subregion_set().size()) ) {
      h2p::SubRegionSet* p_subregions = get_world_map()->get_subregion_set() [m_region_idx];
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
  if (NULL == mp_tree) {
      return;
  }
  if ( get_world_map()  ) {
    if ( m_region_idx >= 0 && m_region_idx < static_cast<int>(get_world_map()->get_subregion_set().size()) ) {
         h2p::SubRegionSet* p_subregions = get_world_map()->get_subregion_set() [m_region_idx];
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
  if( get_world_map() ) {
    if ( m_subsegment_set_idx >= 0 ) {
      m_subsegment_set_idx--;
      m_subsegment_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
    else {
      m_subsegment_set_idx = static_cast<int>(get_world_map()->get_linesubsegment_set().size())-1;
      m_subsegment_idx = 0;
      update_viz_subregions();
      update_viz_line_subsegments();
    }
  }
}

void BIRRTstarViz::next_line_subsegment_set() {
  if( get_world_map() ) {
    if ( m_subsegment_set_idx < static_cast<int>(get_world_map()->get_linesubsegment_set().size())-1 ) {
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
  if( get_string_class_mgr() ) {
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
  if( get_string_class_mgr() ) {
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
  if ( get_world_map() ) {
    if ( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < static_cast<int>(get_world_map()->get_linesubsegment_set().size()) ) {
      h2p::LineSubSegmentSet* p_subsegment_set = get_world_map()->get_linesubsegment_set() [m_subsegment_set_idx];
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
  if ( get_world_map() ) {
    if ( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < static_cast<int>(get_world_map()->get_linesubsegment_set().size()) ) {
      h2p::LineSubSegmentSet* p_subsegment_set = get_world_map()->get_linesubsegment_set() [m_subsegment_set_idx];
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
  if ( get_world_map() ) {
    if ( get_world_map()->get_subregion_set().size() > 0 ) {
      if( m_region_idx >= 0 && m_region_idx < get_world_map()->get_subregion_set().size() ) {
        return get_world_map()->get_subregion_set()[ m_region_idx ];
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

  StringClassMgr* p_cls_mgr = get_string_class_mgr();
  if( p_cls_mgr ) {
    //cout << "NUM " << p_cls_mgr->mp_string_classes.size() << endl;
    if( p_cls_mgr->mp_string_classes.size() > 0 ) {
      if( m_string_class_idx >= 0 && m_string_class_idx < p_cls_mgr->mp_string_classes.size() ) {
        return p_cls_mgr->mp_string_classes[m_string_class_idx];
      }
    }
  }
  return NULL;
}

h2p::LineSubSegmentSet* BIRRTstarViz::get_selected_line_subsegment_set() {
  h2p::LineSubSegmentSet* p_subseg_set = NULL;
  if ( get_world_map() ) {
    if ( get_world_map()->get_linesubsegment_set().size() > 0 ) {
      if( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < get_world_map()->get_linesubsegment_set().size() ) {
        return get_world_map()->get_linesubsegment_set()[ m_subsegment_set_idx ];
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
    if( mp_reference_frame_set && mp_reference_frame_set->get_world_map() ) {
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

  if( get_world_map() && get_string_class_mgr()->mp_rule ) {
    get_string_class_mgr()->mp_rule->get_reference_frames( m_viz_pos_refs, m_viz_neg_refs );
    cout << "update Viz Reference Frames: POS " << m_viz_pos_refs.size();
    cout << " NEG " << m_viz_neg_refs.size() << endl;
  }
  repaint();
}

void BIRRTstarViz::process_world( ) {

  mp_reference_frame_set->process( get_string_class_mgr()->get_primary_obstacle() );
  //std::cout << "NUM OF OBS " << conts.size() << std::endl;
  get_string_class_mgr()->mp_rule = get_string_class_mgr()->get_rule( mp_reference_frame_set );
  update_viz_reference_frames();
}

bool BIRRTstarViz::save( QString filename ) {
  if( get_world_map() ) {
    get_world_map()->to_xml(filename.toStdString());
    return true;
  }
  return false;
}

StringClassMgr* BIRRTstarViz::get_string_class_mgr() {
  return mp_mgr;
}

h2p::WorldMap* BIRRTstarViz::get_world_map() {
  if( mp_reference_frame_set ) {
    return mp_reference_frame_set->get_world_map();
  }
  return NULL;
}
