#ifndef STRING_CLASS_MGR_H
#define STRING_CLASS_MGR_H

#include <vector>
#include "spatial_relation_mgr.h"
#include "string_grammar.h"

namespace birrts {

  class Path;

  class StringClass : public h2p::StringClass {
  public:
    StringClass( std::vector< std::string > id_string );
    virtual ~StringClass();
  
    double m_cost;
    Path*  mp_path;
  };

  class StringClassMgr : public h2p::SpatialRelationMgr {
  public:
    StringClassMgr(h2p::WorldMap* p_worldmap, h2p::StringGrammar* p_grammar);
    virtual ~StringClassMgr();

    void import_path( Path* p_path );
    std::vector<Path*> export_paths();
    void merge();
    StringClass* find_string_class( std::vector< std::string > str );
    std::vector< StringClass* >& get_string_classes() { return _classes; }  
    void export_grammar( std::string filename );
  protected:
    h2p::StringGrammar* _p_grammar;
    std::vector< StringClass* > _classes;
  };

}

#endif /* STRING_CLASS_MGR_H */
