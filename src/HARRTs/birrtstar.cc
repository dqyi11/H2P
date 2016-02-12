#include <limits>
#include <iostream>
#include <fstream>

#include "birrtstar.h"

#define OBSTACLE_THRESHOLD 200

using namespace std;
using namespace h2p;
using namespace birrts;

BIRRTNode::BIRRTNode(POS2D pos) {
  m_pos = pos;
  m_cost = 0.0;
  mp_parent = NULL;
  m_child_nodes.clear();
  m_substring.clear();
}

bool BIRRTNode::operator==(const BIRRTNode &other) {
  return m_pos==other.m_pos;
}

void BIRRTNode::clear_string() {
  m_substring.clear();
}

void BIRRTNode::append_to_string( vector< string > ids ) {
  for( unsigned int i = 0; i < ids.size(); i ++ ) {
    std::string id = ids[i];
    m_substring.push_back( id );
  }
}

Path::Path(POS2D start, POS2D goal) {
  m_start = start;
  m_goal = goal;
  m_cost = 0.0;

  m_way_points.clear();
  m_string.clear();
}

Path::~Path() {
  m_cost = 0.0;
}

void Path::append_waypoints( std::vector<POS2D> waypoints, bool reverse ) {
  if ( reverse ) {
    for( std::vector<POS2D>::reverse_iterator itr = waypoints.rbegin();
         itr != waypoints.rend(); itr++ ) {
      POS2D pos = (*itr);
      m_way_points.push_back( pos );
    }
  }
  else {
    for( std::vector<POS2D>::iterator it = waypoints.begin();
         it != waypoints.end(); it++ ) {
      POS2D pos = (*it);
      m_way_points.push_back( pos );
    }
  }
}

void Path::append_substring( std::vector< std::string > ids, bool reverse ) {
  if ( reverse ) {
    for( std::vector< std::string >::reverse_iterator itr = ids.rbegin();
         itr != ids.rend(); itr++ ) {
      std::string str = (*itr);
      m_string.push_back( str );
    }
  }
  else {
    for( std::vector< std::string >::iterator it = ids.begin();
         it != ids.end(); it++ ) {
      std::string str = (*it);
      m_string.push_back( str );
    }
  }
}

BIRRTstar::BIRRTstar( int width, int height, int segment_length ) {

  _sampling_width = width;
  _sampling_height = height;
  _segment_length = segment_length;
  _p_st_root = NULL;
  _p_gt_root = NULL;
  _reference_frames = NULL;
  _run_type = RUN_BOTH_TREES_TYPE;

  _grammar_type = STRING_GRAMMAR_TYPE;
  _p_st_kd_tree = new KDTree2D( std::ptr_fun(tac) );
  _p_gt_kd_tree = new KDTree2D( std::ptr_fun(tac) );

  _range = (_sampling_width > _sampling_height) ? _sampling_width:_sampling_height;
  _st_ball_radius = _range;
  _gt_ball_radius = _range;
  _obs_check_resolution = 1;
  _current_iteration = 0;
 
  _theta = 10;

  _pp_cost_distribution = NULL;
  _p_string_class_mgr = NULL;

  _pp_map_info = new int*[_sampling_width];
  for(int i=0;i<_sampling_width;i++) {
    _pp_map_info[i] = new int[_sampling_height];
    for(int j=0;j<_sampling_height;j++) {
      _pp_map_info[i][j] = 255;
    }
  }

  _st_nodes.clear();
  _gt_nodes.clear();
}

BIRRTstar::~BIRRTstar() {
  if(_p_st_kd_tree) {
    delete _p_st_kd_tree;
    _p_st_kd_tree = NULL;
  }
  if(_p_gt_kd_tree) {
    delete _p_gt_kd_tree;
    _p_gt_kd_tree = NULL;
  }

  if(_pp_map_info) {
    for(int i=0;i<_sampling_width;i++) {
      delete _pp_map_info[i];
      _pp_map_info[i] = NULL;
    }
    delete _pp_map_info;
    _pp_map_info = NULL;
  }
}

