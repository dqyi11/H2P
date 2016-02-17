#include "spatial_relation_string_class_mgr.h"

using namespace std;
using namespace h2p;

StringClass::StringClass( vector< std::string > string_id ) {
  m_string = string_id;
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

SpatialRelationStringClassMgr::SpatialRelationStringClassMgr( WorldMap* p_worldmap )
  : SpatialRelationMgr(p_worldmap) {

  mp_string_classes.clear();
  m_start_x = -1;
  m_start_y = -1;
  m_goal_x = -1;
  m_goal_y = -1;
}

SpatialRelationStringClassMgr::~SpatialRelationStringClassMgr() {

  for( vector<StringClass*>::iterator it = mp_string_classes.begin();
       it != mp_string_classes.end(); it++ ) {
    StringClass* p_str_cls = (*it);
    delete p_str_cls;
    p_str_cls = NULL;
  }
  mp_string_classes.clear();
}

vector< vector< string > > SpatialRelationStringClassMgr::get_strings( ReferenceFrameSet* p_rfs  ) {
  vector< vector< string > > string_set;

  if( p_rfs ) {
    StringGrammar* p_grammar = p_rfs->get_string_grammar( m_start_x, m_start_y, m_goal_x, m_goal_y );
    if( p_grammar ){
      vector< vector< string > > all_simple_strings = p_grammar->find_simple_strings();
      string_set = filter( all_simple_strings, mp_rule );

      delete p_grammar;
      p_grammar=NULL;
    }
  }
  return string_set;
}

void SpatialRelationStringClassMgr::get_string_classes( ReferenceFrameSet* p_rfs  ) {

  cout << "SpatialRelationMgr::get_string_classes" << endl;
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
