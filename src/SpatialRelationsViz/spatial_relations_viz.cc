#include <cstdlib>
#include <QPainter>
#include <QMouseEvent>
#include "spatial_relations_viz.h"
#include "si_viz_util.h"
#include "img_load_util.h"

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
#define STRING_CLASS_POINT_COLOR QColor(0,255,255)

using namespace std;
using namespace h2p;

SpatialRelationsViz::SpatialRelationsViz(QWidget *parent) :
    QLabel(parent) {

  mp_mgr = NULL;
  mp_reference_frame_set = NULL;
  m_world_width = 0;
  m_world_height = 0;
  m_region_idx = -1;
  m_subregion_idx = -1;
  m_subsegment_set_idx = -1;
  m_subsegment_idx = -1;
  m_string_class_idx = -1;
  m_show_subsegment = true;
  m_viz_subregions.clear();
  m_viz_subsegments.clear();
  mp_viz_string_class = NULL;
  m_mode = SUBREGION;
}

bool SpatialRelationsViz::load_map( QString filename ) {

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

bool SpatialRelationsViz::init_world(QString filename) {

  std::vector< std::vector<Point2D> > conts;
  int map_width = 0, map_height = 0;
  if (mp_reference_frame_set) {
    delete mp_reference_frame_set;
    mp_reference_frame_set = NULL;
  }

  load_map_info( filename.toStdString(), map_width, map_height, conts );   
  //std::cout << "CREATE WORLD " << map_width << " * " << map_height << std::endl;
  mp_reference_frame_set = new ReferenceFrameSet();
  mp_reference_frame_set->init( map_width, map_height, conts );
  mp_mgr = new SpatialRelationMgr( mp_reference_frame_set->get_world_map() );
  return true;
}

void SpatialRelationsViz::process_world( ) {
   
  mp_reference_frame_set->process( mp_mgr->get_primary_obstacle() ); 
  //std::cout << "NUM OF OBS " << conts.size() << std::endl;
  mp_mgr->mp_rule = mp_mgr->get_rule( mp_reference_frame_set );
  update_viz_reference_frames();
}

void SpatialRelationsViz::paintEvent(QPaintEvent * e) {
  QLabel::paintEvent(e);
  paint( this );

}

void SpatialRelationsViz::paint(QPaintDevice * device ) {

  if (mp_mgr) {
    if (mp_mgr->mp_worldmap) {

      QPainter region_painter(device);
      region_painter.setRenderHint(QPainter::Antialiasing);
      QBrush region_brush( SUBREGION_COLOR );
      region_painter.setPen(Qt::NoPen);
      for( std::vector<SubRegion*>::iterator itr = m_viz_subregions.begin();
           itr != m_viz_subregions.end(); itr++ ) {  
  
        SubRegion* p_subreg = (*itr);
        if (p_subreg) {
          QPolygon poly;
          for( unsigned int j=0; j < p_subreg->m_points.size(); j++ ) {
            poly << toQPoint( p_subreg->m_points[j] );
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
 
      for( std::vector< LineSubSegment* >::iterator itLSS = m_viz_subsegments.begin();
           itLSS != m_viz_subsegments.end(); itLSS++ ) {
        LineSubSegment* p_line_subsegment = (*itLSS);
        if( p_line_subsegment ) {
          line_hl_painter.drawLine( toQPoint( p_line_subsegment->m_subseg.source() ),
                                    toQPoint( p_line_subsegment->m_subseg.target() ) );
        }
      }
      line_hl_painter.end();

      std::vector<Obstacle*> obstacles =  mp_mgr->mp_worldmap->get_obstacles();
  
      QPainter obstacle_painter(device);
      obstacle_painter.setRenderHint(QPainter::Antialiasing);
      QPen obstacle_pen( OBSTACLE_COLOR );
      obstacle_painter.setPen(obstacle_pen);
      for( std::vector<Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        Obstacle* p_obstacle = (*it);
        if (p_obstacle) {
          QPolygon poly;
          for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
               itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
            Point2D p = (*itP);
            poly << toQPoint( p );
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
      LineSubSegment* p_subseg = get_selected_line_subsegment();
      if( p_subseg ) {
        for( std::vector<Obstacle*>::iterator it = p_subseg->m_connected_obstacles.begin();
             it != p_subseg->m_connected_obstacles.end(); it++ ) {
          Obstacle* p_obstacle = (*it);
          if (p_obstacle) {
            QPolygon poly;
            for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
                 itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
              Point2D p = (*itP);
              poly << toQPoint( p );
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
      for( std::vector<Obstacle*>::iterator it = m_selected_obstacles.begin();
           it != m_selected_obstacles.end(); it++ ) {
        Obstacle* p_obstacle = (*it);
        if (p_obstacle) {
          QPolygon poly;
          for( Polygon2D::Vertex_iterator itP=p_obstacle->m_pgn.vertices_begin();
               itP != p_obstacle->m_pgn.vertices_end(); itP++ ) {
            Point2D p = (*itP);
            poly << toQPoint( p );
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
        for( std::vector<Obstacle*>::iterator it = obstacles.begin();
             it != obstacles.end(); it++ ) {
          Obstacle* p_obstacle = (*it);
          if ( p_obstacle && p_obstacle->mp_alpha_seg ) {
            //std::cout << "OBS " << p_obstacle->get_index() << " ALPHA:" << p_obstacle->mp_alpha_seg->m_subsegs.size() << std::endl;
            for( std::vector< LineSubSegment* >::iterator itap = p_obstacle->mp_alpha_seg->m_subsegs.begin();
                 itap != p_obstacle->mp_alpha_seg->m_subsegs.end(); itap++ ) {
              LineSubSegment* p_subseg_a = (*itap);
              a_subseg_painter.drawLine( toQPoint( p_subseg_a->m_subseg.source() ), 
                                         toQPoint( p_subseg_a->m_subseg.target() ));
            }
          }
        }
        a_subseg_painter.end();

        QPainter b_subseg_painter(device);
        QPen b_subseg_pen( BETA_COLOR );
        b_subseg_pen.setWidth( LINE_WIDTH );
        b_subseg_painter.setPen( b_subseg_pen );
        for( std::vector<Obstacle*>::iterator it = obstacles.begin();
             it != obstacles.end(); it++ ) {
          Obstacle* p_obstacle = (*it);
          if ( p_obstacle && p_obstacle->mp_beta_seg ) {
            for( std::vector< LineSubSegment* >::iterator itbp = p_obstacle->mp_beta_seg->m_subsegs.begin();
                 itbp != p_obstacle->mp_beta_seg->m_subsegs.end(); itbp++ ) {
              LineSubSegment* p_subseg_b = (*itbp);
              b_subseg_painter.drawLine( toQPoint( p_subseg_b->m_subseg.source() ), 
                                         toQPoint( p_subseg_b->m_subseg.target() ));
            }
          }
        }
        b_subseg_painter.end();
      }
  
      QPainter cp_painter(device);
      QPen cp_pen( CENTER_POINT_COLOR );
      cp_pen.setWidth( POINT_SIZE );
      cp_painter.setPen( cp_pen );
      cp_painter.drawPoint( toQPoint( mp_mgr->mp_worldmap->get_central_point() ) );
      cp_painter.end();

      QPainter bk_painter(device);
      QPen bk_pen( BK_COLOR );
      bk_pen.setWidth( POINT_SIZE );
      bk_painter.setPen( bk_pen );
      for( std::vector<Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        Obstacle* p_obstacle = (*it);
        if ( p_obstacle ) {
          bk_painter.drawPoint( toQPoint( p_obstacle->m_bk ) );
        }
      }
      bk_painter.end();
  
      QPainter intsec_painter(device);
      QPen intsec_pen( INTERSECTION_COLOR );
      intsec_pen.setWidth( POINT_SIZE );
      intsec_painter.setPen( intsec_pen );
      for( std::vector<Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        Obstacle* p_obstacle = (*it);
        if ( p_obstacle ) {
          for( std::vector< IntersectionPoint >::iterator itap = p_obstacle->m_alpha_intersection_points.begin();
               itap != p_obstacle->m_alpha_intersection_points.end(); itap++ ) {
            IntersectionPoint alpha_intsec = (*itap);
            intsec_painter.drawPoint( toQPoint( alpha_intsec.m_point ) );
          }
          for( std::vector< IntersectionPoint >::iterator itbp = p_obstacle->m_beta_intersection_points.begin();
               itbp != p_obstacle->m_beta_intersection_points.end(); itbp++ ) {
            IntersectionPoint beta_intsec = (*itbp);
            intsec_painter.drawPoint( toQPoint( beta_intsec.m_point ) );
          }
        }
      }
      intsec_painter.end();

      QPainter text_painter(device);
      QPen text_pen( TEXT_COLOR );
      text_painter.setPen(text_pen);
      for( std::vector<Obstacle*>::iterator it = obstacles.begin();
           it != obstacles.end(); it++ ) {
        Obstacle* p_obstacle = (*it);
        if( p_obstacle ) {
          int c_x = (p_obstacle->m_pgn.bbox().xmax() + p_obstacle->m_pgn.bbox().xmin() )/2;
          int c_y = (p_obstacle->m_pgn.bbox().ymax() + p_obstacle->m_pgn.bbox().ymin() )/2;
          text_painter.drawText( c_x, c_y, QString::number(p_obstacle->get_index()) );
        }
      }
      text_painter.end();

      if( mp_mgr ) {
        if( mp_mgr->m_start_x >= 0 && mp_mgr->m_start_y >= 0 ) {
          QPainter st_painter(device);
          QPen st_paintpen( START_COLOR );
          st_paintpen.setWidth( POINT_SIZE );
          st_painter.setPen( st_paintpen );
          st_painter.drawPoint( QPoint( mp_mgr->m_start_x, mp_mgr->m_start_y ) );
          st_painter.end();
        }

        if( mp_mgr->m_goal_x >= 0 && mp_mgr->m_goal_y >= 0 ) {
          QPainter gt_painter(device);
          QPen gt_paintpen( GOAL_COLOR );
          gt_paintpen.setWidth( POINT_SIZE );
          gt_painter.setPen( gt_paintpen );
          gt_painter.drawPoint( QPoint( mp_mgr->m_goal_x, mp_mgr->m_goal_y ) );
          gt_painter.end();
        }
      }

      QPainter pos_ref_painter(device);
      QPen pos_ref_paintpen( RULE_POS_COLOR );
      pos_ref_paintpen.setWidth( RULE_LINE_WIDTH );
      pos_ref_painter.setPen( pos_ref_paintpen );
      
      for( vector< ReferenceFrame* >::iterator it =  m_viz_pos_refs.begin();
           it != m_viz_pos_refs.end(); it++ ) {
        ReferenceFrame* p_pos_ref = (*it);
        pos_ref_painter.drawLine( toQPoint( p_pos_ref->m_segment.source() ), 
                                  toQPoint( p_pos_ref->m_segment.target() ));
      }
      pos_ref_painter.end();

      QPainter neg_ref_painter(device);
      QPen neg_ref_paintpen( RULE_NEG_COLOR );
      neg_ref_paintpen.setWidth( RULE_LINE_WIDTH );
      neg_ref_painter.setPen( neg_ref_paintpen );

      for( vector< ReferenceFrame* >::iterator it =  m_viz_neg_refs.begin();
           it != m_viz_neg_refs.end(); it++ ) {
        ReferenceFrame* p_neg_ref = (*it);
        neg_ref_painter.drawLine( toQPoint( p_neg_ref->m_segment.source() ), 
                                  toQPoint( p_neg_ref->m_segment.target() ));
      }
      neg_ref_painter.end();

      if( mp_viz_string_class ) {
        QPainter st_cls_painter(device);
        QPen st_cls_paintpen( STRING_CLASS_POINT_COLOR );
        st_cls_paintpen.setWidth( STRING_CLASS_POINT_SIZE );
        st_cls_painter.setPen( st_cls_paintpen ); 

        if( mp_viz_string_class->mp_reference_frames.size() > 0 ) {
          
          st_cls_painter.drawLine( QPoint( mp_mgr->m_start_x, mp_mgr->m_start_y ),
                                   toQPoint( mp_viz_string_class->mp_reference_frames[0]->m_mid_point ) );
          for( unsigned int i=0; i < mp_viz_string_class->mp_reference_frames.size()-1; i++ ) {

            ReferenceFrame* p_curr_rf_str_cls = mp_viz_string_class->mp_reference_frames[i];
            ReferenceFrame* p_next_rf_str_cls = mp_viz_string_class->mp_reference_frames[i+1];
            if( p_curr_rf_str_cls && p_next_rf_str_cls ) {
              st_cls_painter.drawLine( toQPoint( p_curr_rf_str_cls->m_mid_point ),
                                       toQPoint( p_next_rf_str_cls->m_mid_point ) );
            }
          }
          st_cls_painter.drawLine( toQPoint( mp_viz_string_class->mp_reference_frames.back()->m_mid_point ),
                                   QPoint( mp_mgr->m_goal_x, mp_mgr->m_goal_y ) );
        }
 
        st_cls_painter.end();     
      }

    } 
  }
}

void SpatialRelationsViz::prev_region() {
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

void SpatialRelationsViz::next_region() {
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

void SpatialRelationsViz::prev_subregion() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_region_idx >= 0 && m_region_idx < static_cast<int>(mp_mgr->mp_worldmap->get_subregion_set().size()) ) {
      SubRegionSet* p_subregions = mp_mgr->mp_worldmap->get_subregion_set() [m_region_idx];
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

void SpatialRelationsViz::next_subregion() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_region_idx >= 0 && m_region_idx < static_cast<int>(mp_mgr->mp_worldmap->get_subregion_set().size()) ) {
         SubRegionSet* p_subregions = mp_mgr->mp_worldmap->get_subregion_set() [m_region_idx];
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

void SpatialRelationsViz::prev_line_subsegment_set() {
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

void SpatialRelationsViz::next_line_subsegment_set() {
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

void SpatialRelationsViz::prev_string_class() {
  if( mp_mgr ) {
    if( m_string_class_idx >= 0 ) {
      m_string_class_idx --;
      update_viz_string_class();
    }
    else {
      m_string_class_idx = mp_mgr->mp_string_classes.size()-1;
      update_viz_string_class();
    }
  }
}

void SpatialRelationsViz::next_string_class() {
  if( mp_mgr ) {
    if( m_string_class_idx < mp_mgr->mp_string_classes.size()-1 ) {
      m_string_class_idx ++;
      update_viz_string_class();
    }
    else {
      m_string_class_idx = -1;
      update_viz_string_class();
    }
  }
}

void SpatialRelationsViz::prev_line_subsegment() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < static_cast<int>(mp_mgr->mp_worldmap->get_linesubsegment_set().size()) ) {
      LineSubSegmentSet* p_subsegment_set = mp_mgr->mp_worldmap->get_linesubsegment_set() [m_subsegment_set_idx];
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

void SpatialRelationsViz::next_line_subsegment() {
  if ( mp_mgr->mp_worldmap ) {
    if ( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < static_cast<int>(mp_mgr->mp_worldmap->get_linesubsegment_set().size()) ) {
      LineSubSegmentSet* p_subsegment_set = mp_mgr->mp_worldmap->get_linesubsegment_set() [m_subsegment_set_idx];
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

bool SpatialRelationsViz::save( QString filename ) {
  if( mp_mgr->mp_worldmap ) {
    mp_mgr->mp_worldmap->to_xml(filename.toStdString());
    return true;
  }
  return false;
}

bool SpatialRelationsViz::load( QString filename ) {
  if ( mp_mgr->mp_worldmap == NULL) {
    mp_mgr->mp_worldmap = new WorldMap();
  }

  mp_mgr->mp_worldmap->from_xml(filename.toStdString());

  QPixmap emptyPix( mp_mgr->get_world_map()->get_width(), mp_mgr->get_world_map()->get_height() );
  emptyPix.fill(QColor("white"));
  std::cout << " EMPTY PIX " << emptyPix.width() << " * " << emptyPix.height() << std::endl;
  //setPixmap(pix);
  setPixmap(emptyPix);

  mp_mgr->get_world_map()->init(false);
  repaint();

  return true;
}

SubRegionSet* SpatialRelationsViz::get_selected_region() {
  SubRegionSet* p_region = NULL;
  if ( mp_mgr->get_world_map() ) {
    if ( mp_mgr->get_world_map()->get_subregion_set().size() > 0 ) {
      if( m_region_idx >= 0 && m_region_idx < mp_mgr->get_world_map()->get_subregion_set().size() ) {
        return mp_mgr->get_world_map()->get_subregion_set()[ m_region_idx ];
      }
    }  
  }
  return p_region;
}


SubRegion* SpatialRelationsViz::get_selected_subregion() {
  SubRegion* p_subregion = NULL;
  SubRegionSet* p_region = get_selected_region();
  if( p_region ) {
    if( p_region->m_subregions.size() > 0 ) {
      if( m_subregion_idx >= 0 && m_subregion_idx < p_region->m_subregions.size() ) {
        return p_region->m_subregions[m_subregion_idx];
      }
    }
  }
  return p_subregion;
}

StringClass* SpatialRelationsViz::get_selected_string_class() {
  StringClass* p_string_class = NULL;
  if( mp_mgr->mp_string_classes.size() > 0 ) {
    if( m_string_class_idx >= 0 && m_string_class_idx < mp_mgr->mp_string_classes.size() ) {
      return mp_mgr->mp_string_classes[m_string_class_idx];
    }
  } 
  return p_string_class;
}

LineSubSegmentSet* SpatialRelationsViz::get_selected_line_subsegment_set() {
  LineSubSegmentSet* p_subseg_set = NULL;
  if ( mp_mgr->get_world_map() ) {
    if ( mp_mgr->get_world_map()->get_linesubsegment_set().size() > 0 ) {
      if( m_subsegment_set_idx >= 0 && m_subsegment_set_idx < mp_mgr->get_world_map()->get_linesubsegment_set().size() ) {
        return mp_mgr->get_world_map()->get_linesubsegment_set()[ m_subsegment_set_idx ];
      }
    }  
  }
  return p_subseg_set;
}

LineSubSegment* SpatialRelationsViz::get_selected_line_subsegment() {
  LineSubSegment* p_subseg= NULL;  
  LineSubSegmentSet* p_subseg_set = get_selected_line_subsegment_set();
  if( p_subseg_set ) {
    if( p_subseg_set->m_subsegs.size() > 0 ) {
      if( m_subsegment_idx >= 0 && m_subsegment_idx < p_subseg_set->m_subsegs.size() ) {
        return p_subseg_set->m_subsegs[m_subsegment_idx];
      }
    }
  }
  return p_subseg;
}

void SpatialRelationsViz::update_viz_subregions() {
  m_viz_subregions.clear();
  if( SUBREGION == m_mode ) {
    SubRegionSet* p_region = get_selected_region();
    if( p_region ) {
      SubRegion* p_subregion = get_selected_subregion();
      if (p_subregion) {
        m_viz_subregions.push_back( p_subregion );
      }
      else {
        for( unsigned int i=0; i < p_region->m_subregions.size(); i++ ) {
          SubRegion* p_subregion = p_region->m_subregions[i];
          m_viz_subregions.push_back( p_subregion );
        }
      }
    }
  }
  else if( LINE_SUBSEGMENT == m_mode ) {
    LineSubSegmentSet* p_subseg_set = get_selected_line_subsegment_set();
    if( p_subseg_set ) {
      LineSubSegment* p_subseg = get_selected_line_subsegment();
      if( p_subseg ) {
        for( unsigned int i=0; i < p_subseg->m_neighbors.size(); i++ ) {
          SubRegion* p_subregion = p_subseg->m_neighbors[i];
          m_viz_subregions.push_back( p_subregion );
        }
      }
    }
  }
}

void SpatialRelationsViz::update_viz_line_subsegments() {
  m_viz_subsegments.clear();
  if( SUBREGION == m_mode ) {
    SubRegionSet* p_region = get_selected_region();
    if( p_region ) {
      SubRegion* p_subregion = get_selected_subregion();
      if (p_subregion) {
        for( unsigned int i=0; i < p_subregion->m_neighbors.size(); i++ ) {
          LineSubSegment* p_subseg = p_subregion->m_neighbors[i];
          if( p_subseg ) {
            m_viz_subsegments.push_back( p_subseg );
          }
        }
      }
      else {
        if( p_region->mp_line_segments_a ){
          for( unsigned int i=0; i < p_region->mp_line_segments_a->m_subsegs.size(); i++ ) {
            LineSubSegment* p_subseg = p_region->mp_line_segments_b->m_subsegs[i];
            if( p_subseg ) {
              m_viz_subsegments.push_back( p_subseg );
            }
          }
        }

        if( p_region->mp_line_segments_b ){
          for( unsigned int i=0; i < p_region->mp_line_segments_b->m_subsegs.size(); i++ ) {
            LineSubSegment* p_subseg = p_region->mp_line_segments_b->m_subsegs[i];
            if( p_subseg ) {
              m_viz_subsegments.push_back( p_subseg );
            }
          }
        }
          
      } 
    }
  }
  else if( LINE_SUBSEGMENT == m_mode ) {
    LineSubSegmentSet* p_subseg_set = get_selected_line_subsegment_set();
    if( p_subseg_set ) {
      LineSubSegment* p_subseg = get_selected_line_subsegment();
      if( p_subseg ) {
        m_viz_subsegments.push_back( p_subseg ); 
      }
      else {
        for( unsigned int i=0; i < p_subseg_set->m_subsegs.size(); i++ ) {
          LineSubSegment* p_subseg = p_subseg_set->m_subsegs[i];
          if( p_subseg ){
            m_viz_subsegments.push_back( p_subseg ); 
          }
        }
      }
    }
  }

}

void SpatialRelationsViz::update_viz_string_class() {
  mp_viz_string_class = get_selected_string_class();
}

void SpatialRelationsViz::set_mode( SpatialRelationsVizMode mode ) {
  m_mode = mode;
  m_region_idx = -1;
  m_subregion_idx = -1;
  m_subsegment_set_idx = -1;
  m_subsegment_idx = -1;
  update_viz_subregions();
  update_viz_line_subsegments();
}

void SpatialRelationsViz::mousePressEvent( QMouseEvent * event ) {
  if( event->button() == Qt::LeftButton ) {
    Point2D clicked_point( event->x(), event->y() );
    Obstacle* p_selected_obstacle = mp_reference_frame_set->get_world_map()->find_obstacle( clicked_point );
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

bool SpatialRelationsViz::is_selected_obstacle( Obstacle* p_obstacle ) {
    for(unsigned int i=0; i < m_selected_obstacles.size(); i++ ) {
        Obstacle* p_current_obstacle = m_selected_obstacles[i];
    if( p_current_obstacle ) {
      if( p_current_obstacle == p_obstacle ) {
        return true;
      }
    }
  }
  return false;

}
  
bool SpatialRelationsViz::unselect_obstacle( Obstacle* p_obstacle ) {
  for( std::vector< Obstacle* >::iterator it = m_selected_obstacles.begin();
       it != m_selected_obstacles.end(); it ++ ) {
    Obstacle* p_current_obstacle = (*it);
    if( p_current_obstacle ) {
      if( p_current_obstacle == p_obstacle ) {
        m_selected_obstacles.erase(it);
        return true;
      }
    }
  }
  return false;
}

void SpatialRelationsViz::update_viz_reference_frames() {

  if( mp_mgr && mp_mgr->mp_rule ) {
    mp_mgr->mp_rule->get_reference_frames( m_viz_pos_refs, m_viz_neg_refs );
    cout << "update Viz Reference Frames: POS " << m_viz_pos_refs.size();
    cout << " NEG " << m_viz_neg_refs.size() << endl;
  }
  repaint();
}
