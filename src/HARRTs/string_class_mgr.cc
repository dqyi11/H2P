#include <limits>
#include "string_class_mgr.h"
#include "birrtstar.h"

using namespace std;
using namespace birrts;

StringClass::StringClass( vector< string > id_string )  {
  m_string = id_string;
  m_cost = numeric_limits<float>::max() ;
  mp_path = NULL;
}

StringClass::~StringClass() {
  m_string.clear();
  mp_reference_frames.clear();
  m_cost = 0.0;
  mp_path = NULL;
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

void StringClass::init( h2p::ReferenceFrameSet* p_rfs ) {
  if( p_rfs ) {
    for( unsigned int i=0; i < m_string.size(); i++ ) {
      string id = m_string[i];
      h2p::ReferenceFrame* p_rf = p_rfs->get_reference_frame( id );
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

StringClassMgr::StringClassMgr( h2p::WorldMap* p_worldmap, h2p::StringGrammar* p_grammar ) : h2p::SpatialRelationMgr( p_worldmap ) {
  mp_string_classes.clear();
  m_start_x = -1;
  m_start_y = -1;
  m_goal_x = -1;
  m_goal_y = -1;
  _p_grammar = p_grammar;
}

StringClassMgr::~StringClassMgr() {
  _p_grammar = NULL;
  for( vector<StringClass*>::iterator it = mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    StringClass* p_str_cls = (*it);
    delete p_str_cls;
    p_str_cls = NULL;
  }
  mp_string_classes.clear();
}

bool StringClassMgr::import_path( Path* p_path ) {
  //cout << "add path " << p_path->m_cost << endl;
  vector< string > non_repeating_id_string = _p_grammar->get_non_repeating_form( p_path->m_string );
  //print( p_path->m_string );
  //print( non_repeating_id_string );
  if ( _p_grammar->is_valid_string( non_repeating_id_string ) == false ) {
    //cout << "INVALID STRING " << endl;
    return false;
  }
  StringClass* p_string_class = find_string_class( non_repeating_id_string );
  if( p_string_class ) {
    //cout << "STRING CLASS VAL " << p_string_class->m_cost << endl;
    if( p_string_class->mp_path==NULL || p_string_class->m_cost > p_path->m_cost ) {
      p_string_class->m_cost = p_path->m_cost;
      p_string_class->mp_path = p_path;    
      //cout << "ADDING TO EXISTING STRING CLASS " << p_string_class->get_name() << endl;
    }
    else{
      //cout << "NOT A BETTER PATH" << endl;
    }
  }

  return true;
}

vector<Path*> StringClassMgr::export_paths() {

  vector<Path*> paths;
  for( vector<StringClass*>::iterator it= mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    StringClass* p_str_cls = (*it);
    if( p_str_cls->mp_path ) {
      paths.push_back( p_str_cls->mp_path );
    }
  }
  return paths;
}

StringClass* StringClassMgr::find_string_class( vector< string > id_str ) {
  
  for( vector<StringClass*>::iterator it= mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    StringClass* p_str_cls = (*it);
    if( p_str_cls->m_string.size() == id_str.size()) {
      bool identical = true;
      for( unsigned int j = 0; j < p_str_cls->m_string.size(); j ++) {
        if( p_str_cls->m_string[j] != id_str[j] ) {
          identical = false;
          break;
        }
      }
      if( identical ) {
        return p_str_cls;
      }
    }
  }
  return NULL;
}

vector< StringClass* > StringClassMgr::merge() {
  vector< StringClass* > merged_classes;
  //std::cout << "NUM OF CLASSES " << _classes.size() << std::endl;
  for( vector<StringClass*>::iterator it= mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    StringClass* p_str_cls = (*it);
    if( merged_classes.size() == 0 ) {
      merged_classes.push_back( p_str_cls );
    }
    else {
      //std::cout << "MERGE CLASS SIZE " << merged_classes.size() << std::endl;
      bool found_equivalence = false;
      for( unsigned int j = 0; j < merged_classes.size(); j++) {
        StringClass* str_class_in_mer = merged_classes[j];
        //std::cout << "COMPARE [" << str_class->get_name() << "] and [" << str_class_in_mer->get_name() << "]" << std::endl;
        if( _p_grammar->is_equivalent( str_class_in_mer->m_string, p_str_cls->m_string ) ) {
          if( str_class_in_mer->m_string.size() < p_str_cls->m_string.size() ) {
            str_class_in_mer->m_string = p_str_cls->m_string;
          }
          if ( str_class_in_mer->m_cost > p_str_cls->m_cost ) {
            str_class_in_mer->m_cost = p_str_cls->m_cost;
            str_class_in_mer->mp_path = p_str_cls->mp_path;
          }
          found_equivalence = true;
          break;
        }
      }
      if( found_equivalence == false) {
          merged_classes.push_back( p_str_cls );
      }
    }
  }

  return merged_classes;
}
  
void StringClassMgr::export_grammar( string filename ) {
  if( _p_grammar ) {
    _p_grammar->output( filename );
  }
}

vector< vector< string > > StringClassMgr::get_strings( h2p::ReferenceFrameSet* p_rfs  ) {
  vector< vector< string > > string_set;

  if( p_rfs ) {
    h2p::StringGrammar* p_grammar = p_rfs->get_string_grammar( m_start_x, m_start_y, m_goal_x, m_goal_y );
    if( p_grammar ){
      vector< vector< string > > all_simple_strings = p_grammar->find_simple_strings();
      string_set = filter( all_simple_strings, mp_rule );

      delete p_grammar;
      p_grammar=NULL;
    }
  }
  return string_set;
}

void StringClassMgr::get_string_classes( h2p::ReferenceFrameSet* p_rfs ) {

  cout << "StringClassMgr::get_string_classes" << endl;
  vector< vector< string > > string_set = get_strings( p_rfs );
  mp_string_classes.clear();
  for( vector< vector< string > >::iterator it = string_set.begin();
       it != string_set.end(); it ++ ) {
    vector< string > item = (*it);
    StringClass* p_class = new StringClass( item );
    p_class->init( p_rfs );
    mp_string_classes.push_back( p_class );
    p_rfs->import_string_constraint(item);

  }
}

void StringClassMgr::print(vector<string> id_str ) {
  for( unsigned int i=0; i<id_str.size(); i++ ) {
      cout << id_str[i] << " ";
  }
  cout << endl;
}
