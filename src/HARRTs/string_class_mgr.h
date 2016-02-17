#ifndef STRING_CLASS_MGR_H
#define STRING_CLASS_MGR_H

#include <vector>
#include "spatial_relation_mgr.h"
#include "string_grammar.h"

namespace birrts {

  class Path;

  class StringClass {
  public:
    StringClass( std::vector< std::string > id_string );
    virtual ~StringClass();

    std::string get_name();
    void init( h2p::ReferenceFrameSet* p_rfs );

    std::vector< std::string >     m_string;
    std::vector< h2p::ReferenceFrame* > mp_reference_frames;

    float m_cost;
    Path* mp_path;
  };

  class StringClassMgr : public h2p::SpatialRelationMgr {
  public:
    StringClassMgr(h2p::WorldMap* p_worldmap, h2p::StringGrammar* p_grammar = NULL);
    virtual ~StringClassMgr();

    h2p::StringGrammar* get_grammar() { return _p_grammar; }
    void set_grammar(h2p::StringGrammar* p_grammar) { _p_grammar = p_grammar; }

    bool import_path( Path* p_path );
    std::vector<Path*> export_paths();
    std::vector< StringClass* >  merge();

    std::vector< std::vector< std::string > > get_strings( h2p::ReferenceFrameSet* p_rfs );
    void get_string_classes( h2p::ReferenceFrameSet* p_rfs  );

    StringClass* find_string_class( std::vector< std::string > id_str );
    void export_grammar( std::string filename );

    void print(std::vector<std::string> id_str );

    h2p::StringGrammar* _p_grammar;
    std::vector<StringClass*> mp_string_classes;
    int m_start_x;
    int m_start_y;
    int m_goal_x;
    int m_goal_y;
  };

}

#endif /* STRING_CLASS_MGR_H */
