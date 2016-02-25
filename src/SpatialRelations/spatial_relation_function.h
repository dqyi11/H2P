#ifndef SPATIAL_RELATION_FUNC_H_
#define SPATIAL_RELATION_FUNC_H_

#include <utility>
#include <vector>
#include <string>
#include "rule.h"
#include "reference_frames.h"

namespace h2p {

  enum SpatialRelationFuncType{
    SPATIAL_REL_FUNC_UNKNOWN,
    SPATIAL_REL_FUNC_IN_BETWEEN,
    SPATIAL_REL_FUNC_SIDE_OF,
    SPATIAL_REL_FUNC_AVOID
  };

  class SpatialRelationFunction {
  public:
    SpatialRelationFunction() {};
    virtual ~SpatialRelationFunction() {};

    virtual Rule* get_rule( ReferenceFrameSet* p_reference_frame_set ) = 0; 
    virtual Obstacle* get_primary_obstacle() = 0;
    virtual std::string get_name() = 0;
    virtual SpatialRelationFuncType get_type() = 0;
  };
}

#endif // SPATIAL_RELATION_FUNC_H_
