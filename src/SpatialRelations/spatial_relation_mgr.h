#ifndef SPATIAL_RELATION_MGR_H
#define SPATIAL_RELATION_MGR_H

#include <vector>
#include "worldmap.h"
#include "spatial_relation_function.h"

namespace h2p {

  class StringClass {
  public:
    StringClass( std::vector< std::string > string );
    virtual ~StringClass();
  
    std::string get_name();
    void init( ReferenceFrameSet* p_rfs );
    
    std::vector< std::string >     m_string;
    std::vector< ReferenceFrame* > mp_reference_frames;
  };

  class SpatialRelationMgr {
  public:
    SpatialRelationMgr(WorldMap* p_worldmap);
    virtual ~SpatialRelationMgr();

    Rule* get_rule( ReferenceFrameSet* p_reference_frame_set );
    std::vector< std::string > get_spatial_relation_function_names();
  
    bool has_spatial_relation_function( std::string name );
    void remove_spatial_relation_function( std::string name );
  
    void get_string_classes( ReferenceFrameSet* p_rfs  );
 
    WorldMap* get_world_map() {  return mp_worldmap; }
    Obstacle* get_primary_obstacle();

    std::vector< std::vector< std::string > > filter( std::vector< std::vector< std::string > > string_set, Rule* rule );

    std::vector<StringClass*>             mp_string_classes;
    Rule*                                 mp_rule;

    std::vector<SpatialRelationFunction*> mp_functions; 
    WorldMap*                             mp_worldmap;
    int m_start_x;
    int m_start_y;
    int m_goal_x;
    int m_goal_y;
  };
}

#endif // SPATIAL_RELATION_MGR_H
