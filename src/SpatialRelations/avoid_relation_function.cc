#include "avoid_relation_function.h"

using namespace std;
using namespace h2p;

AvoidRelationFunction::AvoidRelationFunction() {
  mp_obstacle = NULL;
}

AvoidRelationFunction::~AvoidRelationFunction() {
  mp_obstacle = NULL;
}

Rule* AvoidRelationFunction::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;
  return p_rule;  
}

string AvoidRelationFunction::get_name() {
  string name = "AVOID";
  if( mp_obstacle ){
    name += "(" + mp_obstacle->get_name() + ")";
  }
  return name;
}