bool BIRRTstar::init( POS2D start, POS2D goal, COST_FUNC_PTR p_func, double** pp_cost_distribution, grammar_type_t grammar_type ) {
  if( _p_st_root ) {
    delete _p_st_root;
    _p_st_root = NULL;
  }
  if( _p_gt_root ) {
    delete _p_gt_root;
    _p_gt_root = NULL;
  }
  if( _p_string_class_mgr ) {
    delete _p_string_class_mgr;
    _p_string_class_mgr = NULL;
  }
 
  if (_reference_frames == NULL) {
    return false;
  }

  _start = start;
  _goal = goal;
  _p_cost_func = p_func;

  if(pp_cost_distribution) {
    if(_pp_cost_distribution == NULL) {
      _pp_cost_distribution = new double*[_sampling_width];
      for(int i=0;i<_sampling_width;i++) {
        _pp_cost_distribution[i] = new double[_sampling_height];
      }
    }
    for(int i=0;i<_sampling_width;i++) {
      for(int j=0;j<_sampling_height;j++) {
        _pp_cost_distribution[i][j] = pp_cost_distribution[i][j];
      }
    }
  }
  
  std::cout << "Init grammar ... " << std::endl; 
  Point2D start_point( _start[0], _start[1] );
  Point2D goal_point( _goal[0], _goal[1] );
  set_grammar_type(grammar_type);
  _string_grammar = _reference_frames->get_string_grammar( start_point, goal_point );
  
  cout << "Init String Class Mgr ... " << endl;
  _p_string_class_mgr = new StringClassMgr( _reference_frames->get_world_map(), _string_grammar );

  cout << "Init st_tree.." << endl;
  KDNode2D st_root( start );
  _p_st_root = new BIRRTNode( start );
  _st_nodes.push_back(_p_st_root);
  st_root.setBIRRTNode(_p_st_root);
  _p_st_kd_tree->insert( st_root );
  
  cout << "Init gt_tree.." << endl;
  KDNode2D gt_root( goal );
  _p_gt_root = new BIRRTNode( goal );
  _gt_nodes.push_back(_p_gt_root);
  gt_root.setBIRRTNode(_p_gt_root);
  _p_gt_kd_tree->insert( gt_root );

  _current_iteration = 0;

}

void BIRRTstar::load_map( int** pp_map ) {
  for(int i=0;i<_sampling_width;i++) {
    for(int j=0;j<_sampling_height;j++) {
      _pp_map_info[i][j] = pp_map[i][j];
    }
  }
}

POS2D BIRRTstar::_sampling() {
  double x = rand();
  double y = rand();
  int int_x = x * ((double)(_sampling_width)/RAND_MAX);
  int int_y = y * ((double)(_sampling_height)/RAND_MAX);

  POS2D m(int_x,int_y);
  return m;
}

POS2D BIRRTstar::_steer( POS2D pos_a, POS2D pos_b ) {
  POS2D new_pos( pos_a[0], pos_a[1] );
  double delta[2];
  delta[0] = pos_a[0] - pos_b[0];
  delta[1] = pos_a[1] - pos_b[1];
  double delta_len = sqrt(delta[0]*delta[0]+delta[1]*delta[1]);

  if (delta_len > _segment_length) {
    double scale = _segment_length / delta_len;
    delta[0] = delta[0] * scale;
    delta[1] = delta[1] * scale;

    new_pos.setX( pos_b[0]+delta[0] );
    new_pos.setY( pos_b[1]+delta[1] );
  }
  return new_pos;
}

bool BIRRTstar::_is_in_obstacle( POS2D pos ) {
  int x = (int)pos[0];
  int y = (int)pos[1];
  if( _pp_map_info[x][y] < 255 ) {
    return true;
  }
  return false;
}


