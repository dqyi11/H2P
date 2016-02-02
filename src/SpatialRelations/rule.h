#ifndef RULE_H_
#define RULE_H_

#include <vector>
#include "reference_frames.h"

namespace h2p {

  class Rule{
  public:
    Rule();
    virtual ~Rule();
  
    std::vector< Rule* > m_rule_sequence;
    ReferenceFrame* mp_reference_frame;
  };

}

#endif // RULE_H_
