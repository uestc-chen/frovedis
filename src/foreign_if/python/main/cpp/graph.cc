#include "python_client_headers.hpp"
#include "exrpc_graphx.hpp"
#include "short_hand_dense_type.hpp"
#include "short_hand_sparse_type.hpp"

#include "exrpc_model.hpp"
#include "short_hand_model_type.hpp"


extern "C" {
  //pagerank
  long set_graph_data(const char* host, int port, long proxy){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    //int vb = 0; //no log (default=0)
    exrpc_ptr_t result = -1;
    try{
      result = exrpc_async(fm_node,set_graph_data<S_MAT1>,f_dptr).get();
    }
    catch(std::exception& e) { set_status(true,e.what()); }
    return (long)result;
  }
  PyObject* get_graph_data(const char* host, int port, long proxy) {
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    dummy_matrix adj_mat;
    try {
      adj_mat = exrpc_async(fm_node, get_graph_data_dummy_matrix<graph>, f_dptr).get();
    }
    catch(std::exception& e) { set_status(true,e.what()); }
    //return to_py_dummy_edge_array(adj_mat);
    return to_py_dummy_matrix(adj_mat);
  }
  PyObject* call_frovedis_pagerank(const char* host, int port, long proxy,
                                  double epsilon, double dfactor,
                                  int max_iter){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    int vb = 0; //no log (default=0)
    std::vector<double> result;
    try{
      result = exrpc_async(fm_node, frovedis_pagerank<graph>, f_dptr,
                       epsilon, dfactor, max_iter, vb).get();
    }
    catch(std::exception& e) { set_status(true,e.what()); }
    return to_python_double_list(result);
  }
  void call_frovedis_sssp(const char* host, int port, long proxy,
                        int* dist, long* pred, long num_vertices, long source){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    int vb = 0; //no log (default=0)
    sssp_result result;
    try{
      result = exrpc_async(fm_node, frovedis_sssp<graph>, f_dptr,
                       source, vb).get();
    }
    catch(std::exception& e) { set_status(true,e.what()); }
    checkAssumption(result.nodes_dist.size() == num_vertices);
    checkAssumption(result.nodes_pred.size() == num_vertices);
    auto resdistp = result.nodes_dist.data();
    auto respredp = result.nodes_pred.data();
    for(size_t i = 0; i < num_vertices; ++i){
      dist[i] = resdistp[i];
      pred[i] = respredp[i];
    }
  }
  PyObject* call_frovedis_bfs(const char* host, int port, long proxy,
                              long* nodes_in_which_cc, 
                              int* nodes_dist, long num_vertices) {
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    int vb = 0; //no log (default=0)
    bfs_result result;
    try{
      result = exrpc_async(fm_node, frovedis_bfs<graph>, f_dptr, vb).get();
    }
    catch(std::exception& e) { set_status(true,e.what());}
    checkAssumption(result.nodes_dist.size() == num_vertices);
    checkAssumption(result.nodes_in_which_cc.size() == num_vertices);
    auto resdistp = result.nodes_dist.data();
    auto reswhichp = result.nodes_in_which_cc.data();
    for(size_t i = 0; i < num_vertices; ++i){
      nodes_dist[i] = resdistp[i];
      nodes_in_which_cc[i] = reswhichp[i];
    }
    return to_python_long_list(result.num_nodes_in_each_cc);
  }
  void show_graph_py(const char* host, int port, long proxy){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    try{
      exrpc_oneway(fm_node, show_graph<graph>, f_dptr);
    }
    catch(std::exception& e) {set_status(true, e.what());}
  }
  void release_graph_py(const char* host, int port, long proxy){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    try{
      exrpc_oneway(fm_node, release_graph<graph>, f_dptr);
    }
    catch(std::exception& e) {set_status(true, e.what());}
  }
  void save_graph_py(const char* host, int port, long proxy,
                     const char* fname){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    std::string f_name(fname);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    try{
      exrpc_oneway(fm_node, save_graph<graph>, f_dptr, f_name);
    }
    catch(std::exception& e) {set_status(true, e.what());}
  }
  PyObject* load_graph_from_text_file(const char* host, int port,
            const char* fname){
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    std::string f_name(fname);
    dummy_graph ret;
    try{
      ret = exrpc_async(fm_node, load_graph<graph>, f_name).get();
    }
    catch(std::exception& e) {set_status(true, e.what());}
    return to_py_dummy_graph(ret);
  }
  long copy_graph_py(const char* host, int port, long proxy) {
    ASSERT_PTR(host);
    exrpc_node fm_node(host, port);
    auto f_dptr = static_cast<exrpc_ptr_t> (proxy);
    exrpc_ptr_t ret = -1;
    try {
      ret = exrpc_async(fm_node, copy_graph<graph>, f_dptr).get();
      }
    catch(std::exception& e) { set_status(true,e.what()); }
    return (long)ret;
  }
}
