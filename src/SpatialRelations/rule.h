#ifndef RULE_H_
#define RULE_H_

#include <utility>
#include <vector>
#include "reference_frames.h"

namespace h2p {

  enum RuleType{
    EMPTY,
    ATOM,
    MOLECULE_OR,
    MOLECULE_AND,
    MOLECULE_NEG
  };

  class Rule{
  public:
    Rule(RuleType type=EMPTY, ReferenceFrame* ref=NULL, std::vector<Rule*> set=std::vector<Rule*>() );
    virtual ~Rule();

    bool is_ok( std::vector< std::string > id_str );
    bool get_reference_frames(std::vector<ReferenceFrame*>& pos_refs, std::vector<ReferenceFrame*>& neg_refs ); 
  
    std::vector< Rule* > m_rule_set;
    ReferenceFrame* mp_reference_frame;
    RuleType m_type;
  };

}

#endif // RULE_H_
