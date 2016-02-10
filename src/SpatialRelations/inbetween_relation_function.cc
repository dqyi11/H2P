#include "inbetween_relation_function.h"

using namespace std;
using namespace h2p;

InBetweenRelationFunction::InBetweenRelationFunction() {
  mp_obstacles.clear();
}

InBetweenRelationFunction::~InBetweenRelationFunction() {
  mp_obstacles.clear();
}

Obstacle* InBetweenRelationFunction::get_primary_obstacle() {
  if( mp_obstacles.size() > 0 ) {
    return mp_obstacles[0];
  }
  return NULL;
}

Rule* InBetweenRelationFunction::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;
  vector<ReferenceFrame*> refs;
  if( p_reference_frame_set ) {
    // check whether there is a reference frame connecting to reference frames
    for( unsigned int i=0; i<p_reference_frame_set->get_reference_frames().size(); i++ ) {
      ReferenceFrame* p_ref = p_reference_frame_set->get_reference_frames()[i];
      if( p_ref ) {
        if( p_ref->mp_line_subsegment ) {
          if( p_ref->mp_line_subsegment->is_connected( mp_obstacles[0] ) &&
              p_ref->mp_line_subsegment->is_connected( mp_obstacles[1] ) ) {
            refs.push_back( p_ref );
          }
        }    
      }
    }
  }
  vector<Rule*> or_rules;
  for( unsigned int i=0; i< refs.size(); i++ ) {
    or_rules.push_back( new Rule( ATOM, refs[i] ) );
  }
  p_rule = new Rule( MOLECULE_OR, NULL, or_rules );
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
