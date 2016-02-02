#ifndef INBETWEEN_RELATION_FUNC_H_
#define INBETWEEN_RELATION_FUNC_H_

#include "obstacle.h"
#include "spatial_relation_function.h"

namespace h2p {

  class InBetweenRelationFunction : public SpatialRelationFunction {
  public:
    InBetweenRelationFunction();
    virtual ~InBetweenRelationFunction();

    virtual Rule* get_rule( ReferenceFrameSet* p_reference_frame_set ); 
    virtual Obstacle* get_primary_obstacle();
    virtual std::string get_name();

    std::vector<Obstacle*> mp_obstacles;
  };
}

#endif // INBETWEEN_RELATION_FUNC_H_
