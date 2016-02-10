#include "rule.h"

using namespace h2p;

Rule::Rule( RuleType type, ReferenceFrame* ref, std::vector<Rule*> set ) : m_type( type ), mp_reference_frame( ref ), m_rule_set( set ) {

} 

Rule::~Rule() {
  m_rule_set.clear();
  mp_reference_frame = NULL;
}

bool Rule::is_ok( std::vector< std::string > id_str ) {
  return false;
}
