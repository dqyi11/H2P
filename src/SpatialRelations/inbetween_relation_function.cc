#include "inbetween_relation_function.h"

using namespace std;
using namespace h2p;

InBetweenRelationFunction::InBetweenRelationFunction() {
  mp_obstacles.clear();
}

InBetweenRelationFunction::~InBetweenRelationFunction() {
  mp_obstacles.clear();
}

Rule* InBetweenRelationFunction::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;

  if( p_reference_frame_set ) {
    // check whether there is a reference frame connecting to reference frames
    for( unsigned int i=0; i<p_reference_frame_set->get_reference_frames().size(); i++ ) {
      ReferenceFrame* p_ref = p_reference_frame_set->get_reference_frames()[i];
      if( p_ref ) {
        if( p_ref->mp_line_subsegment ) {
          if( p_ref->mp_line_subsegment->is_connected( mp_obstacles[0] ) &&
              p_ref->mp_line_subsegment->is_connected( mp_obstacles[1] ) ) {
            //rules.push_back( make_pair( p_ref, true ) );
          }
        }    
      }
    }
    /*
    if( rules.size() == 0 ) {
      for( unsigned int i=0; i<p_reference_frame_set->get_reference_frames().size(); i++ ) {
        ReferenceFrame* p_ref = p_reference_frame_set->get_reference_frames()[i];
        if( p_ref ) {
          if( p_ref->mp_line_subsegment ) {
            if( p_ref->mp_line_subsegment->is_connected( mp_obstacles[0] ) &&
                p_ref->mp_line_subsegment->m_is_connected_to_central_point ) {

              rules.push_back( make_pair( p_ref, true ) );
            }
            else if( p_ref->mp_line_subsegment->is_connected( mp_obstacles[1] ) &&
                     p_ref->mp_line_subsegment->m_is_connected_to_central_point ) {
              rules.push_back( make_pair( p_ref, true ) );
            }
          }
        }
      }
    } */
  }
  return p_rule;  
}

string InBetweenRelationFunction::get_name() {
  string name = "IN_BETWEEN(";
  for(unsigned int i=0; i<mp_obstacles.size(); i++) {
    if( i < mp_obstacles.size()-1 ) {
      name += mp_obstacles[i]->get_name() + ",";
    }
    else{
      name += mp_obstacles[i]->get_name();
    }
  }  
  name += ")";
  return name;
}
