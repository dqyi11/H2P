#include <sstream>
#include <CGAL/squared_distance_2.h>
#include "line_subsegment.h"
#include "obstacle.h"
#include "worldmap.h"

using namespace std;
using namespace h2p;

IntersectionPoint::IntersectionPoint( Point2D point ) {
  m_point = point;
  m_dist_to_bk = 0.0;
  mp_obstacle = NULL;
}

IntersectionPoint::~IntersectionPoint() {
  mp_obstacle = NULL;
}

namespace h2p {

  std::ostream& operator<<( std::ostream& out, const IntersectionPoint& other ) {
    out << "P[" << other.m_point.x() << "," << other.m_point.y() << "]";
    out << "  (" << other.m_dist_to_bk << ")" << std::endl;
    return out;
  }
}

LineSubSegment::LineSubSegment( Point2D pos_a, Point2D pos_b, LineSubSegmentSet* p_subseg_set, unsigned int index, bool is_connected_to_central_point ) {
  _index = index;
  m_is_connected_to_central_point = is_connected_to_central_point;
  m_subseg = Segment2D( pos_a, pos_b );
  _p_subseg_set = p_subseg_set;

  m_connected_to_boundary = false;
  m_connected_obstacles.clear();
}

LineSubSegment::~LineSubSegment() {
  _p_subseg_set = NULL;

  m_connected_to_boundary = false;
  m_connected_obstacles.clear();
}

string LineSubSegment::get_name() {
  if( _p_subseg_set ) {
    stringstream ss;
    ss << _p_subseg_set->get_name().c_str() << "-" << _index;
    return ss.str();
  }
  return "NA";
}
    
Point2D LineSubSegment::sample_position() {

  float ratio = static_cast<float> (rand())/static_cast<float>(RAND_MAX);
  
  double s_x = CGAL::to_double( m_subseg.source().x() );
  double s_y = CGAL::to_double( m_subseg.source().y() );
  double t_x = CGAL::to_double( m_subseg.target().x() );
  double t_y = CGAL::to_double( m_subseg.target().y() );
  
  double n_x = ( ratio * s_x + (1.0 - ratio) * t_x ) ;
  double n_y = ( ratio * s_y + (1.0 - ratio) * t_y ) ;
  return Point2D( n_x, n_y );
}

bool LineSubSegment::contains( Point2D point ) {
  return m_subseg.has_on(point);
}

void LineSubSegment::to_xml( const string& filename )const {
  xmlDocPtr doc = xmlNewDoc( ( xmlChar* )( "1.0" ) );
  xmlNodePtr root = xmlNewDocNode( doc, NULL, ( xmlChar* )( "root" ), NULL );
  xmlDocSetRootElement( doc, root );
  to_xml( doc, root );
  xmlSaveFormatFileEnc( filename.c_str(), doc, "UTF-8", 1 );
  xmlFreeDoc( doc );
  return;
}

bool LineSubSegment::is_connected( Obstacle* p_obstacle ) {

  for( vector< Obstacle* >::iterator it = m_connected_obstacles.begin();
       it != m_connected_obstacles.end(); it++ ) {
    Obstacle* p_current_obstacle = (*it);
    if( p_current_obstacle ) {
      //cout << "COMPARE " << p_current_obstacle->get_name() << " " << p_obstacle->get_name() << endl;
      if( p_current_obstacle->get_name() == p_obstacle->get_name() ) {
        return true;
      }
    }
  }
  return false;
}

void LineSubSegment::to_xml( xmlDocPtr doc, xmlNodePtr root )const {

}

void LineSubSegment::from_xml( const string& filename ) {
  xmlDoc * doc = NULL;
  xmlNodePtr root = NULL;
  doc = xmlReadFile( filename.c_str(), NULL, 0 );
  if( doc != NULL ){
    root = xmlDocGetRootElement( doc );
    if( root->type == XML_ELEMENT_NODE ){
      xmlNodePtr l1 = NULL;
      for( l1 = root->children; l1; l1 = l1->next ){
        if( l1->type == XML_ELEMENT_NODE ){
          if( xmlStrcmp( l1->name, ( const xmlChar* )( "line_subsegment" ) ) == 0 ){
            from_xml( l1 );
          }
        }
      }
    }
    xmlFreeDoc( doc );
  }
  return;
}

void LineSubSegment::from_xml( xmlNodePtr root ) {

}

namespace h2p {
  std::ostream& operator<<( std::ostream& out, const LineSubSegment& other ) {
    out << "LineSubSegment [" << other.m_subseg.source().x() << "," << other.m_subseg.source().y() <<"] ==> ";
    out << "[" << other.m_subseg.target().x() <<"," << other.m_subseg.target().y() << "]" << std::endl;
    return out;
  }
}

