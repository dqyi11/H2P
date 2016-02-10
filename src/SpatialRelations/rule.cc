#include "rule.h"

using namespace std;
using namespace h2p;

Rule::Rule( RuleType type, ReferenceFrame* ref, std::vector<Rule*> set ) : m_type( type ), mp_reference_frame( ref ), m_rule_set( set ) {

} 

Rule::~Rule() {
  m_rule_set.clear();
  mp_reference_frame = NULL;
}

bool Rule::get_reference_frames( vector<ReferenceFrame*>& pos_refs, vector<ReferenceFrame*>& neg_refs ) {
  vector<ReferenceFrame*> tmp_pos_refs, tmp_neg_refs;
  if(m_type==STAR) {
    return false;
  }
  else if(m_type==ATOM) {
    if(mp_reference_frame) {
      pos_refs.push_back(mp_reference_frame);
    } 
  }
  else if(m_type==MOLECULE_OR || m_type==MOLECULE_AND) {
    for( vector<Rule*>::iterator it = m_rule_set.begin();
         it != m_rule_set.end(); it++ ) {
      Rule* p_rule = (*it);
      if( p_rule ) {
        p_rule->get_reference_frames( pos_refs, neg_refs );
      }
    } 
  }
  else if(m_type==MOLECULE_NEG) {
    for( vector<Rule*>::iterator it = m_rule_set.begin();
         it != m_rule_set.end(); it++ ) {
      Rule* p_rule = (*it);
      if( p_rule ) {
        p_rule->get_reference_frames( neg_refs, pos_refs );
      }
    } 
  }
  return true;
}

bool Rule::scan( std::vector< std::string > id_str, int& curr_idx ) {
  if(m_type==STAR) {
    return true;
  }
  else if(m_type==ATOM) {
    for( unsigned int i=curr_idx; i < id_str.size(); i++ ) {
      if( mp_reference_frame->get_name() == id_str[i] ) {
        curr_idx = i+1;
        return true;
      }
    }
    return false;
  }
  else if(m_type==MOLECULE_OR) {
    for( vector<Rule*>::iterator it = m_rule_set.begin();
         it != m_rule_set.end(); it ++ ) {
      Rule* p_rule = (*it);
      int tmp_curr_idx = curr_idx;
      if( p_rule->scan( id_str, tmp_curr_idx ) ) {
        return true;
      }
    } 
    return false;
  }
  else if(m_type==MOLECULE_AND) {
    for( vector<Rule*>::iterator it = m_rule_set.begin();
         it != m_rule_set.end(); it ++ ) {
      Rule* p_rule = (*it);
      if( curr_idx >= id_str.size() ) {
        return false;
      }
      if( p_rule ) {
         if( false == p_rule->scan( id_str, curr_idx ) ) {
           return false;
         }
      }
    }
    return true;
  }
  else if(m_type==MOLECULE_NEG) {
    if( m_rule_set[0]->scan( id_str, curr_idx ) ) {
      return false;
    }
    else {
      return true;
    }
  } 
 
  return false;
}
