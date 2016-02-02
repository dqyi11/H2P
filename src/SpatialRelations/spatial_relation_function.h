#ifndef SPATIAL_RELATION_FUNC_H_
#define SPATIAL_RELATION_FUNC_H_

#include <utility>
#include <vector>
#include <string>
#include "rule.h"
#include "reference_frames.h"

namespace h2p {

  class SpatialRelationFunction {
  public:
    SpatialRelationFunction() {};
    virtual ~SpatialRelationFunction() {};

    virtual Rule* get_rule( ReferenceFrameSet* p_reference_frame_set ) = 0; 
    virtual Obstacle* get_primary_obstacle() = 0;
    virtual std::string get_name() = 0;
  };
}

#endif // SPATIAL_RELATION_FUNC_H_
