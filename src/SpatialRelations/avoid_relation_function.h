#ifndef AVOID_RELATION_FUNC_H_
#define AVOID_RELATION_FUNC_H_

#include "obstacle.h"
#include "spatial_relation_function.h"

namespace h2p {

  class AvoidRelationFunction : public SpatialRelationFunction {
  public:
    AvoidRelationFunction();
    virtual ~AvoidRelationFunction();

    virtual Rule* get_rule( ReferenceFrameSet* p_reference_frame_set ); 
    virtual Obstacle* get_primary_obstacle();
    virtual std::string get_name();

    Obstacle* mp_obstacle;
  };
}

#endif // AVOID_RELATION_FUNC_H_
