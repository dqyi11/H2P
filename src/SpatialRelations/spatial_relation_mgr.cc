#include "spatial_relation_mgr.h"

using namespace std;
using namespace h2p;

StringClass::StringClass( vector< std::string > string ) {
  m_string = string;
}

StringClass::~StringClass() {
  mp_reference_frames.clear();
}
  
string StringClass::get_name() {
  string name = "";
  for( unsigned int i=0; i<m_string.size(); i++ ) {
    if( i>0 ) {
      name += " ";
    }
    name += m_string[i];
  }
  return name;
}

void StringClass::init( ReferenceFrameSet* p_rfs ) {
  if( p_rfs ) {
    for( unsigned int i=0; i < m_string.size(); i++ ) {
      string id = m_string[i];
      ReferenceFrame* p_rf = p_rfs->get_reference_frame( id );
      if( p_rf ) {
        mp_reference_frames.push_back( p_rf );
      }
    }
  } 
}

bool contains( vector< string > string_set, string string_item ) {
  for( vector< string >::iterator it = string_set.begin();
       it != string_set.end(); it ++ ) {
    string current_string_item = (*it);
    if( current_string_item == string_item ) {
      return true;
    }
  }

  return false;
}

SpatialRelationMgr::SpatialRelationMgr( WorldMap* p_worldmap ) {
  mp_worldmap = p_worldmap;
  mp_functions.clear();
  mp_string_classes.clear();
  mp_rule = NULL;
  m_start_x = -1;
  m_start_y = -1;
  m_goal_x = -1;
  m_goal_y = -1;
}

SpatialRelationMgr::~SpatialRelationMgr() {
  mp_worldmap = NULL;
  mp_functions.clear();
  if( mp_rule ) {
    delete mp_rule;
    mp_rule = NULL;
  }
  for( vector<StringClass*>::iterator it = mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    StringClass* p_str_cls = (*it);
    delete p_str_cls;
    p_str_cls = NULL;
  }
  mp_string_classes.clear();
}

Rule* SpatialRelationMgr::get_rule( ReferenceFrameSet* p_reference_frame_set ) {
  Rule* p_rule;
  for( unsigned int i=0; i < mp_functions.size(); i++ ) {
    cout << "FUNC: " << mp_functions[i]->get_name() << endl;
    Rule* tmp_rule = mp_functions[i]->get_rule( p_reference_frame_set );
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

void SpatialRelationMgr::get_string_classes( ReferenceFrameSet* p_rfs  ) {
  /*
  mp_string_classes.clear();
  m_rules.clear();
  vector< vector< string > > string_set;
  
  if( p_rfs ) {
    StringGrammar* p_grammar = p_rfs->get_string_grammar( m_start_x, m_start_y, m_goal_x, m_goal_y );
    if( p_grammar ){
      vector< vector< string > > all_simple_strings = p_grammar->find_simple_strings(); 
      m_rules = get_rules( p_rfs );
      print_rules( m_rules );
      string_set = filter( all_simple_strings, m_rules );
        
      delete p_grammar;
      p_grammar=NULL;
    }   
  }
  for( vector< vector< string > >::iterator it = string_set.begin();
       it != string_set.end(); it ++ ) {
    vector< string > item = (*it);
    StringClass* p_class = new StringClass( item );
    p_class->init( p_rfs );
    mp_string_classes.push_back( p_class );
  }
  */
}


vector< vector< string > > SpatialRelationMgr::filter( vector< vector< string > > string_set, Rule* rule ) {
  vector< vector < string > > output_set;
  for( vector< vector< string > >::iterator it = string_set.begin();
       it != string_set.end(); it++ ) {
    vector< string > item = (*it);
    if( is_eligible( item, rule ) ) {
      output_set.push_back( item );
    }
  }
  return output_set;
}


bool SpatialRelationMgr::is_eligible( vector< string > string_item, Rule* rule ) {
  return false;
}
