#include "string_class_mgr.h"
#include "birrtstar.h"

using namespace std;
using namespace birrts;

StringClass::StringClass( vector< string > id_string ) : h2p::StringClass( id_string ) {
  m_cost = 0.0;
  mp_path = NULL;
}

StringClass::~StringClass() {
  m_string.clear();
  m_cost = 0.0;
  mp_path = NULL;
}

StringClassMgr::StringClassMgr( h2p::WorldMap* p_worldmap, h2p::StringGrammar* p_grammar ) : h2p::SpatialRelationMgr( p_worldmap ) {
  _p_grammar = p_grammar;
}

StringClassMgr::~StringClassMgr() {
  _p_grammar = NULL;
  //_classes.clear();
}

void StringClassMgr::import_path( Path* p_path ) { 
  vector< string > non_repeating_id_string = _p_grammar->get_non_repeating_form( p_path->m_string );
  if ( _p_grammar->is_valid_string( non_repeating_id_string ) == false ) {
    cout << "INVALID STRING " << endl;
  }
  StringClass* p_string_class = find_string_class( non_repeating_id_string );
  if( p_string_class ) {
    if( p_string_class->m_cost > p_path->m_cost ) {
      p_string_class->m_cost = p_path->m_cost;
      p_string_class->mp_path = p_path;      
    }
  }
  else {
    p_string_class = new StringClass( non_repeating_id_string ); 
    p_string_class->m_cost = p_path->m_cost;
    p_string_class->mp_path = p_path;
    //_classes.push_back(p_string_class);
    mp_string_classes.push_back( p_string_class );
  }
}

vector<Path*> StringClassMgr::export_paths() {

  vector<Path*> paths;
  for( vector<h2p::StringClass*>::iterator it= mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    h2p::StringClass* p_tmp_str_cls = (*it);
    StringClass* p_str_cls = dynamic_cast<StringClass*>( p_tmp_str_cls );
    paths.push_back( p_str_cls->mp_path );
  }
  return paths;
}

StringClass* StringClassMgr::find_string_class( vector< string > str ) {
  
  StringClass* p_string_class = NULL;
  for( vector<h2p::StringClass*>::iterator it= mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    h2p::StringClass* p_tmp_str_cls = (*it);
    StringClass* p_str_cls = dynamic_cast<StringClass*>( p_tmp_str_cls );
    if( p_str_cls->m_string.size() == str.size()) {
      bool identical = true;
      for( unsigned int j = 0; j < p_str_cls->m_string.size(); j ++) {
        if( p_str_cls->m_string[j] != str[j] ) {
          identical = false;
          break;
        }
      }
      if( identical ) {
        return p_str_cls;
      }
    }
  }
  return p_string_class;
}

vector< StringClass* >  StringClassMgr::merge() {
  vector< StringClass* > merged_classes;
  //std::cout << "NUM OF CLASSES " << _classes.size() << std::endl;
  for( vector<h2p::StringClass*>::iterator it= mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    h2p::StringClass* p_tmp_str_cls = (*it);
    StringClass* p_str_cls = dynamic_cast<StringClass*>( p_tmp_str_cls );
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