bool BIRRTstar::_is_obstacle_free( POS2D pos_a, POS2D pos_b ) {
  if ( pos_a == pos_b ) {
    return true;
  }
  int x_dist = pos_a[0] - pos_b[0];
  int y_dist = pos_a[1] - pos_b[1];

  if( x_dist == 0 && y_dist == 0) {
    return true;
  }

  float x1 = pos_a[0];
  float y1 = pos_a[1];
  float x2 = pos_b[0];
  float y2 = pos_b[1];

  const bool steep = ( fabs(y2 - y1) > fabs(x2 - x1) );
  if ( steep ) {
    swap( x1, y1 );
    swap( x2, y2 );
  }

  if ( x1 > x2 ) {
    swap( x1, x2 );
    swap( y1, y2 );
  }

  const float dx = x2 - x1;
  const float dy = fabs( y2 - y1 );

  float error = dx / 2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  int y = (int)y1;

  const int maxX = (int)x2;

  for(int x=(int)x1; x<maxX; x++) {
    if(steep) {
      if ( y>=0 && y<_sampling_width && x>=0 && x<_sampling_height ) {
        if ( _pp_map_info[y][x] < OBSTACLE_THRESHOLD ) {
          return false;
        }
      }
    }
    else {
      if ( x>=0 && x<_sampling_width && y>=0 && y<_sampling_height ) {
        if ( _pp_map_info[x][y] < OBSTACLE_THRESHOLD ) {
          return false;
        }
      }
    }

    error -= dy;
    if(error < 0) {
      y += ystep;
      error += dx;
    }
  }
  return true;
}

void BIRRTstar::extend() {
  BIRRTNode* p_st_new_node = NULL;
  BIRRTNode* p_gt_new_node = NULL;
  Path* p_st_new_path = NULL;
  Path* p_gt_new_path = NULL;
  if( _run_type != RUN_GOAL_TREE_TYPE ) {
    p_st_new_node = _extend(START_TREE_TYPE);
  }
  if( _run_type != RUN_START_TREE_TYPE ) {
    p_gt_new_node = _extend(GOAL_TREE_TYPE);
  }
  
  if( _run_type != RUN_GOAL_TREE_TYPE ) {
    p_st_new_path = find_path( p_st_new_node->m_pos );
  } 
  if( _run_type != RUN_START_TREE_TYPE ) {
    p_gt_new_path = find_path( p_gt_new_node->m_pos ); 
  }

  if( _run_type != RUN_GOAL_TREE_TYPE ) {
    if( p_st_new_path ) {
      _p_string_class_mgr->import_path( p_st_new_path );
    }
  }
  if( _run_type != RUN_START_TREE_TYPE ) {
    if( p_gt_new_path ) {
      _p_string_class_mgr->import_path( p_gt_new_path );
    }
  }
  _current_iteration++;
}

BIRRTNode* BIRRTstar::_extend( RRTree_type_t tree_type ) {
  bool node_inserted = false;
  while( false==node_inserted ) {
    POS2D rnd_pos = _sampling();
    KDNode2D nearest_node = _find_nearest( rnd_pos, tree_type );

    if (rnd_pos[0]==nearest_node[0] && rnd_pos[1]==nearest_node[1]) {
      continue;
    }

    POS2D new_pos = _steer( rnd_pos, nearest_node );
    if( true == _contains(new_pos) ) {
      continue;
    }
    if( true == _is_in_obstacle( new_pos ) ) {
      continue;
    }

    if( true == _is_obstacle_free( nearest_node, new_pos ) ) {
      if( true == _is_homotopy_eligible( nearest_node.getBIRRTNode(), new_pos, tree_type) ) {
        list<KDNode2D> near_list = _find_near( new_pos, tree_type );
        KDNode2D new_node( new_pos );

        // create new node
        BIRRTNode * p_new_rnode = _create_new_node( new_pos, tree_type );
        new_node.setBIRRTNode( p_new_rnode );
        if (tree_type == START_TREE_TYPE) {
          _p_st_kd_tree->insert( new_node );
        }
        else if(tree_type == GOAL_TREE_TYPE) {
          _p_gt_kd_tree->insert( new_node );
        }
        node_inserted = true;

        BIRRTNode* p_nearest_rnode = nearest_node.getBIRRTNode();
        list<BIRRTNode*> near_rnodes;
        near_rnodes.clear();
        for( list<KDNode2D>::iterator itr = near_list.begin();
             itr != near_list.end(); itr++ ) {
          KDNode2D kd_node = (*itr);
          BIRRTNode* p_near_rnode = kd_node.getBIRRTNode();
          near_rnodes.push_back( p_near_rnode );
        }
        // attach new node to reference trees
        _attach_new_node( p_new_rnode, p_nearest_rnode, near_rnodes, tree_type );
        // rewire near nodes of reference trees
        _rewire_near_nodes( p_new_rnode, near_rnodes, tree_type );

        return p_new_rnode;
      }
    }
  }
  return NULL;
}

