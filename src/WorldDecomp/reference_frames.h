#ifndef REFERENCE_FRAMES_H
#define REFERENCE_FRAMES_H

#include <string>
#include "worldmap.h"
#include "string_grammar.h"

namespace h2p {

  static std::string CENTER_POINT_ID_CHARACTER = "C";

  typedef enum {
    STRING_GRAMMAR_TYPE,
    HOMOTOPIC_GRAMMAR_TYPE
  } grammar_type_t;

  class ReferenceFrame {
  public:
    ReferenceFrame( LineSubSegment* p_subseg );
    virtual ~ReferenceFrame();

    bool is_line_crossed( Point2D pos_a, Point2D pos_b );
    std::string get_name() { return m_name; } 

    std::string     m_name; 
    Segment2D       m_segment;
    bool            m_connect_to_cp;
    LineSubSegment* mp_line_subsegment;
    Point2D         m_mid_point;
  };

  class ReferenceFrameSet {

  public:
    ReferenceFrameSet();
    virtual ~ReferenceFrameSet();

    void init(int width, int height, std::vector< std::vector<Point2D> >& obstacles);
    void process( Obstacle* p_obstacle );
    StringGrammar* get_string_grammar( Point2D init, Point2D goal );
    StringGrammar* get_string_grammar( int init_x, int init_y, int goal_x, int goal_y );
    
    std::vector< std::string > get_string ( Point2D start, Point2D end, grammar_type_t type );
    std::vector< std::string > get_string ( std::vector<Point2D> points, grammar_type_t type );
    std::vector<ReferenceFrame*>& get_reference_frames() { return _reference_frames; }

    WorldMap* get_world_map() { return _p_world_map; }
    ReferenceFrame* get_reference_frame( std::string name );    
   
    void import_string_constraint( std::vector<std::string> id_string );
    void import_string_constraint( std::vector<Point2D> points, grammar_type_t type );
    bool is_constrained_substring( std::vector< std::string > sub_str, bool reverse );
    std::vector< std::vector< std::string > >& get_string_constraint() { return _string_constraint; }

  protected:
    StringGrammar* get_string_grammar( SubRegion* p_init, SubRegion* p_goal );
    bool is_eligible_substring( std::vector< std::string > substring, std::vector< std::string > ref_str, bool reverse );
  
    WorldMap*                                 _p_world_map;
    std::vector<ReferenceFrame*>              _reference_frames;
    std::vector< std::vector< std::string > > _string_constraint; 
  };

}

#endif // REFERENCE_FRAMES_H
