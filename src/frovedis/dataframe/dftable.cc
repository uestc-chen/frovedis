#include <unordered_set>
#include "dftable.hpp"
#include "dfoperator.hpp"
#include "dftable_to_string.hpp"
#include "dftable_to_words.hpp"
#include "make_dftable_loadtext.hpp"
#include "../text/char_int_conv.hpp"
#include "../text/load_text.hpp"
#include "../core/prefix_sum.hpp"

#define GROUPED_DFTABLE_VLEN 256

namespace frovedis {

std::vector<std::string> dftable_base::columns() const {
  return col_order;
}

std::vector<std::pair<std::string, std::string>>
dftable_base::dtypes() {
  std::vector<std::pair<std::string,std::string>> ret;
  auto cols = columns();
  for(size_t i = 0; i < cols.size(); i++) {
    ret.push_back(std::make_pair(cols[i],
                                 column(cols[i])->dtype()));
  }
  return ret;
}

dftable dftable_base::select(const std::vector<std::string>& cols) {
  dftable ret;
  ret.row_size = row_size;
  for(size_t i = 0; i < cols.size(); i++) {
    ret.col[cols[i]] = column(cols[i]);
  }
  ret.col_order = cols;
  return ret;
}

dftable dftable_base::materialize(){return select(columns());}

sorted_dftable dftable_base::sort(const std::string& name) {
  node_local<std::vector<size_t>> idx;
  auto sorted_column = column(name)->sort(idx);
  return sorted_dftable(*this, std::move(idx), name, std::move(sorted_column));
}

sorted_dftable dftable_base::sort_desc(const std::string& name) {
  node_local<std::vector<size_t>> idx;
  auto sorted_column = column(name)->sort_desc(idx);
  return sorted_dftable(*this, std::move(idx), name, std::move(sorted_column));
}

void join_column_name_check(const std::map<std::string,
                            std::shared_ptr<dfcolumn>>& col1,
                            const std::map<std::string,
                            std::shared_ptr<dfcolumn>>& col2) {
  for(auto i = col2.cbegin(); i != col2.end(); ++i) {
    if(col1.find(i->first) != col1.end())
      throw std::runtime_error("joining tables have same column name: " +
                               i->first);
  }
}

hash_joined_dftable
dftable_base::hash_join(dftable_base& right_,
                        const std::shared_ptr<dfoperator>& op) {
  if(right_.is_right_joinable()) {
    auto& right = right_;
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    auto idxs = op->hash_join(*this, right, left_idx, right_idx);
    return hash_joined_dftable(*this, right, std::move(idxs.first),
                               std::move(idxs.second));
  }
  else {
    auto right = right_.materialize();
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    auto idxs = op->hash_join(*this, right, left_idx, right_idx);
    return hash_joined_dftable(*this, right, std::move(idxs.first),
                               std::move(idxs.second));
  }
}

hash_joined_dftable 
dftable_base::outer_hash_join(dftable_base& right_,
                              const std::shared_ptr<dfoperator>& op) {
  if(right_.is_right_joinable()) {
    auto& right = right_;
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    node_local<std::vector<size_t>> left_idx_;
    node_local<std::vector<size_t>> right_idx_;
    node_local<std::vector<size_t>> right_nulls_;
    std::tie(left_idx_, right_idx_, right_nulls_)
      = op->outer_hash_join(*this, right, left_idx, right_idx);
    return hash_joined_dftable(*this, right, std::move(left_idx_),
                               std::move(right_idx_), std::move(right_nulls_));
  } else {
    auto right = right_.materialize();
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    node_local<std::vector<size_t>> left_idx_;
    node_local<std::vector<size_t>> right_idx_;
    node_local<std::vector<size_t>> right_nulls_;
    std::tie(left_idx_, right_idx_, right_nulls_)
      = op->outer_hash_join(*this, right, left_idx, right_idx);
    return hash_joined_dftable(*this, right, std::move(left_idx_),
                               std::move(right_idx_), std::move(right_nulls_));
  }
}

bcast_joined_dftable
dftable_base::bcast_join(dftable_base& right_,
                         const std::shared_ptr<dfoperator>& op) {
  if(right_.is_right_joinable()) {
    auto& right = right_;
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    auto idxs = op->bcast_join(*this, right, left_idx, right_idx);
    return bcast_joined_dftable(*this, right, std::move(idxs.first),
                                std::move(idxs.second));
  }
  else {
    auto right = right_.materialize();
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    auto idxs = op->bcast_join(*this, right, left_idx, right_idx);
    return bcast_joined_dftable(*this, right, std::move(idxs.first),
                                std::move(idxs.second));
  }
}

bcast_joined_dftable 
dftable_base::outer_bcast_join(dftable_base& right_,
                               const std::shared_ptr<dfoperator>& op) {
  if(right_.is_right_joinable()) {
    auto& right = right_;
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    node_local<std::vector<size_t>> left_idx_;
    node_local<std::vector<size_t>> right_idx_;
    node_local<std::vector<size_t>> right_nulls_;
    std::tie(left_idx_, right_idx_, right_nulls_)
      = op->outer_bcast_join(*this, right, left_idx, right_idx);
    return bcast_joined_dftable(*this, right,
                                std::move(left_idx_),
                                std::move(right_idx_),
                                std::move(right_nulls_));
  } else {
    auto right = right_.materialize();
    join_column_name_check(col, right.col);
    auto left_idx = get_local_index();
    auto right_idx = right.get_local_index();
    node_local<std::vector<size_t>> left_idx_;
    node_local<std::vector<size_t>> right_idx_;
    node_local<std::vector<size_t>> right_nulls_;
    std::tie(left_idx_, right_idx_, right_nulls_)
      = op->outer_bcast_join(*this, right, left_idx, right_idx);
    return bcast_joined_dftable(*this, right,
                                std::move(left_idx_),
                                std::move(right_idx_),
                                std::move(right_nulls_));
  }
}

void shrink_missed_inplace(std::vector<size_t>& to_shrink,
                           std::vector<size_t>& missed) {
  auto ret = shrink_missed(to_shrink, missed);
  to_shrink.swap(ret);
}

// use pointers to get derived tables
// vector of reference is not allowed
star_joined_dftable
dftable_base::star_join(const std::vector<dftable_base*>& dftables, 
                        const std::vector<std::shared_ptr<dfoperator>>& ops) {
  size_t dftablessize = dftables.size();
  if(dftablessize == 0)
    throw std::runtime_error("star_join: size of table is zero");
  if(dftablessize != ops.size())
    throw std::runtime_error
      ("star_join: size of table is different from size of ops");

  std::vector<dftable_base> rights(dftablessize);
  node_local<std::vector<size_t>> ret_left_idx = get_local_index();
  std::vector<node_local<std::vector<size_t>>> ret_right_idxs(dftablessize);
  for(size_t i = 0; i < dftablessize; i++) {
    dftable_base* rightp;
    dftable_base right_tmp;
    if(dftables[i]->is_right_joinable()) {
      rights[i] = *dftables[i];
      rightp = dftables[i];
    } else {
      rights[i] = dftables[i]->materialize();
      rightp = &rights[i];
    }
    join_column_name_check(col, rights[i].col);
    auto right_idx = rightp->get_local_index();
    node_local<std::vector<size_t>> ret_right_idx;
    node_local<std::vector<size_t>> missed;
    std::tie(ret_right_idx, missed) = 
      ops[i]->star_join(*this, *rightp, ret_left_idx, right_idx);
    ret_left_idx.mapv(shrink_missed_inplace, missed);
    ret_right_idx.mapv(shrink_missed_inplace, missed);
    ret_right_idxs[i] = std::move(ret_right_idx);
    for(size_t j = 0; j < i; j++) 
      ret_right_idxs[j].mapv(shrink_missed_inplace, missed);
  }
  return star_joined_dftable(*this, std::move(rights),
                             std::move(ret_left_idx), 
                             std::move(ret_right_idxs));
}

void merge_split(std::vector<size_t>& split, std::vector<size_t>& to_merge) {
  auto ret = set_union(split, to_merge);
  split.swap(ret);
}

void create_merge_map(std::vector<size_t>& nodeid,
                      std::vector<size_t>& split,
                      std::vector<std::vector<size_t>>& merge_map);
                                   
grouped_dftable
dftable_base::group_by(const std::vector<std::string>& cols) {
  size_t size = cols.size();
  if(size == 0) {
    throw std::runtime_error("column is not specified for group by");
  } else if (size == 1) {
    auto local_idx = get_local_index(); // might be filtered
    auto split_idx = make_node_local_allocate<std::vector<size_t>>();
    auto hash_divide =
      make_node_local_allocate<std::vector<std::vector<size_t>>>();
    auto merge_map =
      make_node_local_allocate<std::vector<std::vector<size_t>>>();
    auto grouped_column = column(cols[0])->group_by
      (local_idx, split_idx, hash_divide, merge_map);
    return grouped_dftable(*this, std::move(local_idx), std::move(split_idx),
                           std::move(hash_divide), std::move(merge_map),
                           {grouped_column}, cols);
  } else {
    std::vector<std::shared_ptr<dfcolumn>> pcols(size);
    for(size_t i = 0; i < size; i++) pcols[i] = column(cols[i]);
    auto local_idx = get_local_index(); // might be filtered
    // sort reverse order to show intuitive result
    for(size_t i = 0; i < size-1; i++) {
      pcols[size - i - 1]->multi_group_by_sort(local_idx);
    }
    auto split_idx = pcols[0]->multi_group_by_sort_split(local_idx);
    for(size_t i = 1; i < size; i++) {
      auto tmp = pcols[i]->multi_group_by_split(local_idx);
      split_idx.mapv(merge_split, tmp);
    }
    std::vector<std::shared_ptr<dfcolumn>> pcols2(size);
    for(size_t i = 0; i < size; i++) {
      pcols2[i] = pcols[i]->multi_group_by_extract(local_idx, split_idx,
                                                   false);
    }
    auto hash_base = pcols2[0]->calc_hash_base();
    // 52 is fraction of double, size_t might be 32bit...
    int bit_len = std::min(sizeof(size_t) * 8, size_t(52));
    int shift = bit_len / size;
    // from memory access point of view, not efficient though...
    for(size_t i = 1; i < size; i++) {
      pcols2[i]->calc_hash_base(hash_base, shift);
    }
    auto hash_divide =
      make_node_local_allocate<std::vector<std::vector<size_t>>>();
    hash_base.mapv
      (+[](std::vector<size_t>& hash_base,
           std::vector<std::vector<size_t>>& hash_divide) {
        auto size = hash_base.size();
        std::vector<size_t> iota(size);
        auto iotap = iota.data();
        for(size_t i = 0; i < size; i++) iotap[i] = i;
        split_by_hash_no_outval(hash_base, iota, hash_divide);
      }, hash_divide);
    std::vector<std::shared_ptr<dfcolumn>> pcols3(size);
    for(size_t i = 0; i < size; i++) {
      pcols3[i] = pcols2[i]->multi_group_by_exchange(hash_divide);
    }
    auto sizes_tmp = hash_divide.map
      (+[](std::vector<std::vector<size_t>>& hash_divide) {
        std::vector<size_t> ret(hash_divide.size());
        for(size_t i = 0; i < hash_divide.size(); i++) {
          ret[i] = hash_divide[i].size();
        }
        return ret;
      });
    auto sizes = alltoall_exchange(sizes_tmp);
    auto nodeid = sizes.map
      (+[](std::vector<size_t>& sizes) {
        size_t total = 0;
        auto sizesp = sizes.data();
        for(size_t i = 0; i < sizes.size(); i++) total += sizesp[i];
        std::vector<size_t> ret(total);
        auto retp = ret.data();
        for(size_t i = 0; i < sizes.size(); i++) {
          auto size = sizes[i];
          for(size_t j = 0; j < size; j++) {
            retp[j] = i;
          }
          retp += size;
        }
        return ret;
      });
    auto local_idx2 = nodeid.map
      (+[](std::vector<size_t>& nodeid) {
        size_t size = nodeid.size();
        std::vector<size_t> ret(size);
        auto retp = ret.data();
        for(size_t i = 0; i < size; i++) retp[i] = i;
        return ret;
      });
    for(size_t i = 0; i < size-1; i++) {
      pcols3[size - i - 1]->multi_group_by_sort(local_idx2);
    }
    auto split_idx2 = pcols3[0]->multi_group_by_sort_split(local_idx2);
    for(size_t i = 1; i < size; i++) {
      auto tmp = pcols3[i]->multi_group_by_split(local_idx2);
      split_idx2.mapv(merge_split, tmp);
    }
    auto nodeid2 = nodeid.map
      (+[](std::vector<size_t>& nodeid, std::vector<size_t>& local_idx) {
        auto size = nodeid.size();
        auto nodeidp = nodeid.data();
        auto local_idxp = local_idx.data();
        std::vector<size_t> ret(size);
        auto retp = ret.data();
        for(size_t i = 0; i < size; i++) {
          retp[i] = nodeidp[local_idxp[i]];
        }
        return ret;
      }, local_idx2);
    auto merge_map =
      make_node_local_allocate<std::vector<std::vector<size_t>>>();
    merge_map.mapv
      (+[](std::vector<std::vector<size_t>>& merge_map,
           std::vector<size_t> sizes) {
        merge_map.resize(sizes.size());
        for(size_t i = 0; i < sizes.size(); i++) {
          auto size = sizes[i];
          for(size_t j = 0; j < size; j++) {
            merge_map[i].resize(size);
          }
        }
      }, sizes);
    nodeid2.mapv(create_merge_map, split_idx2, merge_map);
    std::vector<std::shared_ptr<dfcolumn>> pcols4(size);    
    for(size_t i = 0; i < size; i++) {
      pcols4[i] = pcols3[i]->
        multi_group_by_extract(local_idx2, split_idx2, true);
    }
    return grouped_dftable(*this, std::move(local_idx), std::move(split_idx),
                           std::move(hash_divide), std::move(merge_map),
                           std::move(pcols4), cols);
  }
}

dftable dftable_base::head(size_t limit) {
  dftable limit_table;
  auto cols = columns();
  for(size_t i = 0; i < cols.size(); i++) {
    limit_table.append_column(cols[i],
                              this->column(cols[i])->head(limit));
  }
  return limit_table;
}

dftable dftable_base::tail(size_t limit) {
  dftable limit_table;
  auto cols = columns();
  for(size_t i = 0; i < cols.size(); i++) {
    limit_table.append_column(cols[i],
                              this->column(cols[i])->tail(limit));
  }
  return limit_table;
}

void dftable_base::show_all(bool with_index) {
  auto table_words = dftable_to_words(*this).reduce(merge_words);
  auto cols = columns();
  auto num_col = cols.size();
  auto num_words = table_words.starts.size();
  auto num_row = num_words / num_col;
  if(num_words % num_col != 0)
    throw std::runtime_error("show_all: incorrect number of col or row");
  if(with_index) std::cout << "\t";
  for(size_t i = 0; i < num_col-1; i++) {
    std::cout << cols[i] << "\t";
  }
  std::cout << cols[num_col-1] << std::endl;
  auto charsp = table_words.chars.data();
  for(size_t i = 0; i < num_row; i++) {
    if(with_index) std::cout << i << "\t";
    for(size_t j = 0; j < num_col-1; j++) {
      auto crnt_start = table_words.starts[i * num_col + j];
      auto crnt_len = table_words.lens[i * num_col + j];
      for(size_t k = 0; k < crnt_len; k++) {
        std::cout << static_cast<char>(charsp[crnt_start + k]);
      }
      std::cout << "\t";
    }
    auto crnt_start = table_words.starts[i * num_col + num_col - 1];
    auto crnt_len = table_words.lens[i * num_col + num_col - 1];
    for(size_t k = 0; k < crnt_len; k++) {
      std::cout << static_cast<char>(charsp[crnt_start + k]);
    }
    std::cout << "\n";
  }
}

void dftable_base::show(size_t limit) {
  head(limit).show_all();
}

void dftable_base::show() {
  show(20);
  if(num_row() > 20) std::cout << "..." << std::endl;
}

void dftable_base::print() {
  auto nr = num_row();
  if(nr < 61) show_all(true);
  else {
    head(30).show_all(true);
    std::cout << "..." << std::endl;
    auto t = tail(30);
    auto table_string = dftable_to_string(t).gather();
    for(size_t i = 0; i < table_string.size(); i++) {
      std::cout << nr - 30 + i << "\t";
      for(size_t j = 0; j < table_string[i].size()-1; j++) {
        std::cout << table_string[i][j] << "\t";
      }
      std::cout << table_string[i][table_string[i].size()-1] << std::endl;
    }
  }
}

size_t dftable_base::count(const std::string& name) {
  return column(name)->count();
}

double dftable_base::avg(const std::string& name) {
  return column(name)->avg();
}

std::shared_ptr<dfcolumn> dftable_base::raw_column(const std::string& name) {
  return dftable_base::column(name);
}

void dftable_base::save(const std::string& dir) {
  struct stat sb;
  if(stat(dir.c_str(), &sb) != 0) { // no file/directory
    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO; // man 2 stat
    if(mkdir(dir.c_str(), mode) != 0) {
      perror("mkdir failed:");
      throw std::runtime_error("mkdir failed");
    }
  } else if(!S_ISDIR(sb.st_mode)) {
    throw std::runtime_error(dir + " is not a directory");
  }
  std::string colfile = dir + "/columns";
  std::string typefile = dir + "/types";
  std::ofstream colstr;
  std::ofstream typestr;
  colstr.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  colstr.open(colfile.c_str());
  typestr.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  typestr.open(typefile.c_str());
  auto dt = dtypes();
  for(size_t i = 0; i < dt.size(); i++) {
    colstr << dt[i].first << std::endl;
    typestr << dt[i].second << std::endl;
    column(dt[i].first)->save(dir + "/" + dt[i].first);
  }
}

std::vector<char> savetext_helper(const words& ws, size_t num_col,
                                  const std::string& sep) {
  std::vector<size_t> new_starts;
  auto vecint = concat_words(ws, sep, new_starts);
  auto new_starts_size = new_starts.size();
  auto new_startsp = new_starts.data();
  auto vecintp = vecint.data();
  auto num_row = new_starts_size / num_col;
  if(new_starts_size % num_col != 0)
    throw std::runtime_error("savetext: incorrect number of col or row");
  for(size_t i = 1; i < num_row; i++) {
    vecintp[new_startsp[i * num_col] - 1] = '\n';
  }
  auto vecint_size = vecint.size();
  vecintp[vecint_size-1] = '\n';
  std::vector<char> ret(vecint_size);
  int_to_char(vecintp, vecint_size, ret.data());
  return ret;
}

std::vector<std::pair<std::string, std::string>>
  dftable_base::savetext(const std::string& file, const std::string& sep,
                         bool quote_and_escape, const std::string& nullstr) {
  auto table_words = dftable_to_words(*this, quote_and_escape, nullstr);
  table_words.map(savetext_helper, broadcast(num_col()), broadcast(sep))
    .moveto_dvector<char>().savebinary(file);
  return dtypes();
}

colmajor_matrix<float>
dftable_base::to_colmajor_matrix_float(const std::vector<std::string>& cols) {
  auto materialized = select(cols);
  colmajor_matrix<float> ret;
  ret.data = make_node_local_allocate<colmajor_matrix_local<float>>();
  ret.num_col = cols.size();
  ret.num_row = row_size;
  auto bcols = broadcast(cols.size());
  for(size_t i = 0; i < cols.size(); i++) {
    auto nl = materialized.column(cols[i])->as_dvector_float().
      moveto_node_local();
    ret.data.mapv(append_column_to_colmajor_matrix<float>, broadcast(i),
                  bcols, nl);
  }
  return ret;
}

colmajor_matrix<double>
dftable_base::to_colmajor_matrix_double(const std::vector<std::string>& cols) {
  auto materialized = select(cols);
  colmajor_matrix<double> ret;
  ret.data = make_node_local_allocate<colmajor_matrix_local<double>>();
  ret.num_col = cols.size();
  ret.num_row = row_size;
  auto bcols = broadcast(cols.size());
  for(size_t i = 0; i < cols.size(); i++) {
    auto nl = materialized.column(cols[i])->as_dvector_double().
      moveto_node_local();
    ret.data.mapv(append_column_to_colmajor_matrix<double>, broadcast(i),
                  bcols, nl);
  }
  return ret;
}

rowmajor_matrix<float>
dftable_base::to_rowmajor_matrix_float(const std::vector<std::string>& cols) {
  return to_colmajor_matrix_float(cols).to_rowmajor();
}

rowmajor_matrix<double>
dftable_base::to_rowmajor_matrix_double(const std::vector<std::string>& cols) {
  return to_colmajor_matrix_double(cols).to_rowmajor();
}

void dftable_to_sparse_info::save(const std::string& dir) {
  struct stat sb;
  if(stat(dir.c_str(), &sb) != 0) { // no file/directory
    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO; // man 2 stat
    if(mkdir(dir.c_str(), mode) != 0) {
      perror("mkdir failed:");
      throw std::runtime_error("mkdir failed");
    }
  } else if(!S_ISDIR(sb.st_mode)) {
    throw std::runtime_error(dir + " is not a directory");
  }
  std::string colfile = dir + "/columns";
  std::string catfile = dir + "/categories";
  std::string numsfile = dir + "/nums";
  std::ofstream colstr;
  std::ofstream catstr;
  std::ofstream numstr;
  colstr.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  catstr.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  numstr.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  colstr.open(colfile.c_str());
  catstr.open(catfile.c_str());
  numstr.open(numsfile.c_str());
  for(size_t i = 0; i < columns.size(); i++) {
    colstr << columns[i] << std::endl;
  }
  for(size_t i = 0; i < categories.size(); i++) {
    catstr << categories[i] << std::endl;
  }
  numstr << num_row << "\n" << num_col << std::endl;
  for(size_t i = 0; i < mapping_tables.size(); i++) {
    mapping_tables[i].save(dir + "/" + categories[i]);
  }
}

void dftable_to_sparse_info::load(const std::string& input) {
  std::string colfile = input + "/columns";
  std::string catfile = input + "/categories";
  std::string numsfile = input + "/nums";
  std::ifstream colstr;
  std::ifstream catstr;
  std::ifstream numstr;
  colstr.open(colfile.c_str());
  catstr.open(catfile.c_str());
  numstr.open(numsfile.c_str());
  std::string tmp;
  columns.clear();
  categories.clear();
  while(std::getline(colstr, tmp)) {
    columns.push_back(tmp);
  }
  while(std::getline(catstr, tmp)) {
    categories.push_back(tmp);
  }
  numstr >> num_row >> num_col;
  mapping_tables.clear();
  size_t cat_size = categories.size();
  mapping_tables.resize(cat_size);
  for(size_t i = 0; i < cat_size; i++) {
    mapping_tables[i].load(input + "/" + categories[i]);
  }
}

ell_matrix<float>
dftable_base::to_ell_matrix_float(dftable_to_sparse_info& info) {
  auto& cols = info.columns;
  auto& cats = info.categories;
  if(cols.size() == 0)
    throw std::runtime_error("to_ell_matrix: no colums to convert");
  auto materialized = select(cols); // cause exception if not applicable
  std::unordered_set<std::string> cats_set(cats.begin(), cats.end());
  auto first_column = materialized.column(cols[0]);

  ell_matrix<float> ret;
  ret.data = make_node_local_allocate<ell_matrix_local<float>>();
  ret.num_row = info.num_row;
  ret.num_col = info.num_col;
  auto sizes = first_column->sizes();
  ret.data.mapv(to_ell_matrix_init<float>, broadcast(cols.size()),
                broadcast(sizes), broadcast(ret.num_col));
  size_t current_category = 0;
  size_t current_logical_col = 0;
  for(size_t i = 0; i < cols.size(); i++) {
    auto col = cols[i];
    if(cats_set.find(col) != cats_set.end()) {
      auto tmp1 = materialized.select({col});
      auto tmp2 = info.mapping_tables[current_category++];
      auto col_idx = tmp1.bcast_join(tmp2, frovedis::eq(col, "key")).
        column("col_idx")->as_dvector<size_t>().moveto_node_local();
      ret.data.mapv(to_ell_matrix_addcategory<float>,
                    col_idx, broadcast(i));
      current_logical_col += tmp2.num_row();
    } else {
      auto val = materialized.column(col)->as_dvector_float().
        moveto_node_local();
      ret.data.mapv(to_ell_matrix_addvalue<float>,
                    val, broadcast(current_logical_col), broadcast(i));
      current_logical_col++;
    }
  }
  return ret;
}

ell_matrix<float>
dftable_base::to_ell_matrix_float(const std::vector<std::string>& cols,
                                  const std::vector<std::string>& cats,
                                  dftable_to_sparse_info& info) {
  if(cols.size() == 0)
    throw std::runtime_error("to_ell_matrix: no colums to convert");
  auto materialized = select(cols); // cause exception if not applicable
  info.columns = cols;
  info.categories = cats;
  info.mapping_tables.clear();
  size_t current_logical_col = 0;
  std::unordered_set<std::string> cats_set(cats.begin(), cats.end());
  // first collect information like size
  for(size_t i = 0; i < cols.size(); i++) {
    if(cats_set.find(cols[i]) != cats_set.end()) {
      auto col = cols[i];
      auto tmp1 = materialized.select({col});
      auto tmp2 = tmp1.group_by({col}).select({col}).sort(col).
        materialize().rename(col, "key").
        append_rowid("col_idx", current_logical_col);
      info.mapping_tables.push_back(tmp2);
      current_logical_col += tmp2.num_row();
    } else {
      current_logical_col++;
    }
  }
  auto first_column = materialized.column(cols[0]);
  info.num_row = first_column->size();
  info.num_col = current_logical_col;
  // next, actually copies the data
  ell_matrix<float> ret;
  ret.data = make_node_local_allocate<ell_matrix_local<float>>();
  ret.num_row = info.num_row;
  ret.num_col = info.num_col;
  auto sizes = first_column->sizes();
  ret.data.mapv(to_ell_matrix_init<float>, broadcast(cols.size()),
                broadcast(sizes), broadcast(ret.num_col));
  size_t current_category = 0;
  current_logical_col = 0;
  for(size_t i = 0; i < cols.size(); i++) {
    auto col = cols[i];
    if(cats_set.find(col) != cats_set.end()) {
      auto tmp1 = materialized.select({col});
      auto tmp2 = info.mapping_tables[current_category++];
      auto col_idx = tmp1.bcast_join(tmp2, frovedis::eq(col, "key")).
        column("col_idx")->as_dvector<size_t>().moveto_node_local();
      ret.data.mapv(to_ell_matrix_addcategory<float>,
                    col_idx, broadcast(i));
      current_logical_col += tmp2.num_row();
    } else {
      auto val = materialized.column(col)->as_dvector_float().
        moveto_node_local();
      ret.data.mapv(to_ell_matrix_addvalue<float>,
                    val, broadcast(current_logical_col), broadcast(i));
      current_logical_col++;
    }
  }
  return ret;
}

ell_matrix<double>
dftable_base::to_ell_matrix_double(dftable_to_sparse_info& info) {
  auto& cols = info.columns;
  auto& cats = info.categories;
  if(cols.size() == 0)
    throw std::runtime_error("to_ell_matrix: no colums to convert");
  auto materialized = select(cols); // cause exception if not applicable
  std::unordered_set<std::string> cats_set(cats.begin(), cats.end());
  auto first_column = materialized.column(cols[0]);

  ell_matrix<double> ret;
  ret.data = make_node_local_allocate<ell_matrix_local<double>>();
  ret.num_row = info.num_row;
  ret.num_col = info.num_col;
  auto sizes = first_column->sizes();
  ret.data.mapv(to_ell_matrix_init<double>, broadcast(cols.size()),
                broadcast(sizes), broadcast(ret.num_col));
  size_t current_category = 0;
  size_t current_logical_col = 0;
  for(size_t i = 0; i < cols.size(); i++) {
    auto col = cols[i];
    if(cats_set.find(col) != cats_set.end()) {
      auto tmp1 = materialized.select({col});
      auto tmp2 = info.mapping_tables[current_category++];
      auto col_idx = tmp1.bcast_join(tmp2, frovedis::eq(col, "key")).
        column("col_idx")->as_dvector<size_t>().moveto_node_local();
      ret.data.mapv(to_ell_matrix_addcategory<double>,
                    col_idx, broadcast(i));
      current_logical_col += tmp2.num_row();
    } else {
      auto val = materialized.column(col)->as_dvector_double().
        moveto_node_local();
      ret.data.mapv(to_ell_matrix_addvalue<double>,
                    val, broadcast(current_logical_col), broadcast(i));
      current_logical_col++;
    }
  }
  return ret;
}

ell_matrix<double>
dftable_base::to_ell_matrix_double(const std::vector<std::string>& cols,
                                   const std::vector<std::string>& cats,
                                   dftable_to_sparse_info& info) {
  if(cols.size() == 0)
    throw std::runtime_error("to_ell_matrix: no colums to convert");
  auto materialized = select(cols); // cause exception if not applicable
  info.columns = cols;
  info.categories = cats;
  info.mapping_tables.clear();
  size_t current_logical_col = 0;
  std::unordered_set<std::string> cats_set(cats.begin(), cats.end());
  // first collect information like size
  for(size_t i = 0; i < cols.size(); i++) {
    if(cats_set.find(cols[i]) != cats_set.end()) {
      auto col = cols[i];
      auto tmp1 = materialized.select({col});
      auto tmp2 = tmp1.group_by({col}).select({col}).sort(col).
        materialize().rename(col, "key").
        append_rowid("col_idx", current_logical_col);
      info.mapping_tables.push_back(tmp2);
      current_logical_col += tmp2.num_row();
    } else {
      current_logical_col++;
    }
  }
  auto first_column = materialized.column(cols[0]);
  info.num_row = first_column->size();
  info.num_col = current_logical_col;
  // next, actually copies the data
  ell_matrix<double> ret;
  ret.data = make_node_local_allocate<ell_matrix_local<double>>();
  ret.num_row = info.num_row;
  ret.num_col = info.num_col;
  auto sizes = first_column->sizes();
  ret.data.mapv(to_ell_matrix_init<double>, broadcast(cols.size()),
                broadcast(sizes), broadcast(ret.num_col));
  size_t current_category = 0;
  current_logical_col = 0;
  for(size_t i = 0; i < cols.size(); i++) {
    auto col = cols[i];
    if(cats_set.find(col) != cats_set.end()) {
      auto tmp1 = materialized.select({col});
      auto tmp2 = info.mapping_tables[current_category++];
      auto col_idx = tmp1.bcast_join(tmp2, frovedis::eq(col, "key")).
        column("col_idx")->as_dvector<size_t>().moveto_node_local();
      ret.data.mapv(to_ell_matrix_addcategory<double>,
                    col_idx, broadcast(i));
      current_logical_col += tmp2.num_row();
    } else {
      auto val = materialized.column(col)->as_dvector_double().
        moveto_node_local();
      ret.data.mapv(to_ell_matrix_addvalue<double>,
                    val, broadcast(current_logical_col), broadcast(i));
      current_logical_col++;
    }
  }
  return ret;
}

crs_matrix<float>
dftable_base::to_crs_matrix_float(const std::vector<std::string>& cols,
                                  const std::vector<std::string>& cat,
                                  dftable_to_sparse_info& info) {
  return to_ell_matrix_float(cols, cat, info).to_crs_allow_zero();
}

crs_matrix<float>
dftable_base::to_crs_matrix_float(dftable_to_sparse_info& info) {
  return to_ell_matrix_float(info).to_crs_allow_zero();
}

crs_matrix<double>
dftable_base::to_crs_matrix_double(const std::vector<std::string>& cols,
                                   const std::vector<std::string>& cat,
                                   dftable_to_sparse_info& info) {
  return to_ell_matrix_double(cols, cat, info).to_crs_allow_zero();
}

crs_matrix<double>
dftable_base::to_crs_matrix_double(dftable_to_sparse_info& info) {
  return to_ell_matrix_double(info).to_crs_allow_zero();
}

std::shared_ptr<dfcolumn> dftable_base::column(const std::string& name) {
  auto ret = col.find(name);
  if(ret == col.end()) throw std::runtime_error("no such column: " + name);
  else return ret->second;
}
 
node_local<std::vector<size_t>> dftable_base::get_local_index() {
  if(col.size() == 0)
    throw std::runtime_error("get_local_index(): no columns");
  else return col.begin()->second->get_local_index();
}

void dftable_base::debug_print() {
  for(auto& cs: dftable_base::columns()) {
    std::cout << "column: " << cs << std::endl;
    dftable_base::column(cs)->debug_print();
  }
  std::cout << "row_size: " << row_size << std::endl;
}

// ---------- for dftable ----------

// might be reused for other type of tables (currently not)
void drop_impl(const std::string& name,
               std::map<std::string, std::shared_ptr<dfcolumn>>& col,
               std::vector<std::string>& col_order) {
  col.erase(name);
  col_order.erase(std::remove(col_order.begin(), col_order.end(), name),
                  col_order.end());
}

dftable& dftable::drop(const std::string& name) {
  drop_impl(name, col, col_order);
  return *this;
}

void rename_impl(const std::string& name, const std::string& name2,
                 std::map<std::string, std::shared_ptr<dfcolumn>>& col,
                 std::vector<std::string>& col_order) {
  auto ret = col.find(name);
  if(ret == col.end()) throw std::runtime_error("no such column: " + name);
  auto name_col = ret->second;
  if(col.find(name2) != col.end())
    throw std::runtime_error("column already exists: " + name2);
  col.erase(name);
  col[name2] = name_col;
  for(size_t i = 0; i < col_order.size(); i++) {
    if(col_order[i] == name) {
      col_order[i] = name2;
      break;
    }
  }
}

dftable& dftable::rename(const std::string& name, const std::string& name2) {
  rename_impl(name, name2, col, col_order);
  return *this;
}

dftable& dftable::append_column(const std::string& name,
                                const std::shared_ptr<dfcolumn>& c) {
  if(col.size() == 0) row_size = c->size();
  else if(c->size() != row_size)
    throw std::runtime_error("different size of columns");
  col.insert(std::make_pair(name, c));
  col_order.push_back(name);
  return *this;
}

struct append_rowid_helper {
  append_rowid_helper(){}
  append_rowid_helper(std::vector<size_t> sizes, size_t offset) :
    sizes(sizes), offset(offset) {}
  void operator()(std::vector<size_t>& v) {
    int self = get_selfid();
    size_t size = sizes[self];
    v.resize(size);
    size_t* vp = v.data();
    size_t start = 0;
    auto sizesp = sizes.data();
    for(size_t i = 0; i < self; i++) start += sizesp[i];
    start += offset;
    for(size_t i = 0; i < size; i++) vp[i] = start + i;
  }
  std::vector<size_t> sizes;
  size_t offset;
  SERIALIZE(sizes, offset)
};

dftable& dftable::append_rowid(const std::string& name, size_t offset) {
  if(col.size() == 0)
    throw std::runtime_error
      ("append_rowid: there is no column to append rowid");
  auto sizes = column(col_order[0])->sizes();
  auto nl = make_node_local_allocate<std::vector<size_t>>();
  nl.mapv(append_rowid_helper(sizes, offset));
  return append_column(name, nl.template moveto_dvector<size_t>());
}

std::vector<std::pair<std::string, size_t>>
add_contiguous_idx(std::vector<std::string>& vs, std::vector<size_t>& sizes) {
  int self = get_selfid();
  size_t toadd = 0;
  size_t* sizesp = sizes.data();
  for(size_t i = 0; i < self; i++) toadd += sizesp[i];
  size_t size = vs.size();
  std::vector<std::pair<std::string, size_t>> ret(size);
  std::pair<std::string, size_t>* retp = ret.data();
  std::string* vsp = vs.data();
  for(size_t i = 0; i < size; i++) {
    retp[i].first = vsp[i];
    retp[i].second = i + toadd;
  }
  return ret;
}

void prepare_string_load(my_map<std::string, std::vector<size_t>>& group,
                         my_map<std::string, size_t>& dic,
                         std::vector<std::string>& dic_idx,
                         std::vector<size_t>& cont_idx,
                         std::vector<size_t>& global_idx) {
  size_t selfid = static_cast<size_t>(get_selfid());
  size_t nodeinfo = selfid << DFNODESHIFT;
  auto size = group.size();
  dic_idx.resize(size);
  if(selfid == 0) {
    cont_idx.resize(size+1);
    global_idx.resize(size+1);
  } else {
    cont_idx.resize(size);
    global_idx.resize(size);
  }
  size_t i = 0;
  for(auto it = group.begin(); it != group.end(); ++it, ++i) {
    auto str = it->first;
    dic.insert(std::make_pair(str, i + nodeinfo));
    dic_idx[i] = str;
    cont_idx[i] = (it->second)[0];
  }
  size_t* global_idxp = global_idx.data();
  for(i = 0; i < size; i++) {
    global_idxp[i] = i + nodeinfo;
  }
  if(selfid == 0) {
    // for null
    cont_idx[size] = std::numeric_limits<size_t>::max();
    global_idx[size] = std::numeric_limits<size_t>::max();
  }
}

void dftable_offset_nulls(std::vector<size_t>& nulls,
                          const std::vector<size_t>& pxsizes) {
  auto selfid = get_selfid();
  auto lower = selfid == 0 ? 0 : pxsizes[selfid - 1];
  auto upper = pxsizes[selfid];
  auto lowerpos = std::lower_bound(nulls.begin(), nulls.end(), lower)
    - nulls.begin();
  auto upperpos = std::lower_bound(nulls.begin(), nulls.end(), upper)
    - nulls.begin();
  auto new_nulls_size = upperpos - lowerpos;
  std::vector<size_t> new_nulls(new_nulls_size);
  auto nullsp = nulls.data();
  auto new_nullsp = new_nulls.data();
  for(size_t i = 0; i < new_nulls_size; i++) {
    new_nullsp[i] = nullsp[lowerpos + i] - lower;
  }
  nulls.swap(new_nulls);
}

void dftable::load(const std::string& input) {
  std::string colfile = input + "/columns";
  std::string typefile = input + "/types";
  std::ifstream colstr;
  std::ifstream typestr;
  colstr.open(colfile.c_str());
  typestr.open(typefile.c_str());
  std::vector<std::string> cols;
  std::vector<std::string> types;
  std::string tmp;
  while(std::getline(colstr, tmp)) {
    cols.push_back(tmp);
  }
  while(std::getline(typestr, tmp)) {
    types.push_back(tmp);
  }
  for(size_t i = 0; i < cols.size(); i++) {
    std::string valfile = input + "/" + cols[i];
    std::string nullsfile = input + "/" + cols[i]+ "_nulls";
    if(types[i] == "int") {
      auto vec = make_dvector_loadbinary<int>(valfile);
      auto pxsizes = broadcast(prefix_sum(vec.sizes()));
      append_column(cols[i], std::move(vec));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      std::dynamic_pointer_cast<typed_dfcolumn<int>>
        (column(cols[i]))->nulls = std::move(nulls);
      column(cols[i])->contain_nulls_check();
    } else if(types[i] == "unsigned int") {
      auto vec = make_dvector_loadbinary<unsigned int>(valfile);
      auto pxsizes = broadcast(prefix_sum(vec.sizes()));
      append_column(cols[i], std::move(vec));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      std::dynamic_pointer_cast<typed_dfcolumn<unsigned int>>
        (column(cols[i]))->nulls = std::move(nulls);
      column(cols[i])->contain_nulls_check();
    } else if(types[i] == "long") {
      auto vec = make_dvector_loadbinary<long>(valfile);
      auto pxsizes = broadcast(prefix_sum(vec.sizes()));
      append_column(cols[i], std::move(vec));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      std::dynamic_pointer_cast<typed_dfcolumn<long>>
        (column(cols[i]))->nulls = std::move(nulls);
      column(cols[i])->contain_nulls_check();
    } else if(types[i] == "unsigned long") {
      auto vec = make_dvector_loadbinary<unsigned long>(valfile);
      auto pxsizes = broadcast(prefix_sum(vec.sizes()));
      append_column(cols[i], std::move(vec));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      std::dynamic_pointer_cast<typed_dfcolumn<unsigned long>>
        (column(cols[i]))->nulls = std::move(nulls);
      column(cols[i])->contain_nulls_check();
    } else if(types[i] == "float") {
      auto vec = make_dvector_loadbinary<float>(valfile);
      auto pxsizes = broadcast(prefix_sum(vec.sizes()));
      append_column(cols[i], std::move(vec));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      std::dynamic_pointer_cast<typed_dfcolumn<float>>
        (column(cols[i]))->nulls = std::move(nulls);
      column(cols[i])->contain_nulls_check();
    } else if(types[i] == "double") {
      auto vec = make_dvector_loadbinary<double>(valfile);
      auto pxsizes = broadcast(prefix_sum(vec.sizes()));
      append_column(cols[i], std::move(vec));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      std::dynamic_pointer_cast<typed_dfcolumn<double>>
        (column(cols[i]))->nulls = std::move(nulls);
      column(cols[i])->contain_nulls_check();
    } else if(types[i] == "string") {
      auto toappend = std::make_shared<typed_dfcolumn<std::string>>();
      auto loaddic = make_dvector_loadline<std::string>(valfile + "_dic");
      auto dicsizes = loaddic.sizes();
      auto withcontidx = loaddic.map_partitions(add_contiguous_idx,
                                                broadcast(dicsizes));
      auto group = withcontidx.group_by_key<std::string,size_t>();
      auto groupnl = group.viewas_node_local();
      toappend->dic = std::make_shared<dunordered_map<std::string,size_t>>
        (make_dunordered_map_allocate<std::string,size_t>());
      toappend->dic_idx = std::make_shared<node_local<std::vector<std::string>>>
        (make_node_local_allocate<std::vector<std::string>>());
      auto retdicnl = toappend->dic->viewas_node_local();
      auto cont_idx = make_node_local_allocate<std::vector<size_t>>();
      auto global_idx = make_node_local_allocate<std::vector<size_t>>();
      groupnl.mapv(prepare_string_load, retdicnl, *toappend->dic_idx, cont_idx,
                   global_idx);

      auto vec = make_dvector_loadbinary<size_t>(valfile + "_idx");
      std::vector<size_t> sizes;
      if(i != 0) {
        sizes = column(col_order[0])->sizes();
        vec.align_as(sizes);
      } else {
        sizes = vec.sizes();
      }
      auto pxsizes = broadcast(prefix_sum(sizes));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      toappend->nulls = std::move(nulls);
      toappend->contain_nulls_check();
      dftable t1, t2;
      t1.append_column("original", std::move(vec));
      t2.append_column("contiguous", cont_idx.moveto_dvector<size_t>());
      t2.append_column("global_idx", global_idx.moveto_dvector<size_t>());
      auto conved = t1.bcast_join(t2, frovedis::eq("original", "contiguous"))
        .as_dvector<size_t>("global_idx");
      toappend->val = conved.moveto_node_local();
      append_column(cols[i], std::move(toappend));
    } else if(types[i] == "dic_string") {
      auto toappend = std::make_shared<typed_dfcolumn<dic_string>>();
      auto loaddic = loadbinary_local<char>(valfile + "_dic");
      auto vintdic = vchar_to_int(loaddic);
      auto dicwords = split_to_words(vintdic, "\n");
      auto dic_to_use = make_dict_from_words(dicwords);
      toappend->dic = std::make_shared<node_local<dict>>(broadcast(dic_to_use));
      auto vec = make_dvector_loadbinary<size_t>(valfile + "_idx");
      std::vector<size_t> sizes;
      if(i != 0) {
        sizes = column(col_order[0])->sizes();
        vec.align_as(sizes);
      } else {
        sizes = vec.sizes();
      }
      auto pxsizes = broadcast(prefix_sum(sizes));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      toappend->nulls = std::move(nulls);
      toappend->contain_nulls_check();
      toappend->val = vec.moveto_node_local();
      append_column(cols[i], std::move(toappend));
    } else if(types[i] == "raw_string") {
      auto toappend = std::make_shared<typed_dfcolumn<raw_string>>();
      auto sep = make_node_local_allocate<std::vector<size_t>>();
      auto len = make_node_local_allocate<std::vector<size_t>>();
      auto loaded_text = load_text(valfile, "\n", sep, len);
      toappend->comp_words =
        loaded_text.map(+[](std::vector<int>& t,
                            std::vector<size_t>& s,
                            std::vector<size_t>& l) {
                          words w;
                          w.chars.swap(t);
                          w.starts.swap(s);
                          w.lens.swap(l);
                          return make_compressed_words(w);
                        }, sep, len);
      std::vector<size_t> sizes;
      if(i != 0) {
        sizes = column(col_order[0])->sizes();
        toappend->align_as(sizes);
      } else {
        sizes = toappend->sizes();
      }
      auto pxsizes = broadcast(prefix_sum(sizes));
      auto nulls = broadcast(make_dvector_loadbinary<size_t>(nullsfile)
                             .gather()).mapv(dftable_offset_nulls, pxsizes);
      toappend->nulls = std::move(nulls);
      toappend->contain_nulls_check();
      append_column(cols[i], std::move(toappend));
    }
  }
}

dftable make_dftable_load(const std::string& input) {
  dftable t;
  t.load(input);
  return t;
}

void dftable::loadtext(const std::string& filename,
                       const std::vector<std::string>& types,
                       int separator,
                       const std::string& nullstr,
                       bool is_crlf) {
  auto ret = make_dftable_loadtext(filename, types, separator, nullstr,
                                   is_crlf);
  *this = std::move(ret);
}

void dftable::loadtext(const std::string& filename,
                       const std::vector<std::string>& types,
                       const std::vector<std::string>& names,
                       int separator,
                       const std::string& nullstr,
                       bool is_crlf) {
  auto ret = make_dftable_loadtext(filename, types, names, separator, nullstr,
                                   is_crlf);
  *this = std::move(ret);
}

// ---------- for sorted_dftable ----------

dftable sorted_dftable::select(const std::vector<std::string>& cols) {
  dftable ret;
  for(size_t i = 0; i < cols.size(); i++) {
    ret.col[cols[i]] = column(cols[i]);
  }
  ret.row_size = global_idx.viewas_dvector<size_t>().size();
  ret.col_order = cols;
  return ret;
}

sorted_dftable sorted_dftable::sort(const std::string& name) {
  node_local<std::vector<size_t>> idx;
  auto sorted_column = column(name)->sort_with_idx(global_idx, idx);
  return sorted_dftable(*this, std::move(idx), name, std::move(sorted_column));
}

sorted_dftable sorted_dftable::sort_desc(const std::string& name) {
  node_local<std::vector<size_t>> idx;
  auto sorted_column = column(name)->sort_with_idx_desc(global_idx, idx);
  return sorted_dftable(*this, std::move(idx), name, std::move(sorted_column));
}

hash_joined_dftable
sorted_dftable::hash_join(dftable_base& right,
                          const std::shared_ptr<dfoperator>& op) {
  RLOG(DEBUG) << "calling hash_join after sort" << std::endl;
  return materialize().hash_join(right, op);
}

hash_joined_dftable
sorted_dftable::outer_hash_join(dftable_base& right,
                                const std::shared_ptr<dfoperator>& op) {
  RLOG(DEBUG) << "calling outer_hash_join after sort" << std::endl;
  return materialize().outer_hash_join(right, op);
}

bcast_joined_dftable
sorted_dftable::bcast_join(dftable_base& right,
                           const std::shared_ptr<dfoperator>& op) {
  RLOG(DEBUG) << "calling bcast_join after sort" << std::endl;
  return materialize().bcast_join(right, op);
}

bcast_joined_dftable
sorted_dftable::outer_bcast_join(dftable_base& right,
                                 const std::shared_ptr<dfoperator>& op) {
  RLOG(DEBUG) << "calling outer_bcast_join after sort" << std::endl;
  return materialize().outer_bcast_join(right, op);
}

star_joined_dftable
sorted_dftable::star_join
(const std::vector<dftable_base*>& dftables, 
 const std::vector<std::shared_ptr<dfoperator>>& op){
  return materialize().star_join(dftables, op);
}

grouped_dftable
sorted_dftable::group_by(const std::vector<std::string>& cols) {
  RLOG(DEBUG) << "calling group_by after sort" << std::endl;
  return materialize().group_by(cols);
}

std::shared_ptr<dfcolumn> sorted_dftable::column(const std::string& name) {
  if(name == column_name && is_cachable) return sorted_column;
  else {
    auto ret = col.find(name);
    if(ret == col.end()) throw std::runtime_error("no such column: " + name);
    else {
      auto col = (*ret).second;
      return col->global_extract(global_idx, to_store_idx, exchanged_idx);
    }
  }
}

void sorted_dftable::debug_print() {
  std::cout << "global_idx: " << std::endl;
  size_t nodemask = (size_t(1) << DFNODESHIFT) - 1;
  for(auto& i: global_idx.gather()) {
    for(auto j: i) {
      std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "to_store_idx: " << std::endl;
  for(auto& i: to_store_idx.gather()) {
    for(auto j: i) {
      std::cout << j << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "exchanged_idx: " << std::endl;
  for(auto& i: exchanged_idx.gather()) {
    for(auto j: i) {
      for(auto k: j) {
        std::cout << k << " ";
      }
      std::cout << "| ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "sorted_column: " << column_name << std::endl;
  dftable_base::debug_print();
}

/*
sorted_dftable& sorted_dftable::drop(const std::string& name) {
  if(name == column_name) {
    column_name = "";
    sorted_column = std::shared_ptr<dfcolumn>();
  }
  drop_impl(name, col, col_order);
  return *this;
}

sorted_dftable& sorted_dftable::rename(const std::string& name,
                                       const std::string& name2) {
  rename_impl(name, name2, col, col_order);
  if(name == column_name) column_name = name2;
  return *this;
}
*/

dftable sorted_dftable::append_rowid(const std::string& name,
                                     size_t offset) {
  return this->materialize().append_rowid(name, offset);
}

// ---------- hash_joined_dftable ----------

std::vector<size_t>
concat_idx(std::vector<size_t>& a, std::vector<size_t>& b) {
  size_t asize = a.size();
  size_t bsize = b.size();
  std::vector<size_t> ret(asize+bsize);
  size_t* ap = &a[0];
  size_t* bp = &b[0];
  size_t* retp = &ret[0];
  for(size_t i = 0; i < asize; i++) retp[i] = ap[i];
  for(size_t i = 0; i < bsize; i++) retp[asize+i] = bp[i];
  return ret;
}

size_t hash_joined_dftable::num_col() const {
    return dftable_base::num_col() + right.num_col();
}

size_t hash_joined_dftable::num_row() {
  return left_idx.viewas_dvector<size_t>().size();
}

std::vector<std::string> hash_joined_dftable::columns() const {
  std::vector<std::string> ret = dftable_base::columns();
  auto right_cols = right.columns();
  ret.insert(ret.end(), right_cols.begin(), right_cols.end());
  return ret;
}

dftable hash_joined_dftable::select(const std::vector<std::string>& cols) {    
  dftable ret;
  for(size_t i = 0; i < cols.size(); i++) {
    ret.col[cols[i]] = column(cols[i]);
  }
  ret.row_size = left_idx.viewas_dvector<size_t>().size();
  ret.col_order = cols;
  return ret;
}

// TODO: sort only on the specified column to create left/right idx
// it is possible but return type becomes hash_joined_dftable...
sorted_dftable hash_joined_dftable::sort(const std::string& name) {
  return materialize().sort(name);
}

sorted_dftable hash_joined_dftable::sort_desc(const std::string& name) {
  return materialize().sort_desc(name);
}

hash_joined_dftable
hash_joined_dftable::hash_join(dftable_base& right,
                               const std::shared_ptr<dfoperator>& op) {
  return materialize().hash_join(right, op);
}

hash_joined_dftable
hash_joined_dftable::outer_hash_join(dftable_base& right,
                                     const std::shared_ptr<dfoperator>& op) {
  return materialize().outer_hash_join(right, op);
}

bcast_joined_dftable
hash_joined_dftable::bcast_join(dftable_base& right,
                               const std::shared_ptr<dfoperator>& op) {
  return materialize().bcast_join(right, op);
}

bcast_joined_dftable
hash_joined_dftable::outer_bcast_join(dftable_base& right,
                                     const std::shared_ptr<dfoperator>& op) {
  return materialize().outer_bcast_join(right, op);
}

star_joined_dftable
hash_joined_dftable::star_join
(const std::vector<dftable_base*>& dftables, 
 const std::vector<std::shared_ptr<dfoperator>>& op){
  return materialize().star_join(dftables, op);
}

grouped_dftable
hash_joined_dftable::group_by(const std::vector<std::string>& cols) {
  return materialize().group_by(cols);
}

std::shared_ptr<dfcolumn>
hash_joined_dftable::column(const std::string& name) {
  auto left_ret = col.find(name);
  if(left_ret != col.end()) {
    auto c = (*left_ret).second;
    // if outer, left_idx is changed to contain nulls in ctor
    return c->global_extract(left_idx, left_to_store_idx,
                             left_exchanged_idx);
  } else {
    auto right_ret = right.col.find(name);
    if(right_ret != right.col.end()) {
      auto c = (*right_ret).second;
      if(is_outer) {
        auto retcol = c->global_extract(right_idx, right_to_store_idx,
                                        right_exchanged_idx);
        retcol->append_nulls(right_nulls);
        return retcol;
      } else {
        return c->global_extract(right_idx, right_to_store_idx,
                                 right_exchanged_idx);
      }
    } else {
      throw std::runtime_error("no such column: " + name);
    }
  }
}

void hash_joined_dftable::debug_print() {
  std::cout << "left: " << std::endl;
  dftable_base::debug_print();
  std::cout << "right: " << std::endl;
  right.debug_print();
  std::cout << "left_idx: " << std::endl;
  size_t nodemask = (size_t(1) << DFNODESHIFT) - 1;
  for(auto& i: left_idx.gather()) {
    for(auto j: i) {
      std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "right_idx: " << std::endl;
  for(auto& i: right_idx.gather()) {
    for(auto j: i) {
      std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "left_to_store_idx: " << std::endl;
  for(auto& i: left_to_store_idx.gather()) {
    for(auto j: i) {
      std::cout << j << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "right_to_store_idx: " << std::endl;
  for(auto& i: right_to_store_idx.gather()) {
    for(auto j: i) {
      std::cout << j << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "left_exchanged_idx: " << std::endl;
  for(auto& i: left_exchanged_idx.gather()) {
    for(auto j: i) {
      for(auto k: j) {
        std::cout << k << " ";
      }
      std::cout << "| ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "right_exchanged_idx: " << std::endl;
  for(auto& i: right_exchanged_idx.gather()) {
    for(auto j: i) {
      for(auto k: j) {
        std::cout << k << " ";
      }
      std::cout << "| ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  if(is_outer) {
    std::cout << "right_nulls: " << std::endl;
    for(auto& i: right_nulls.gather()) {
      for(auto j: i) {
        std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
      }
      std::cout << ": ";
    }
    std::cout << std::endl;
  }
}

dftable hash_joined_dftable::append_rowid(const std::string& name,
                                          size_t offset) {
  return this->materialize().append_rowid(name, offset);
}

// ---------- bcast_joined_dftable ----------

size_t bcast_joined_dftable::num_col() const {
  return dftable_base::num_col() + right.num_col();
}

size_t bcast_joined_dftable::num_row() {
  return left_idx.viewas_dvector<size_t>().size();
}

std::vector<std::string> bcast_joined_dftable::columns() const {
  std::vector<std::string> ret = dftable_base::columns();
  auto right_cols = right.columns();
  ret.insert(ret.end(), right_cols.begin(), right_cols.end());
  return ret;
}

dftable bcast_joined_dftable::select(const std::vector<std::string>& cols) {
  dftable ret;
  for(size_t i = 0; i < cols.size(); i++) {
    ret.col[cols[i]] = column(cols[i]);
  }
  ret.row_size = left_idx.viewas_dvector<size_t>().size();
  ret.col_order = cols;
  return ret;
}

// TODO: sort only on the specified column to create left/right idx
// it is possible but return type becomes hash_joined_dftable...
sorted_dftable bcast_joined_dftable::sort(const std::string& name) {
  return materialize().sort(name);
}

sorted_dftable bcast_joined_dftable::sort_desc(const std::string& name) {
  return materialize().sort_desc(name);
}

hash_joined_dftable
bcast_joined_dftable::hash_join(dftable_base& right,
                                const std::shared_ptr<dfoperator>& op) {
  return materialize().hash_join(right, op);
}

hash_joined_dftable
bcast_joined_dftable::outer_hash_join(dftable_base& right,
                                      const std::shared_ptr<dfoperator>& op) {
  return materialize().outer_hash_join(right, op);
}

bcast_joined_dftable
bcast_joined_dftable::bcast_join(dftable_base& right,
                                 const std::shared_ptr<dfoperator>& op) {
  return materialize().bcast_join(right, op);
}

bcast_joined_dftable
bcast_joined_dftable::outer_bcast_join(dftable_base& right,
                                       const std::shared_ptr<dfoperator>& op) {
  return materialize().outer_bcast_join(right, op);
}

star_joined_dftable
bcast_joined_dftable::star_join
(const std::vector<dftable_base*>& dftables, 
 const std::vector<std::shared_ptr<dfoperator>>& op){
  return materialize().star_join(dftables, op);
}

grouped_dftable
bcast_joined_dftable::group_by(const std::vector<std::string>& cols) {
  return materialize().group_by(cols);
}

std::shared_ptr<dfcolumn>
bcast_joined_dftable::column(const std::string& name) {
  auto left_ret = col.find(name);
  if(left_ret != col.end()) {
    auto c = (*left_ret).second;
    return c->extract(left_idx);
  } else {
    auto right_ret = right.col.find(name);
    if(right_ret != right.col.end()) {
      auto c = (*right_ret).second;
      if(is_outer) {
        auto retcol = c->global_extract(right_idx, right_to_store_idx,
                                        right_exchanged_idx);
        retcol->append_nulls(right_nulls);
        return retcol;
      } else {
        return c->global_extract(right_idx, right_to_store_idx,
                                 right_exchanged_idx);
      }
    } else {
      throw std::runtime_error("no such column: " + name);
    }
  }
}

void bcast_joined_dftable::debug_print() {
  std::cout << "left: " << std::endl;
  dftable_base::debug_print();
  std::cout << "right: " << std::endl;
  right.debug_print();
  std::cout << "left_idx: " << std::endl;
  for(auto& i: left_idx.gather()) {
    for(auto j: i) {
      std::cout << j << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  size_t nodemask = (size_t(1) << DFNODESHIFT) - 1;
  std::cout << "right_idx: " << std::endl;
  for(auto& i: right_idx.gather()) {
    for(auto j: i) {
      std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  if(is_outer) {
    std::cout << "right_nulls: " << std::endl;
    for(auto& i: right_nulls.gather()) {
      for(auto j: i) {
        std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
      }
      std::cout << ": ";
    }
    std::cout << std::endl;
  }
}

dftable bcast_joined_dftable::append_rowid(const std::string& name,
                                          size_t offset) {
  return this->materialize().append_rowid(name, offset);
}

// ---------- star_joined_dftable ----------

size_t star_joined_dftable::num_col() const {
  size_t total = dftable_base::num_col();
  for(size_t i = 0; i < rights.size(); i++) total += rights[i].num_col();
  return total;
}

size_t star_joined_dftable::num_row() {
  return left_idx.viewas_dvector<size_t>().size();
}

std::vector<std::string> star_joined_dftable::columns() const {
  std::vector<std::string> ret = dftable_base::columns();
  for(size_t i = 0; i < rights.size(); i++) {
    auto right_cols = rights[i].columns();
    ret.insert(ret.end(), right_cols.begin(), right_cols.end());
  }
  return ret;
}

dftable star_joined_dftable::select(const std::vector<std::string>& cols) {    
  dftable ret;
  for(size_t i = 0; i < cols.size(); i++) {
    ret.col[cols[i]] = column(cols[i]);
  }
  ret.row_size = left_idx.viewas_dvector<size_t>().size();
  ret.col_order = cols;
  return ret;
}

sorted_dftable star_joined_dftable::sort(const std::string& name) {
  return materialize().sort(name);
}

sorted_dftable star_joined_dftable::sort_desc(const std::string& name) {
  return materialize().sort_desc(name);
}

hash_joined_dftable
star_joined_dftable::hash_join(dftable_base& right,
                               const std::shared_ptr<dfoperator>& op) {
  return materialize().hash_join(right, op);
}

hash_joined_dftable
star_joined_dftable::outer_hash_join(dftable_base& right,
                                     const std::shared_ptr<dfoperator>& op) {
  return materialize().outer_hash_join(right, op);
}

bcast_joined_dftable
star_joined_dftable::bcast_join(dftable_base& right,
                                const std::shared_ptr<dfoperator>& op) {
  return materialize().bcast_join(right, op);
}

star_joined_dftable
star_joined_dftable::star_join
(const std::vector<dftable_base*>& dftables, 
 const std::vector<std::shared_ptr<dfoperator>>& op){
  return materialize().star_join(dftables, op);
}

bcast_joined_dftable
star_joined_dftable::outer_bcast_join(dftable_base& right,
                                      const std::shared_ptr<dfoperator>& op) {
  return materialize().outer_bcast_join(right, op);
}

grouped_dftable
star_joined_dftable::group_by(const std::vector<std::string>& cols) {
  return materialize().group_by(cols);
}

std::shared_ptr<dfcolumn>
star_joined_dftable::column(const std::string& name) {
  auto left_ret = col.find(name);
  if(left_ret != col.end()) {
    auto c = (*left_ret).second;
    return c->extract(left_idx);
  } else {
    for(size_t i = 0; i < rights.size(); i++) {
      auto right_ret = rights[i].col.find(name);
      if(right_ret != rights[i].col.end()) {
        auto c = (*right_ret).second;
        return c->global_extract(right_idxs[i], right_to_store_idxs[i],
                                 right_exchanged_idxs[i]);
      }
    }
    throw std::runtime_error("no such column: " + name);
  }
}

void star_joined_dftable::debug_print() {
  std::cout << "left: " << std::endl;
  dftable_base::debug_print();
  std::cout << "rights: " << std::endl;
  for(size_t i = 0; i < rights.size(); i++) {
    rights[i].debug_print();
  }
  std::cout << "left_idx: " << std::endl;
  for(auto& i: left_idx.gather()) {
    for(auto j: i) {
      std::cout << j << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  size_t nodemask = (size_t(1) << DFNODESHIFT) - 1;
  std::cout << "right_idxs: " << std::endl;
  for(size_t k = 0; k < rights.size(); k++) {
    for(auto& i: right_idxs[k].gather()) {
      for(auto j: i) {
        std::cout << (j >> DFNODESHIFT) << "-" << (j & nodemask) << " ";
      }
      std::cout << ": ";
    }
    std::cout << std::endl;
  }
}

dftable star_joined_dftable::append_rowid(const std::string& name,
                                          size_t offset) {
  return this->materialize().append_rowid(name, offset);
}

// ---------- grouped_dftable ----------

dftable grouped_dftable::select(const std::vector<std::string>& cols) {
  dftable ret_table;
  size_t colssize = cols.size();
  for(size_t i = 0; i < colssize; i++) {
    bool in_grouped_cols = false;
    for(size_t j = 0; j < grouped_col_names.size(); j++) {
      if(cols[i] == grouped_col_names[j]) {
        in_grouped_cols = true; break;
      }
    }
    if(!in_grouped_cols)
      throw std::runtime_error("select not grouped column");
    ret_table.col[cols[i]] = grouped_cols[i];
  }
  ret_table.row_size = num_row();
  ret_table.col_order = cols;
  return ret_table;
}

dftable
grouped_dftable::select(const std::vector<std::string>& cols,
                        const std::vector<std::shared_ptr<dfaggregator>>& aggs) {
  dftable ret_table;
  size_t colssize = cols.size();
  if(colssize != 0) {
    for(size_t i = 0; i < colssize; i++) {
      bool in_grouped_cols = false;
      for(size_t j = 0; j < grouped_col_names.size(); j++) {
        if(cols[i] == grouped_col_names[j]) {
          in_grouped_cols = true; break;
        }
      }
      if(!in_grouped_cols)
        throw std::runtime_error("select not grouped column");
      ret_table.col[cols[i]] = grouped_cols[i];
    }
  }
  ret_table.col_order = cols;
  size_t aggssize = aggs.size();
  auto row_sizes = grouped_cols[0]->sizes();
  auto nl_row_sizes = make_node_local_scatter(row_sizes);
  for(size_t i = 0; i < aggssize; i++) {
    auto newcol = aggs[i]->aggregate(org_table,
                                     local_grouped_idx,
                                     local_idx_split,
                                     hash_divide,
                                     merge_map,
                                     nl_row_sizes);
    if(aggs[i]->has_as) {
      if(ret_table.col.find(aggs[i]->as) != ret_table.col.end())
        throw std::runtime_error
          ("grouped_dftable::select: same column name already exists");
      ret_table.col[aggs[i]->as] = newcol;
      ret_table.col_order.push_back(aggs[i]->as);
    } else {
      if(ret_table.col.find(aggs[i]->col) != ret_table.col.end())
        throw std::runtime_error
          ("grouped_dftable::select: same column name already exists");
      ret_table.col[aggs[i]->col] = newcol;
      ret_table.col_order.push_back(aggs[i]->col);
    }
  }
  ret_table.row_size = num_row();
  return ret_table;
}

void grouped_dftable::debug_print() {
  std::cout << "table: " << std::endl;
  org_table.debug_print();
  std::cout << "local_grouped_idx: " << std::endl;
  for(auto& i: local_grouped_idx.gather()) {
    for(auto j: i) {
      std::cout << j << " ";
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "local_idx_split: " << std::endl;
  for(auto& i: local_idx_split.gather()) {
    for(auto j: i) {
        std::cout << j << " "; 
    }
    std::cout << ": ";
  }
  std::cout << std::endl;
  std::cout << "hash_divide: " << std::endl;
  for(auto& i: hash_divide.gather()) {
    for(auto& j: i) {
      for(auto k: j) {
        std::cout << k << " "; 
      }
      std::cout << ": ";
    }
    std::cout << "| ";
  }
  std::cout << std::endl;
  std::cout << "merge_map: " << std::endl;
  for(auto& i: merge_map.gather()) {
    for(auto& j: i) {
      for(auto k: j) {
        std::cout << k << " "; 
      }
      std::cout << ": ";
    }
    std::cout << "| ";
  }
  std::cout << std::endl;
  std::cout << "grouped_cols: " << std::endl;
  for(auto& i: grouped_cols) i->debug_print();
  std::cout << "grouped_col_names: " << std::endl;
  for(auto& i: grouped_col_names) std::cout << i << " ";
  std::cout << std::endl;
}

}
