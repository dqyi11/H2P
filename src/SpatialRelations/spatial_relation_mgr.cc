#include "inbetween_relation_function.h"
#include "sideof_relation_function.h"
#include "avoid_relation_function.h"
#include "spatial_relation_mgr.h"

using namespace std;
using namespace h2p;

SpatialRelationMgr::SpatialRelationMgr( WorldMap* p_worldmap ) {
  mp_worldmap = p_worldmap;
  mp_functions.clear();
  mp_rule = NULL;
}

SpatialRelationMgr::~SpatialRelationMgr() {
  mp_worldmap = NULL;
  mp_functions.clear();
  if( mp_rule ) {
    delete mp_rule;
    mp_rule = NULL;
  }
}

Rule* SpatialRelationMgr::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule = NULL;
  vector< Rule* > tmp_rules;
  for( unsigned int i=0; i < mp_functions.size(); i++ ) {
    cout << "FUNC: " << mp_functions[i]->get_name() << endl;
    tmp_rules.push_back( mp_functions[i]->get_rule( p_reference_frame_set ) );
  }
  if( tmp_rules.size() > 1 ) {
    p_rule = new Rule( MOLECULE_AND, NULL, tmp_rules );
  }
  else if( tmp_rules.size() == 1 ) {
    p_rule = tmp_rules[0];
  }
  return p_rule;
}

vector< string > SpatialRelationMgr::get_spatial_relation_function_names() {
  vector< string > names;
  for(unsigned int i=0; i < mp_functions.size(); i++) {
    SpatialRelationFunction* p_func = mp_functions[i];
    if( p_func ) {
      names.push_back( p_func->get_name() );
    }
  }
  return names;
}

bool SpatialRelationMgr::has_spatial_relation_function( string name ) {
  for(unsigned int i=0; i < mp_functions.size(); i++) {
    SpatialRelationFunction* p_func = mp_functions[i];
    if( p_func ) {
      if( p_func->get_name() == name ) {
        return true;
      }
    }
  }
  return false;
}

void SpatialRelationMgr::remove_spatial_relation_function( string name ) {

  for( vector<SpatialRelationFunction*>::iterator it = mp_functions.begin();
       it != mp_functions.end(); /* it ++ */ ) { 
    SpatialRelationFunction* p_func = (*it);
    if( p_func && ( p_func->get_name() == name ) ) {
      mp_functions.erase( it );
    }
    else {
      ++ it;
    }
  }
}

vector< vector< string > > SpatialRelationMgr::filter( vector< vector< string > > string_set, Rule* rule ) {
  vector< vector < string > > output_set;
  for( vector< vector< string > >::iterator it = string_set.begin();
       it != string_set.end(); it++ ) {
    vector< string > item = (*it);
    bool ok = true;
    if( rule ) {
      int idx = 0;
      ok = rule->scan(item , idx);
    }
    if( ok ) {
      output_set.push_back( item );
    }
  }
  return output_set;
}

Obstacle* SpatialRelationMgr::get_primary_obstacle() {
  Obstacle* p_obstacle = NULL;
  if( mp_functions.size() > 0 ) {
    p_obstacle = mp_functions[0]->get_primary_obstacle(); 
  }
  return p_obstacle;
}

string SpatialRelationMgr::typeToString( SpatialRelationType type ) {
  switch( type ) {
    case SPATIAL_REL_IN_BETWEEN:
      return "in_between";
    case SPATIAL_REL_LEFT_OF:
      return "left_of";
    case SPATIAL_REL_RIGHT_OF:
      return "right_of";
    case SPATIAL_REL_TOP_OF:
      return "top_of";
    case SPATIAL_REL_BOTTOM_OF:
      return "bottom_of";
    case SPATIAL_REL_AVOID:
      return "avoid";
    case SPATIAL_REL_UNKNOWN:
    default:
      return "unknown";
  }
  return "unknown";
}

SpatialRelationType SpatialRelationMgr::stringToType( string name ) {
  if( name == "in_between" ) {
    return SPATIAL_REL_IN_BETWEEN;
  }
  else if( name == "left_of" ) {
    return SPATIAL_REL_LEFT_OF;
  }
  else if( name == "right_of" ) {
    return SPATIAL_REL_RIGHT_OF;
  }
  else if( name == "top_of" ) {
    return SPATIAL_REL_TOP_OF;
  }
  else if( name == "bottom_of" ) {
    return SPATIAL_REL_BOTTOM_OF;
  }
  else if( name == "avoid" ) {
    return SPATIAL_REL_AVOID;
  }
  return SPATIAL_REL_UNKNOWN;
}

SpatialRelationFunction*  SpatialRelationMgr::add_function( SpatialRelationType type, std::vector<Obstacle*> obstacles ) {
  SpatialRelationFunction* p_func = create_function( type, obstacles );
  if( p_func ) {
     mp_functions.push_back( p_func );
     return p_func;
  }
  return NULL;
}

SpatialRelationFunction* SpatialRelationMgr::create_function( SpatialRelationType type, std::vector<Obstacle*> obstacles ) {
  switch( type ) {
    case SPATIAL_REL_IN_BETWEEN:
      if( obstacles.size() == 2 ) {
        InBetweenRelationFunction* p_func = new InBetweenRelationFunction();
        p_func->mp_obstacles = obstacles;
        return p_func;
      }
      return NULL;
    case SPATIAL_REL_LEFT_OF:
      if( obstacles.size() == 1 ) {
        SideOfRelationFunction* p_func = new SideOfRelationFunction(SIDE_TYPE_LEFT);
        p_func->mp_obstacle = obstacles[0];
        return p_func;
      }
      return NULL;
    case SPATIAL_REL_RIGHT_OF:
      if( obstacles.size() == 1 ) {
        SideOfRelationFunction* p_func = new SideOfRelationFunction(SIDE_TYPE_RIGHT);
        p_func->mp_obstacle = obstacles[0];
        return p_func;
      }
      return NULL;
    case SPATIAL_REL_TOP_OF:
      if( obstacles.size() == 1 ) {
        SideOfRelationFunction* p_func = new SideOfRelationFunction(SIDE_TYPE_TOP);
        p_func->mp_obstacle = obstacles[0];
        return p_func;
      }
      return NULL;
    case SPATIAL_REL_BOTTOM_OF:
      if( obstacles.size() == 1 ) {
        SideOfRelationFunction* p_func = new SideOfRelationFunction(SIDE_TYPE_BOTTOM);
        p_func->mp_obstacle = obstacles[0];
        return p_func;
      }
      return NULL;
    case SPATIAL_REL_AVOID:
    case SPATIAL_REL_UNKNOWN:
    default:
      return NULL;
  }
  return NULL;
}

SpatialRelationFunction*  SpatialRelationMgr::add_avoid_function( SpatialRelationFunction* p_func ) {

  if( p_func ) {
    AvoidRelationFunction* p_avoid_func = new AvoidRelationFunction();
    p_avoid_func->mp_child_func = p_func;
    mp_functions.push_back( p_avoid_func );
    return p_avoid_func;
  }
  return NULL;
}

SpatialRelationFunction*  SpatialRelationMgr::add_function( SpatialRelationFunction* p_func ) {
    if( p_func ) {
      mp_functions.push_back( p_func );
      return p_func;
    }
    return NULL;
}