KDNode2D BIRRTstar::_find_nearest( POS2D pos, RRTree_type_t tree_type ) {
  KDNode2D node( pos );
  if( START_TREE_TYPE == tree_type) {
    pair<KDTree2D::const_iterator,double> found = _p_st_kd_tree->find_nearest( node );
    KDNode2D near_node = *found.first;
    return near_node;
  }
  else if( GOAL_TREE_TYPE == tree_type ) {
    pair<KDTree2D::const_iterator,double> found = _p_gt_kd_tree->find_nearest( node );
    KDNode2D near_node = *found.first;
    return near_node;
  }
  return node;
}

list<KDNode2D> BIRRTstar::_find_near( POS2D pos, RRTree_type_t tree_type ) {
  list<KDNode2D> near_list;
  KDNode2D node(pos);

  int num_dimensions = 2;
  if ( START_TREE_TYPE == tree_type ) {
    int num_vertices = _p_st_kd_tree->size();
    _st_ball_radius =  _theta * _range * pow( log((double)(num_vertices + 1.0))/((double)(num_vertices + 1.0)), 1.0/((double)num_dimensions) );

    _p_st_kd_tree->find_within_range( node, _st_ball_radius, back_inserter( near_list ) );
  }
  else if ( GOAL_TREE_TYPE == tree_type ) {
    int num_vertices = _p_gt_kd_tree->size();
    _gt_ball_radius =  _theta * _range * pow( log((double)(num_vertices + 1.0))/((double)(num_vertices + 1.0)), 1.0/((double)num_dimensions) );

    _p_gt_kd_tree->find_within_range( node, _gt_ball_radius, back_inserter( near_list ) );
  }
  return near_list;
}


