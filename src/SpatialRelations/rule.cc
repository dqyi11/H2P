#include "rule.h"

using namespace h2p;

Rule::Rule() {
  mp_reference_frame = NULL;
}

Rule::~Rule() {
  m_rule_sequence.clear();
  mp_reference_frame = NULL;
}
