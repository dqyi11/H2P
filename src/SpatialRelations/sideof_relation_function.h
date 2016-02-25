#ifndef SIDEOF_RELATION_FUNC_H_
#define SIDEOF_RELATION_FUNC_H_

#include "obstacle.h"
#include "spatial_relation_function.h"

namespace h2p {

  typedef enum{
    SIDE_TYPE_UNKNOWN,
    SIDE_TYPE_LEFT,
    SIDE_TYPE_RIGHT,
    SIDE_TYPE_TOP,
    SIDE_TYPE_BOTTOM,
    NUM_SIDE_TYPES
  } side_type_t;

  class SideOfRelationFunction : public SpatialRelationFunction {
  public:
    SideOfRelationFunction( side_type_t side_type = SIDE_TYPE_UNKNOWN );
    virtual ~SideOfRelationFunction();

    void set_side_type( side_type_t side_type ) { m_type = side_type; }
    side_type_t get_side_type( void ) { return m_type; }

    virtual Rule* get_rule( ReferenceFrameSet* p_reference_frame_set ); 
    virtual Obstacle* get_primary_obstacle();
    virtual std::string get_name();
    virtual SpatialRelationFuncType get_type();

    Obstacle*   mp_obstacle;
    side_type_t m_type;
  };
}

#endif // SIDEOF_RELATION_FUNC_H_