LineSubSegmentSet::LineSubSegmentSet( Point2D pos_a, Point2D pos_b, unsigned int type, Obstacle* p_obstacle ) {

  m_seg = Segment2D(pos_a, pos_b);
  m_type = type;
  _p_obstacle = p_obstacle;
  m_subsegs.clear();
}

LineSubSegmentSet::~LineSubSegmentSet() {
  for( vector< LineSubSegment* >::iterator it=m_subsegs.begin(); it != m_subsegs.end(); it++ ) {
    LineSubSegment* p_line_subseg = (*it);
    delete p_line_subseg;
    p_line_subseg = NULL;
  }
  m_subsegs.clear();
}

string LineSubSegmentSet::type_to_std_string ( const unsigned int& type ) {
  switch( type ) {
  case( LINE_TYPE_ALPHA ):
    return "A";
  case( LINE_TYPE_BETA ):
    return "B";
  case( LINE_TYPE_UNKNOWN ):
  default:
    return "na";
  }
}

unsigned int LineSubSegmentSet::type_from_std_string ( const string& type_str ) {
  for( unsigned int i = 0; i < NUM_LINE_TYPE; i++ ){
    if( type_str == LineSubSegmentSet::type_to_std_string( i ) ){
      return i;
    }
  }
  return LINE_TYPE_UNKNOWN;
}

bool LineSubSegmentSet::load( vector<IntersectionPoint>& intersections ) {

  if( _p_obstacle == NULL ) {
    return false;
  }
  if( _p_obstacle->get_world() == NULL ) {
    return false;
  }
  if( intersections.size()==0 ) {
    return false;
  }

  m_subsegs.clear();
  if( m_type == LINE_TYPE_ALPHA ) {

    unsigned int idx = 0;
    for( unsigned int i = 0; i < intersections.size()-1; i ++ ) {
      IntersectionPoint sec1 = intersections[i];
      IntersectionPoint sec2 = intersections[i+1];

      Point2D mid_point = Point2D( (sec1.m_point.x()+sec2.m_point.x())/2, (sec1.m_point.y()+sec2.m_point.y())/2);
      if (_p_obstacle->get_world()->is_in_obstacle(mid_point) == false ) {

        if ( sec1.m_dist_to_bk <= _p_obstacle->m_dist_bk2cp && sec2.m_dist_to_bk >= _p_obstacle->m_dist_bk2cp ) {
          LineSubSegment* p_subseg = new LineSubSegment( sec1.m_point , _p_obstacle->get_world()->get_central_point(), this, idx, true );
          p_subseg->m_connected_obstacles.push_back( sec1.mp_obstacle );
          p_subseg->m_is_connected_to_central_point = true;
          m_subsegs.push_back( p_subseg );
          idx += 1;

          LineSubSegment* p_subseg2 = new LineSubSegment( _p_obstacle->get_world()->get_central_point(), sec2.m_point, this, idx, true );
          p_subseg2->m_is_connected_to_central_point = true;
          p_subseg2->m_connected_obstacles.push_back( sec2.mp_obstacle );
          m_subsegs.push_back( p_subseg2 );
          idx += 1;
        }
        else {
          LineSubSegment* p_subseg = new LineSubSegment( sec1.m_point, sec2.m_point, this, idx );
          p_subseg->m_connected_obstacles.push_back( sec1.mp_obstacle );
          p_subseg->m_connected_obstacles.push_back( sec2.mp_obstacle );
          m_subsegs.push_back(p_subseg);
          idx += 1;
        }
      }
    }

    if( intersections[intersections.size()-1].m_dist_to_bk > _p_obstacle->m_dist_bk2cp ) {
      Point2D end_point = static_cast<Point2D>(m_seg.target());
      Point2D last_sec_point = intersections[intersections.size()-1].m_point;

      Point2D mid_point = Point2D( (end_point.x()+last_sec_point.x())/2, (end_point.y()+last_sec_point.y())/2);
      if( _p_obstacle->get_world()->is_in_obstacle(mid_point) == false ) {
        LineSubSegment* p_subseg = new LineSubSegment( intersections[intersections.size()-1].m_point, end_point, this, idx );
        p_subseg->m_connected_obstacles.push_back( intersections[intersections.size()-1].mp_obstacle );
        p_subseg->m_connected_to_boundary = true;
        m_subsegs.push_back( p_subseg );
        idx += 1;
      }
    }
    else{
      Point2D end_point = static_cast<Point2D>(m_seg.target());
      Point2D last_sec_point = intersections[intersections.size()-1].m_point;

      Point2D mid_point = Point2D( (end_point.x()+last_sec_point.x())/2, (end_point.y()+last_sec_point.y())/2);
      if( _p_obstacle->get_world()->is_in_obstacle(mid_point) == false ) {
        LineSubSegment* p_subseg = new LineSubSegment( intersections[intersections.size()-1].m_point, _p_obstacle->get_world()->get_central_point(), this, idx, true );
        p_subseg->m_connected_obstacles.push_back( intersections[intersections.size()-1].mp_obstacle );
        p_subseg->m_is_connected_to_central_point = true;
        m_subsegs.push_back( p_subseg );
        idx += 1;

        LineSubSegment* p_subseg2 = new LineSubSegment( _p_obstacle->get_world()->get_central_point(), end_point, this, idx, true );
        p_subseg2->m_connected_obstacles.push_back( intersections[intersections.size()-1].mp_obstacle );
        p_subseg2->m_is_connected_to_central_point = true;
        p_subseg2->m_connected_to_boundary = true;
        m_subsegs.push_back( p_subseg2 );
        idx += 1;
      }
    }
  }
  else if( m_type == LINE_TYPE_BETA ) {
    unsigned int idx = 0;
    for( unsigned int i = 0; i < intersections.size()-1; i++ ) {
      IntersectionPoint sec1 = intersections[i];
      IntersectionPoint sec2 = intersections[i+1];

      Point2D mid_point = Point2D( (sec1.m_point.x()+sec2.m_point.x())/2, (sec1.m_point.y()+sec2.m_point.y())/2);
      if (_p_obstacle->get_world()->is_in_obstacle(mid_point) == false ) {
        LineSubSegment* p_subseg = new LineSubSegment( sec1.m_point, sec2.m_point, this, idx );
        p_subseg->m_connected_obstacles.push_back( sec1.mp_obstacle );
        p_subseg->m_connected_obstacles.push_back( sec2.mp_obstacle );
        m_subsegs.push_back( p_subseg );
        idx += 1;
      }

    }
    Point2D end_point = static_cast<Point2D>(m_seg.target());
    Point2D last_sec_point = intersections[intersections.size()-1].m_point;

    Point2D mid_point = Point2D( (end_point.x()+last_sec_point.x())/2, (end_point.y()+last_sec_point.y())/2);
    if (_p_obstacle->get_world()->is_in_obstacle(mid_point) == false ) {
      LineSubSegment* p_subseg = new LineSubSegment( last_sec_point, end_point, this, idx );
      p_subseg->m_connected_obstacles.push_back( intersections[intersections.size()-1].mp_obstacle );
      p_subseg->m_connected_to_boundary = true;
      m_subsegs.push_back( p_subseg );
      idx += 1;
    }
  }else {
    return false;
  }
  return true;
}