bool BIRRTstar::_contains( POS2D pos ) {
  if(_p_st_kd_tree) {
    KDNode2D node( pos[0], pos[1] );
    KDTree2D::const_iterator it = _p_st_kd_tree->find(node);
    if( it!=_p_st_kd_tree->end() ) {
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

double BIRRTstar::_calculate_cost( POS2D& pos_a, POS2D& pos_b ) {
  return _p_cost_func(pos_a, pos_b, _pp_cost_distribution, this);
}

BIRRTNode* BIRRTstar::_create_new_node(POS2D pos, RRTree_type_t tree_type) {
  BIRRTNode * pNode = new BIRRTNode(pos);
  if( tree_type == START_TREE_TYPE ) {
    _st_nodes.push_back(pNode);
  }
  else if( tree_type == GOAL_TREE_TYPE ) {
    _gt_nodes.push_back(pNode);
  }
  return pNode;
}

bool BIRRTstar::_remove_edge( BIRRTNode* p_node_parent, BIRRTNode*  p_node_child ) {
  if( p_node_parent==NULL ) {
    return false;
  }

  p_node_child->mp_parent = NULL;
  bool removed = false;
  for( list<BIRRTNode*>::iterator it=p_node_parent->m_child_nodes.begin();it!=p_node_parent->m_child_nodes.end();it++ ) {
    BIRRTNode* p_current = (BIRRTNode*)(*it);
    if ( p_current == p_node_child || p_current->m_pos==p_node_child->m_pos ) {
      p_current->mp_parent = NULL;
      p_current->clear_string();
      it = p_node_parent->m_child_nodes.erase(it);
      removed = true;
    }
  }
  return removed;
}

bool BIRRTstar::_has_edge(BIRRTNode* p_node_parent, BIRRTNode* p_node_child) {
  if ( p_node_parent == NULL || p_node_child == NULL ) {
    return false;
  }
  for( list<BIRRTNode*>::iterator it=p_node_parent->m_child_nodes.begin();it!=p_node_parent->m_child_nodes.end();it++ ) {
    BIRRTNode* p_curr_node = (*it);
    if( p_curr_node == p_node_child ) {
      return true;
    }
  }
  /*
    if (pNode_p == pNode_c->mpParent)
        return true;
  */
  return false;
}

bool BIRRTstar::_add_edge( BIRRTNode* p_node_parent, BIRRTNode* p_node_child ) {
  if( p_node_parent == NULL || p_node_child == NULL || p_node_parent == p_node_child ) {
    return false;
  }
  if ( p_node_parent->m_pos == p_node_child->m_pos ) {
    return false;
  }
  // generate the string of ID characters
  Point2D start( p_node_parent->m_pos[0], p_node_parent->m_pos[1] );
  Point2D goal( p_node_child->m_pos[0], p_node_child->m_pos[1] );
  //std::cout << "START " << start << " END " << goal << std::endl;
  vector< string > ids = _reference_frames->get_string( start, goal, _grammar_type );
  p_node_child->clear_string();
  p_node_child->append_to_string( p_node_parent->m_substring );
  p_node_child->append_to_string( ids );

  if ( true == _has_edge( p_node_parent, p_node_child ) ) {
    p_node_child->mp_parent = p_node_parent;
  }
  else {
    p_node_parent->m_child_nodes.push_back( p_node_child );
    p_node_child->mp_parent = p_node_parent;
  }
  p_node_child->m_child_nodes.unique();

  return true;
}

list<BIRRTNode*> BIRRTstar::_find_all_children( BIRRTNode* p_node ) {
  int level = 0;
  bool finished = false;
  list<BIRRTNode*> child_list;

  list<BIRRTNode*> current_level_nodes;
  current_level_nodes.push_back( p_node );
  while( false==finished ) {
    list<BIRRTNode*> current_level_children;
    int child_list_num = child_list.size();

    for( list<BIRRTNode*>::iterator it=current_level_nodes.begin(); it!=current_level_nodes.end(); it++ ) {
      BIRRTNode* pCurrentNode = (*it);
      for( list<BIRRTNode*>::iterator itc=pCurrentNode->m_child_nodes.begin(); itc!=pCurrentNode->m_child_nodes.end();itc++ ) {
        BIRRTNode *p_child_node= (*itc);
        if(p_child_node) {
          current_level_children.push_back(p_child_node);
          child_list.push_back(p_child_node);
        }
      }
    }

    child_list.unique();
    current_level_children.unique();

    if (current_level_children.size()==0) {
      finished = true;
    }
    else if (child_list.size()==child_list_num) {
      finished = true;
    }
    else {
      current_level_nodes.clear();
      for( list<BIRRTNode*>::iterator itt=current_level_children.begin();itt!=current_level_children.end();itt++ ) {
        BIRRTNode * pTempNode = (*itt);
        if( pTempNode ) {
          current_level_nodes.push_back( pTempNode );
        }
      }
      level +=1;
    }

    if(level>100) {
      break;
    }
  }
  child_list.unique();
  return child_list;
}


BIRRTNode* BIRRTstar::_find_ancestor(BIRRTNode* p_node) {
  return get_ancestor( p_node );
}

Path* BIRRTstar::find_path( POS2D via_pos ) {
  Path* p_new_path = NULL; 

  BIRRTNode * p_st_first_node = _find_nearest( via_pos, START_TREE_TYPE ).getBIRRTNode();
  BIRRTNode * p_gt_first_node = _find_nearest( via_pos, GOAL_TREE_TYPE ).getBIRRTNode();
   
  if ( false == _is_obstacle_free( p_st_first_node->m_pos, p_gt_first_node->m_pos ) ) {
    return p_new_path;
  }
   
  if( p_st_first_node != NULL && p_gt_first_node != NULL ) {
    Path* p_from_path = _get_subpath( p_st_first_node, START_TREE_TYPE );
    Path* p_to_path = _get_subpath( p_gt_first_node, GOAL_TREE_TYPE );

    p_new_path = _concatenate_paths( p_from_path, p_to_path ); 
  }

  return p_new_path;
}

void BIRRTstar::_attach_new_node(BIRRTNode* p_node_new, BIRRTNode* p_nearest_node, list<BIRRTNode*> near_nodes, RRTree_type_t tree_type) {
  double min_new_node_cost = p_nearest_node->m_cost + _calculate_cost(p_nearest_node->m_pos, p_node_new->m_pos);
  BIRRTNode* p_min_node = p_nearest_node;

  for(list<BIRRTNode*>::iterator it=near_nodes.begin();it!=near_nodes.end();it++) {
    BIRRTNode* p_near_node = *it;
    if ( true == _is_obstacle_free( p_near_node->m_pos, p_node_new->m_pos ) ) {
      if ( true == _is_homotopy_eligible( p_near_node, p_node_new->m_pos, tree_type ) ) {
        double delta_cost = _calculate_cost( p_near_node->m_pos, p_node_new->m_pos );
        double new_cost = p_near_node->m_cost + delta_cost;
        if ( new_cost < min_new_node_cost ) {
          p_min_node = p_near_node;
          min_new_node_cost = new_cost;
        }
      }
    }
  }

  bool added = _add_edge( p_min_node, p_node_new );
  if( added ) {
    p_node_new->m_cost = min_new_node_cost;
  }

}

void BIRRTstar::_rewire_near_nodes(BIRRTNode* p_node_new, list<BIRRTNode*> near_nodes, RRTree_type_t tree_type) {
  for( list<BIRRTNode*>::iterator it=near_nodes.begin(); it!=near_nodes.end(); it++ ) {
    BIRRTNode * p_near_node = (*it);

    if(p_near_node->m_pos ==p_node_new->m_pos ||  p_near_node->m_pos==_p_st_root->m_pos || p_node_new->mp_parent->m_pos==p_near_node->m_pos) {
      continue;
    }

    if( true == _is_obstacle_free( p_node_new->m_pos, p_near_node->m_pos ) ) {
      if( true == _is_homotopy_eligible(p_near_node, p_node_new->m_pos, tree_type) ) {
        double temp_delta_cost = _calculate_cost( p_node_new->m_pos, p_near_node->m_pos );
        double temp_cost_from_new_node = p_node_new->m_cost + temp_delta_cost;
        if( temp_cost_from_new_node < p_near_node->m_cost ) {
          double min_delta_cost = p_near_node->m_cost - temp_cost_from_new_node;
          BIRRTNode * p_parent_node = p_near_node->mp_parent;
          bool removed = _remove_edge(p_parent_node, p_near_node);
          if(removed) {
            bool added = _add_edge(p_node_new, p_near_node);
            if( added ) {
              p_near_node->m_cost = temp_cost_from_new_node;
              _update_cost_to_children(p_near_node, min_delta_cost);
            }
          }
          else {
            cout << " Failed in removing " << endl;
          }
        }
      }
    }
  }
}

void BIRRTstar::_update_cost_to_children( BIRRTNode* p_node, double delta_cost ) {
  list<BIRRTNode*> child_list = _find_all_children( p_node );
  for( list<BIRRTNode*>::iterator it = child_list.begin(); it != child_list.end();it++ ) {
    BIRRTNode* p_child_node = (*it);
    if( p_child_node ) {
      p_child_node->m_cost -= delta_cost;
    }
  }
}

bool BIRRTstar::_get_closest_node ( POS2D pos, BIRRTNode*& p_node_closet_to_goal, double& delta_cost, RRTree_type_t tree_type ) {
  bool found = false;

  list<KDNode2D> near_nodes = _find_near( pos, tree_type );
  double min_total_cost = numeric_limits<double>::max();

  for(list<KDNode2D>::iterator it=near_nodes.begin();
      it!=near_nodes.end();it++) {
    KDNode2D kd_node = (*it);
    BIRRTNode* p_node = kd_node.getBIRRTNode();
    double new_delta_cost = _calculate_cost(p_node->m_pos, pos);
    double new_total_cost= p_node->m_cost + new_delta_cost;
    if (new_total_cost < min_total_cost) {
      min_total_cost = new_total_cost;
      p_node_closet_to_goal = p_node;
      delta_cost = new_delta_cost;
      found = true;
    }
  }
  return found;
}

void BIRRTstar::dump_distribution(string filename) {
  ofstream myfile;
  myfile.open (filename.c_str());
  if(_pp_cost_distribution) {
    for(int i=0;i<_sampling_width;i++) {
      for(int j=0;j<_sampling_height;j++) {
        myfile << _pp_cost_distribution[i][j] << " ";
      }
      myfile << "\n";
    }
  }
  myfile.close();
}

Path* BIRRTstar::_concatenate_paths( Path* p_from_path, Path* p_to_path ) {
  Path* p_new_path = new Path( p_from_path->m_start, p_to_path->m_start );
  Point2D from_path_end( p_from_path->m_goal[0], p_from_path->m_goal[1] );
  Point2D to_path_end( p_to_path->m_goal[0], p_to_path->m_goal[1] );
  vector< string > between_ids = _reference_frames->get_string( from_path_end, to_path_end , _grammar_type );
  double delta_cost = _calculate_cost( p_from_path->m_goal, p_to_path->m_goal );

  p_new_path->append_waypoints( p_from_path->m_way_points );
  p_new_path->append_substring( p_from_path->m_string );
  p_new_path->append_substring( between_ids );
  p_new_path->append_waypoints( p_to_path->m_way_points, true );
  p_new_path->append_substring( p_to_path->m_string, true );
  p_new_path->m_cost = p_from_path->m_cost + delta_cost + p_to_path->m_cost;
  
  return p_new_path;
}

Path* BIRRTstar::_get_subpath( BIRRTNode* p_end_node, RRTree_type_t tree_type ) {
  Path* p_subpath = NULL; 
  list<BIRRTNode*> node_list;
  get_parent_node_list( p_end_node , node_list );
  if( tree_type == START_TREE_TYPE ) {
    p_subpath = new Path( _p_st_root->m_pos, p_end_node->m_pos );
  }
  else if ( tree_type == GOAL_TREE_TYPE ) {
    p_subpath = new Path( _p_gt_root->m_pos, p_end_node->m_pos );
  }
  p_subpath->m_cost = p_end_node->m_cost;
  p_subpath->append_substring( p_end_node->m_substring ); 
  p_subpath->m_way_points.clear();
  for( list<BIRRTNode*>::reverse_iterator itr = node_list.rbegin();
       itr != node_list.rend(); itr ++ ) {
    BIRRTNode* p_rrt_node = (*itr);
    p_subpath->m_way_points.push_back( p_rrt_node->m_pos ); 
  }
  return p_subpath;
}

vector<Path*> BIRRTstar::get_paths() {
  vector<Path*> paths;
  if ( _p_string_class_mgr ) {
    paths = _p_string_class_mgr->export_paths();  
  } 
  return paths;
}

void BIRRTstar::set_reference_frames( ReferenceFrameSet* p_reference_frames ) {
  _reference_frames = p_reference_frames;
}


bool BIRRTstar::_is_homotopy_eligible( BIRRTNode* p_node_parent, POS2D pos, RRTree_type_t tree_type ) {
  if( _reference_frames && _reference_frames->get_string_constraint().size()==0 ) {
      return true;
  }
  Point2D start( p_node_parent->m_pos[0], p_node_parent->m_pos[1] );
  Point2D end( pos[0], pos[1] );
  vector< string > ids = _reference_frames->get_string( start, end, _grammar_type );
  vector< string > temp_ids;

  for( vector< string >::iterator it = p_node_parent->m_substring.begin();
       it != p_node_parent->m_substring.end(); it ++ ) {
    string id = (*it);
    temp_ids.push_back( id );
  }
  for( vector< string >::iterator it = ids.begin();
       it != ids.end(); it++) {
    string id = (*it);
    temp_ids.push_back( id );
  }

  if( tree_type == START_TREE_TYPE ) {
    return _reference_frames->is_constained_substring(temp_ids, false);
  } 
  else if( tree_type == GOAL_TREE_TYPE ) {
    return _reference_frames->is_constained_substring(temp_ids, true);
  } 
 
  return false;
}
