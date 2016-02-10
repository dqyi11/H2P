#include "avoid_relation_function.h"

using namespace std;
using namespace h2p;

AvoidRelationFunction::AvoidRelationFunction() {
  mp_obstacle = NULL;
}

AvoidRelationFunction::~AvoidRelationFunction() {
  mp_obstacle = NULL;
}

Obstacle* AvoidRelationFunction::get_primary_obstacle() {
  return mp_obstacle;
}

Rule* AvoidRelationFunction::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;
  vector<ReferenceFrame*> refs;

  if( p_reference_frame_set ) {
    // check whether there is a reference frame connecting to reference frames
    for( unsigned int i=0; i<p_reference_frame_set->get_reference_frames().size(); i++ ) {
      ReferenceFrame* p_ref = p_reference_frame_set->get_reference_frames()[i];
      if( p_ref ) {
        if( p_ref->mp_line_subsegment && p_ref->mp_line_subsegment->is_connected( mp_obstacle ) ) {
          refs.push_back( p_ref );
        }
      }
    }
  }
  vector<Rule*> or_rules;
  for( unsigned int i=0; i< refs.size(); i++ ) {
    or_rules.push_back( new Rule( ATOM, refs[i] ) );
  }
  Rule* p_or_rule = new Rule( MOLECULE_OR, NULL, or_rules );
  vector<Rule*> neg_rules;
  neg_rules.push_back( p_or_rule );
  p_rule = new Rule( MOLECULE_NEG, NULL, neg_rules );
  return p_rule;  
}

string AvoidRelationFunction::get_name() {
  string name = "AVOID";
  if( mp_obstacle ){
    name += "(" + mp_obstacle->get_name() + ")";
  }
  return name;
}