string LineSubSegmentSet::get_name() {
  stringstream ss;
  if( m_type == LINE_TYPE_ALPHA ) {
    ss << "A";
  }
  else if( m_type == LINE_TYPE_BETA ) {
    ss << "B";
  }
  ss << _p_obstacle->get_index();
  return ss.str();
}

void LineSubSegmentSet::to_xml( const string& filename )const {
  xmlDocPtr doc = xmlNewDoc( ( xmlChar* )( "1.0" ) );
  xmlNodePtr root = xmlNewDocNode( doc, NULL, ( xmlChar* )( "root" ), NULL );
  xmlDocSetRootElement( doc, root );
  to_xml( doc, root );

  xmlSaveFormatFileEnc( filename.c_str(), doc, "UTF-8", 1 );
  xmlFreeDoc( doc );
  return;
}

void LineSubSegmentSet::to_xml( xmlDocPtr doc, xmlNodePtr root )const {
  xmlNodePtr node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "line_subsegment_set" ), NULL );
  for( unsigned int i=0; i < m_subsegs.size(); i++ ) {
    if( m_subsegs[i] != NULL ) {
      m_subsegs[i]->to_xml( doc, node );
    }
  }
  xmlAddChild( root, node );
  return;
}

void LineSubSegmentSet::from_xml( const string& filename ) {
  xmlDoc * doc = NULL;
  xmlNodePtr root = NULL;
  doc = xmlReadFile( filename.c_str(), NULL, 0 );
  if( doc != NULL ){
    root = xmlDocGetRootElement( doc );
    if( root->type == XML_ELEMENT_NODE ){
      xmlNodePtr l1 = NULL;
      for( l1 = root->children; l1; l1 = l1->next ){
        if( l1->type == XML_ELEMENT_NODE ){
          if( xmlStrcmp( l1->name, ( const xmlChar* )( "line_subsegment_set" ) ) == 0 ){
            from_xml( l1 );
          }
        }
      }
    }
    xmlFreeDoc( doc );
  }
  return;
}

void LineSubSegmentSet::from_xml( xmlNodePtr root ) {

}

namespace world {

  std::ostream& operator<<( std::ostream& out, const LineSubSegmentSet& other ) {
    for( unsigned int i =0; i < other.m_subsegs.size(); i++ ) {
      LineSubSegment* seg = other.m_subsegs[i];
      out << (*seg) << std::endl;
    }
    return out;
  }

}
