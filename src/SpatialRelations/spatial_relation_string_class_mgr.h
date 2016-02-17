#ifndef SPATIAL_RELATION_STRING_CLASS_MGR_H
#define SPATIAL_RELATION_STRING_CLASS_MGR_H

#include "spatial_relation_mgr.h"

namespace h2p {

  class StringClass {
  public:
    StringClass( std::vector< std::string > id_string );
    virtual ~StringClass();

    std::string get_name();
    void init( ReferenceFrameSet* p_rfs );

    std::vector< std::string >     m_string;
    std::vector< ReferenceFrame* > mp_reference_frames;
  };

  class SpatialRelationStringClassMgr : public SpatialRelationMgr {
  public:
    SpatialRelationStringClassMgr(  WorldMap* p_worldmap  );
    virtual ~SpatialRelationStringClassMgr();

    std::vector< std::vector< std::string > > get_strings( ReferenceFrameSet* p_rfs );
    void get_string_classes( ReferenceFrameSet* p_rfs  );

    std::vector<StringClass*>             mp_string_classes;
    int m_start_x;
    int m_start_y;
    int m_goal_x;
    int m_goal_y;
  };

}

#endif // SPATIAL_RELATION_STRING_CLASS_MGR_H
