#ifndef SPATIAL_RELATION_MGR_H
#define SPATIAL_RELATION_MGR_H

#include <vector>
#include "worldmap.h"
#include "spatial_relation_function.h"

namespace h2p {

  enum SpatialRelationFuncType{
    SPATIAL_REL_UNKNOWN,
    SPATIAL_REL_IN_BETWEEN,
    SPATIAL_REL_LEFT_OF,
    SPATIAL_REL_RIGHT_OF,
    SPATIAL_REL_TOP_OF,
    SPATIAL_REL_BOTTOM_OF,
    SPATIAL_REL_AVOID
  };

  class SpatialRelationMgr {
  public:
    SpatialRelationMgr(WorldMap* p_worldmap);
    virtual ~SpatialRelationMgr();

    Rule* get_rule( ReferenceFrameSet* p_reference_frame_set );
    std::vector< std::string > get_spatial_relation_function_names();
  
    bool has_spatial_relation_function( std::string name );
    void remove_spatial_relation_function( std::string name );
 
    WorldMap* get_world_map() {  return mp_worldmap; }
    Obstacle* get_primary_obstacle();
 
    inline std::string typeToString( SpatialRelationFuncType type );
    inline SpatialRelationFuncType stringToType( std::string name );

    std::vector< std::vector< std::string > > filter( std::vector< std::vector< std::string > > string_set, Rule* rule );

    Rule*                                 mp_rule;
    std::vector<SpatialRelationFunction*> mp_functions; 
    WorldMap*                             mp_worldmap;

  };
}

#endif // SPATIAL_RELATION_MGR_H
