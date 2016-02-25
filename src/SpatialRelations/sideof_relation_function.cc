#include "cgal_util.h"
#include "sideof_relation_function.h"

using namespace std;
using namespace h2p;

#define PI 3.14159265

SideOfRelationFunction::SideOfRelationFunction( side_type_t side_type ) {
  m_type = side_type;
  mp_obstacle = NULL;
}

SideOfRelationFunction::~SideOfRelationFunction() {
  mp_obstacle = NULL;
}

Obstacle* SideOfRelationFunction::get_primary_obstacle() {
  return mp_obstacle;
}

Rule* SideOfRelationFunction::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;
  vector<ReferenceFrame* > refs;
  if( p_reference_frame_set ) {
    for( unsigned int i=0; i<p_reference_frame_set->get_reference_frames().size(); i++ ) {
      ReferenceFrame* p_ref = p_reference_frame_set->get_reference_frames()[i];
      if( p_ref ) {
        if( p_ref->mp_line_subsegment && p_ref->mp_line_subsegment->is_connected( mp_obstacle ) ) {
          double radius = get_radius( mp_obstacle->get_centroid(), p_ref->m_mid_point );         
          switch( m_type ) {
          default:
            break;
          case SIDE_TYPE_LEFT:
            // [0.75PI, PI] [-PI, -0.75PI) 
            if( (radius>=0.75*PI && radius<PI) || (radius>=-PI && radius<-0.75*PI) ) {
              refs.push_back( p_ref );
            }
            break;
          case SIDE_TYPE_RIGHT:
            // [-0.25PI, 0.25PI)
            if( radius>=-0.25*PI && radius<0.25*PI ) {
              refs.push_back( p_ref );
            }
            break;
          case SIDE_TYPE_TOP:
            // [-0.75PI, -0.25PI)
            if( radius>=-0.75*PI && radius<-0.25*PI ) {
              refs.push_back( p_ref );
            }
            break;
          case SIDE_TYPE_BOTTOM:
            // [0.25PI, 0.75PI)
            if( radius>=0.25*PI && radius<0.75*PI ) {
              refs.push_back( p_ref );
            }
            break;
          } 
        }
      }
    }
    
    // IF no reference is find, use region to find associated reference frames  
  }

  vector<Rule*> or_rules;
  for( unsigned int i=0; i< refs.size(); i++ ) {
    or_rules.push_back( new Rule( ATOM, refs[i] ) );
  }
  p_rule = new Rule( MOLECULE_OR, NULL, or_rules );
  return p_rule;  
}

string SideOfRelationFunction::get_name() {
  string name = "";
  switch( m_type ) {
    default:
      name = "UNKNOWN";
      break;
    case SIDE_TYPE_LEFT:
      name = "LEFT";
      break;
    case SIDE_TYPE_RIGHT:
      name = "RIGHT";
      break;
    case SIDE_TYPE_TOP:
      name = "TOP";
      break;
    case SIDE_TYPE_BOTTOM:
      name = "BOTTOM";
      break;
  }
  if( mp_obstacle ) {
    name += "(" + mp_obstacle->get_name() + ")";
  }
  return name;
}

SpatialRelationFuncType SideOfRelationFunction::get_type() {
  return SPATIAL_REL_FUNC_SIDE_OF;
}
