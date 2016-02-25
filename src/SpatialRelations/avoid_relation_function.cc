#include "avoid_relation_function.h"

using namespace std;
using namespace h2p;

AvoidRelationFunction::AvoidRelationFunction() {
  mp_child_func = NULL;
}

AvoidRelationFunction::~AvoidRelationFunction() {
  if( mp_child_func ) {
    delete mp_child_func;
    mp_child_func = NULL;
  }
}

Obstacle* AvoidRelationFunction::get_primary_obstacle() {
  if( mp_child_func ) {
    return mp_child_func->get_primary_obstacle();
  }
  return NULL;
}

Rule* AvoidRelationFunction::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;
  vector<ReferenceFrame*> refs;
  
  Rule* p_child_rule = NULL;
  if( mp_child_func && p_reference_frame_set ) {
    // check whether there is a reference frame connecting to reference frames
    p_child_rule = mp_child_func->get_rule( p_reference_frame_set );
  }
  vector<Rule*> neg_rules;
  neg_rules.push_back( p_child_rule );
  p_rule = new Rule( MOLECULE_NEG, NULL, neg_rules );
  return p_rule;  
}

string AvoidRelationFunction::get_name() {
  string name = "AVOID";
  if( mp_child_func ){
    name += "(" + mp_child_func->get_name() + ")";
  }
  return name;
}

SpatialRelationFuncType AvoidRelationFunction::get_type() {
  return SPATIAL_REL_FUNC_AVOID;
}
