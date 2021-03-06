"""
rpclib.py
"""
#!/usr/bin/env python

# This source provides the interfaces to call the C/C++ functions from python
# code

import numpy as np
from ctypes import c_char_p, c_int, c_ulong, c_short, c_float, c_double,\
                   c_long, c_bool, c_char, c_void_p, CDLL, py_object, POINTER
from numpy.ctypeslib import ndpointer
from scipy.sparse.csr import csr_matrix

# A dynamic library containing implementation of server side code
try:
    LIB = CDLL("libfrovedis_client_python.so")
except OSError:
    try:
        LIB = CDLL("../lib/libfrovedis_client_python.so")
    except OSError:
        raise OSError("libfrovedis_client_python.so: No such dll found \
                      (set LD_LIBRARY_PATH)")

# --- Frovedis Server ---
initialize_server = LIB.initialize_server
initialize_server.argtypes = [c_char_p]
initialize_server.restype = py_object

get_worker_size = LIB.get_worker_size
get_worker_size.argtypes = [c_char_p, c_int]
get_worker_size.restype = c_int

clean_server = LIB.clean_server
clean_server.argtypes = [c_char_p, c_int]

finalize_server = LIB.finalize_server
finalize_server.argtypes = [c_char_p, c_int]

check_server_exception = LIB.check_server_exception
check_server_exception.restype = py_object

# --- Frovedis dvector ---
# create from numpy array
create_frovedis_int_dvector = LIB.create_frovedis_int_dvector
create_frovedis_int_dvector.argtypes = [c_char_p, c_int,\
                            ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                  c_int]
create_frovedis_int_dvector.restype = py_object

create_frovedis_long_dvector = LIB.create_frovedis_long_dvector
create_frovedis_long_dvector.argtypes = [c_char_p, c_int,\
                           ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                  c_int]
create_frovedis_long_dvector.restype = py_object

create_frovedis_float_dvector = LIB.create_frovedis_float_dvector
create_frovedis_float_dvector.argtypes = [c_char_p, c_int,\
                          ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                  c_int]
create_frovedis_float_dvector.restype = py_object

create_frovedis_double_dvector = LIB.create_frovedis_double_dvector
create_frovedis_double_dvector.argtypes = [c_char_p, c_int,\
                         ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                  c_int]
create_frovedis_double_dvector.restype = py_object

create_frovedis_string_dvector = LIB.create_frovedis_string_dvector
create_frovedis_string_dvector.argtypes = [c_char_p, c_int, POINTER(c_char_p),\
                                           c_int]
create_frovedis_string_dvector.restype = py_object

#To Print Created dvector

show_frovedis_dvector = LIB.show_frovedis_dvector
show_frovedis_dvector.argtypes = [c_char_p, c_int, c_long, c_int]

release_frovedis_dvector = LIB.release_frovedis_dvector
release_frovedis_dvector.argtypes = [c_char_p, c_int, c_long, c_int]

#------ Frovedis Vector
create_frovedis_int_vector = LIB.create_frovedis_int_vector
create_frovedis_int_vector.argtypes = [c_char_p, c_int, 
                                       POINTER(c_int), c_ulong, c_short]
create_frovedis_int_vector.restype = py_object

create_frovedis_long_vector = LIB.create_frovedis_long_vector
create_frovedis_long_vector.argtypes = [c_char_p, c_int, 
                                        POINTER(c_long), c_ulong, c_short]
create_frovedis_long_vector.restype = py_object

create_frovedis_float_vector = LIB.create_frovedis_float_vector
create_frovedis_float_vector.argtypes = [c_char_p, c_int, 
                                         POINTER(c_float), c_ulong, c_short]
create_frovedis_float_vector.restype = py_object

create_frovedis_double_vector = LIB.create_frovedis_double_vector
create_frovedis_double_vector.argtypes = [c_char_p, c_int, 
                                          POINTER(c_double), c_ulong, c_short]
create_frovedis_double_vector.restype = py_object

create_frovedis_string_vector = LIB.create_frovedis_string_vector
create_frovedis_string_vector.argtypes = [c_char_p, c_int, 
                                          POINTER(c_char_p), c_ulong, c_short]
create_frovedis_string_vector.restype = py_object

get_frovedis_array = LIB.get_frovedis_array
get_frovedis_array.argtypes = [c_char_p, c_int, c_long, c_short]
get_frovedis_array.restype = py_object

get_float_array = LIB.get_float_array
get_float_array.argtypes = [c_char_p, c_int, c_long,
                            ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS")]

get_double_array = LIB.get_double_array
get_double_array.argtypes = [c_char_p, c_int, c_long,
                             ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS")]

save_frovedis_vector_client = LIB.save_frovedis_vector_client
save_frovedis_vector_client.argtypes = [c_char_p, c_int, # host, port
                                        c_long,  # data pointer
                                        c_char_p, #path
                                        c_bool, #is_binary
                                        c_short #data type
                                        ]

load_frovedis_vector_client = LIB.load_frovedis_vector_client
load_frovedis_vector_client.argtypes = [c_char_p, c_int, # host, port
                                        c_char_p, #path
                                        c_bool, #is_binary
                                        c_short #data type
                                        ]
load_frovedis_vector_client.restype = py_object

release_frovedis_array = LIB.release_frovedis_array
release_frovedis_array.argtypes = [c_char_p, c_int, c_long, c_short]


#----Frovedis Dataframe from Python--------------------
create_frovedis_dataframe = LIB.create_frovedis_dataframe
create_frovedis_dataframe.argtypes = [c_char_p, c_int, POINTER(c_short),
                                      POINTER(c_char_p), POINTER(c_long), c_int]
create_frovedis_dataframe.restype = c_long

show_frovedis_dataframe = LIB.show_frovedis_dataframe
show_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long]

release_frovedis_dataframe = LIB.release_frovedis_dataframe
release_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long]

release_dfoperator = LIB.release_dfoperator
release_dfoperator.argtypes = [c_char_p, c_int, c_long]

get_frovedis_dfoperator = LIB.get_frovedis_dfoperator
get_frovedis_dfoperator.argtypes = [c_char_p, c_int, c_char_p, c_char_p,\
                                    c_short, c_short, c_bool]
get_frovedis_dfoperator.restype = c_long

get_dfANDoperator = LIB.get_frovedis_dfANDoperator
get_dfANDoperator.argtypes = [c_char_p, c_int, c_long, c_long]
get_dfANDoperator.restype = c_long

get_dfORoperator = LIB.get_frovedis_dfORoperator
get_dfORoperator.argtypes = [c_char_p, c_int, c_long, c_long]
get_dfORoperator.restype = c_long

filter_frovedis_dataframe = LIB.filter_frovedis_dataframe
filter_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long, c_long]
filter_frovedis_dataframe.restype = c_long

select_frovedis_dataframe = LIB.select_frovedis_dataframe
select_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long, \
                                      POINTER(c_char_p), c_int]
select_frovedis_dataframe.restype = c_long

sort_frovedis_dataframe = LIB.sort_frovedis_dataframe
sort_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long, \
                                    POINTER(c_char_p), c_int, c_bool]
sort_frovedis_dataframe.restype = c_long

group_frovedis_dataframe = LIB.group_frovedis_dataframe
group_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long, \
                                     POINTER(c_char_p), c_int]
group_frovedis_dataframe.restype = c_long

agg_grouped_dataframe = LIB.agg_grouped_dataframe
agg_grouped_dataframe.argtypes = [c_char_p, c_int, c_long,
                                  POINTER(c_char_p), c_ulong,
                                  POINTER(c_char_p), POINTER(c_char_p),
                                  POINTER(c_char_p), c_ulong]
agg_grouped_dataframe.restype = c_long

merge_frovedis_dataframe = LIB.join_frovedis_dataframe
merge_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long, c_long, c_long,
                                     c_char_p, c_char_p]
merge_frovedis_dataframe.restype = c_long

rename_frovedis_dataframe = LIB.rename_frovedis_dataframe
rename_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                      POINTER(c_char_p), POINTER(c_char_p),\
                                       c_int]
rename_frovedis_dataframe.restype = c_long

get_min_frovedis_dataframe = LIB.min_frovedis_dataframe
get_min_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                       POINTER(c_char_p), POINTER(c_short),\
                                       c_int]
get_min_frovedis_dataframe.restype = py_object

get_max_frovedis_dataframe = LIB.max_frovedis_dataframe
get_max_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                       POINTER(c_char_p), POINTER(c_short),\
                                       c_int]
get_max_frovedis_dataframe.restype = py_object

get_sum_frovedis_dataframe = LIB.sum_frovedis_dataframe
get_sum_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                       POINTER(c_char_p), POINTER(c_short),\
                                       c_int]
get_sum_frovedis_dataframe.restype = py_object

get_avg_frovedis_dataframe = LIB.avg_frovedis_dataframe
get_avg_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                       POINTER(c_char_p), c_int]
get_avg_frovedis_dataframe.restype = py_object

get_cnt_frovedis_dataframe = LIB.cnt_frovedis_dataframe
get_cnt_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                       POINTER(c_char_p), c_int]
get_cnt_frovedis_dataframe.restype = py_object

get_std_frovedis_dataframe = LIB.std_frovedis_dataframe
get_std_frovedis_dataframe.argtypes = [c_char_p, c_int, c_long,
                                       POINTER(c_char_p), POINTER(c_short),\
                                       c_int]
get_std_frovedis_dataframe.restype = py_object

get_frovedis_col = LIB.get_frovedis_col
get_frovedis_col.argtypes = [c_char_p, c_int, c_long, c_char_p, c_short]
get_frovedis_col.restype = py_object

df_to_rowmajor = LIB.df_to_rowmajor
df_to_rowmajor.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                           POINTER(c_char_p), c_int, #t_cols_arr, size
                           c_short]                  #dtype
df_to_rowmajor.restype = py_object

df_to_colmajor = LIB.df_to_colmajor
df_to_colmajor.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                           POINTER(c_char_p), c_int, #t_cols_arr, size
                           c_short]                  #dtype
df_to_colmajor.restype = py_object

df_to_crs = LIB.df_to_crs
df_to_crs.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                      POINTER(c_char_p), c_int, #t_cols_arr, size1
                      POINTER(c_char_p), c_int, #cat_cols_arr, size2
                      c_long, c_short]          #info_id, dtype
df_to_crs.restype = py_object

df_to_crs_using_info = LIB.df_to_crs_using_info
df_to_crs_using_info.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                                 c_long, c_short]          #info_id, dtype
df_to_crs_using_info.restype = py_object

# --- Frovedis dftable_to_sparse_info ---
load_dftable_to_sparse_info = LIB.load_dftable_to_sparse_info
load_dftable_to_sparse_info.argtypes = [c_char_p, c_int,  #host, port
                                        c_long, c_char_p] #info_id, dirname

save_dftable_to_sparse_info = LIB.save_dftable_to_sparse_info
save_dftable_to_sparse_info.argtypes = [c_char_p, c_int,  #host, port
                                        c_long, c_char_p] #info_id, dirname

release_dftable_to_sparse_info = LIB.release_dftable_to_sparse_info
release_dftable_to_sparse_info.argtypes = [c_char_p, c_int, c_long]  #host,\
                                           # port, info_id

# --- Frovedis sparse matrices ---
# create from scipy matrix

create_frovedis_crs_II_matrix = LIB.create_frovedis_crs_II_matrix
create_frovedis_crs_II_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_int, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_int, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          c_ulong]
create_frovedis_crs_II_matrix.restype = py_object

create_frovedis_crs_IL_matrix = LIB.create_frovedis_crs_IL_matrix
create_frovedis_crs_IL_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_int, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          c_ulong]
create_frovedis_crs_IL_matrix.restype = py_object

create_frovedis_crs_LI_matrix = LIB.create_frovedis_crs_LI_matrix
create_frovedis_crs_LI_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_int, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          c_ulong]
create_frovedis_crs_LI_matrix.restype = py_object

create_frovedis_crs_LL_matrix = LIB.create_frovedis_crs_LL_matrix
create_frovedis_crs_LL_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          c_ulong]
create_frovedis_crs_LL_matrix.restype = py_object

create_frovedis_crs_FI_matrix = LIB.create_frovedis_crs_FI_matrix
create_frovedis_crs_FI_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_float, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_int, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          c_ulong]
create_frovedis_crs_FI_matrix.restype = py_object

create_frovedis_crs_FL_matrix = LIB.create_frovedis_crs_FL_matrix
create_frovedis_crs_FL_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_float, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          c_ulong]
create_frovedis_crs_FL_matrix.restype = py_object

create_frovedis_crs_DI_matrix = LIB.create_frovedis_crs_DI_matrix
create_frovedis_crs_DI_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_double, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_int, ndim=1,\
                                          flags="C_CONTIGUOUS"),
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),
                                          c_ulong]
create_frovedis_crs_DI_matrix.restype = py_object

create_frovedis_crs_DL_matrix = LIB.create_frovedis_crs_DL_matrix
create_frovedis_crs_DL_matrix.argtypes = [c_char_p, c_int,
                                          c_ulong, c_ulong,
                                          ndpointer(c_double, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),\
                                          ndpointer(c_long, ndim=1,\
                                          flags="C_CONTIGUOUS"),
                                          c_ulong]
create_frovedis_crs_DL_matrix.restype = py_object

get_crs_matrix_components = LIB.get_crs_matrix_components
get_crs_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         c_void_p,\
                                         c_void_p,\
                                         c_void_p,\
                                         c_short, c_short,\
                                         c_ulong, c_ulong]

transpose_frovedis_sparse_matrix = LIB.transpose_frovedis_sparse_matrix
transpose_frovedis_sparse_matrix.argtypes = [c_char_p, c_int,\
                                            c_long, c_short, c_short]
transpose_frovedis_sparse_matrix.restype = py_object


compute_spmv = LIB.compute_spmv
compute_spmv.argtypes = [c_char_p, c_int, \
                         c_long, c_long, \
                         c_short, c_short]
compute_spmv.restype = py_object

'''
#get crs II matrix
get_crs_II_matrix_components = LIB.get_crs_II_matrix_components
get_crs_II_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_int, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_int, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]

#get crs IL matrix
get_crs_IL_matrix_components = LIB.get_crs_IL_matrix_components
get_crs_IL_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_int, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]

#get crs LI matrix
get_crs_LI_matrix_components = LIB.get_crs_LI_matrix_components
get_crs_LI_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_int, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]

#get crs LL matrix
get_crs_LL_matrix_components = LIB.get_crs_LL_matrix_components
get_crs_LL_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]

#get crs FI matrix
get_crs_FI_matrix_components = LIB.get_crs_FI_matrix_components
get_crs_FI_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_float, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_int, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]


#get crs FL matrix
get_crs_FL_matrix_components = LIB.get_crs_FL_matrix_components
get_crs_FL_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_float, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]


#get crs DI matrix
get_crs_DI_matrix_components = LIB.get_crs_DI_matrix_components
get_crs_DI_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_double, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_int, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]


#get crs DL matrix
get_crs_DL_matrix_components = LIB.get_crs_DL_matrix_components
get_crs_DL_matrix_components.argtypes = [c_char_p, c_int,\
                                         c_ulong,\
                                         ndpointer(c_double, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS"),\
                                         ndpointer(c_long, ndim=1,\
                                         flags="C_CONTIGUOUS")]

'''
# load from text/bin file
load_frovedis_crs_matrix = LIB.load_frovedis_crs_matrix
load_frovedis_crs_matrix.argtypes = [c_char_p, c_int,
                                     c_char_p, c_bool,
                                     c_short, c_short]
load_frovedis_crs_matrix.restype = py_object

save_frovedis_crs_matrix = LIB.save_frovedis_crs_matrix
save_frovedis_crs_matrix.argtypes = [c_char_p, c_int,
                                     c_long, c_char_p, c_bool,
                                     c_short, c_short]

release_frovedis_crs_matrix = LIB.release_frovedis_crs_matrix
release_frovedis_crs_matrix.argtypes = [c_char_p, c_int, c_long,
                                        c_short, c_short]

show_frovedis_crs_matrix = LIB.show_frovedis_crs_matrix
show_frovedis_crs_matrix.argtypes = [c_char_p, c_int, c_long,
                                     c_short, c_short]

# --- Frovedis Dense matrices ---
# create from numpy matrix
create_frovedis_double_dense_matrix = LIB.create_frovedis_double_dense_matrix
create_frovedis_double_dense_matrix.argtypes = [c_char_p, c_int,\
                                       c_ulong, c_ulong,\
                                       ndpointer(c_double, ndim=1,\
                                       flags="C_CONTIGUOUS"),\
                                       c_char]
create_frovedis_double_dense_matrix.restype = py_object

create_frovedis_float_dense_matrix = LIB.create_frovedis_float_dense_matrix
create_frovedis_float_dense_matrix.argtypes = [c_char_p, c_int,\
                                       c_ulong, c_ulong,\
                                       ndpointer(c_float, ndim=1,\
                                       flags="C_CONTIGUOUS"),\
                                       c_char]
create_frovedis_float_dense_matrix.restype = py_object



create_frovedis_long_dense_matrix = LIB.create_frovedis_long_dense_matrix
create_frovedis_long_dense_matrix.argtypes = [c_char_p, c_int,\
                                       c_ulong, c_ulong,\
                                       ndpointer(c_long, ndim=1,\
                                       flags="C_CONTIGUOUS"),\
                                       c_char]
create_frovedis_long_dense_matrix.restype = py_object



create_frovedis_int_dense_matrix = LIB.create_frovedis_int_dense_matrix
create_frovedis_int_dense_matrix.argtypes = [c_char_p, c_int,\
                                       c_ulong, c_ulong,\
                                       ndpointer(c_int, ndim=1,\
                                       flags="C_CONTIGUOUS"),\
                                       c_char]
create_frovedis_int_dense_matrix.restype = py_object


# load from text/bin file
load_frovedis_dense_matrix = LIB.load_frovedis_dense_matrix
load_frovedis_dense_matrix.argtypes = [c_char_p, c_int,
                                       c_char_p, c_bool, c_char, c_short]
load_frovedis_dense_matrix.restype = py_object

save_frovedis_dense_matrix = LIB.save_frovedis_dense_matrix
save_frovedis_dense_matrix.argtypes = [c_char_p, c_int,
                                       c_long, c_char_p,
                                       c_bool, c_char, c_short]

transpose_frovedis_dense_matrix = LIB.transpose_frovedis_dense_matrix
transpose_frovedis_dense_matrix.argtypes = [c_char_p, c_int,
                                            c_long, c_char, c_short]
transpose_frovedis_dense_matrix.restype = py_object

copy_frovedis_dense_matrix = LIB.copy_frovedis_dense_matrix
copy_frovedis_dense_matrix.argtypes = [c_char_p, c_int, c_long, c_char,\
                                       c_short]
copy_frovedis_dense_matrix.restype = py_object

release_frovedis_dense_matrix = LIB.release_frovedis_dense_matrix
release_frovedis_dense_matrix.argtypes = [c_char_p, c_int, c_long, c_char,\
                                          c_short]

show_frovedis_dense_matrix = LIB.show_frovedis_dense_matrix
show_frovedis_dense_matrix.argtypes = [c_char_p, c_int, c_long, c_char,\
                                       c_short]

get_frovedis_rowmatrix = LIB.get_frovedis_rowmatrix
get_frovedis_rowmatrix.argtypes = [c_char_p, c_int, c_long,
                                   c_ulong, c_ulong,
                                   c_char, c_short]
get_frovedis_rowmatrix.restype = py_object

get_double_rowmajor_array_as_int_array = LIB.get_double_rowmajor_array_as_int_array
get_double_rowmajor_array_as_int_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_double_rowmajor_array_as_long_array = LIB.get_double_rowmajor_array_as_long_array
get_double_rowmajor_array_as_long_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_double_rowmajor_array_as_float_array = LIB.get_double_rowmajor_array_as_float_array
get_double_rowmajor_array_as_float_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_double_rowmajor_array_as_double_array = LIB.get_double_rowmajor_array_as_double_array
get_double_rowmajor_array_as_double_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_float_rowmajor_array_as_int_array = LIB.get_float_rowmajor_array_as_int_array
get_float_rowmajor_array_as_int_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_float_rowmajor_array_as_long_array = LIB.get_float_rowmajor_array_as_long_array
get_float_rowmajor_array_as_long_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_float_rowmajor_array_as_float_array = LIB.get_float_rowmajor_array_as_float_array
get_float_rowmajor_array_as_float_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_float_rowmajor_array_as_double_array = LIB.get_float_rowmajor_array_as_double_array
get_float_rowmajor_array_as_double_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_long_rowmajor_array_as_int_array = LIB.get_long_rowmajor_array_as_int_array
get_long_rowmajor_array_as_int_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_long_rowmajor_array_as_long_array = LIB.get_long_rowmajor_array_as_long_array
get_long_rowmajor_array_as_long_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_long_rowmajor_array_as_float_array = LIB.get_long_rowmajor_array_as_float_array
get_long_rowmajor_array_as_float_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_long_rowmajor_array_as_double_array = LIB.get_long_rowmajor_array_as_double_array
get_long_rowmajor_array_as_double_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_int_rowmajor_array_as_int_array = LIB.get_int_rowmajor_array_as_int_array
get_int_rowmajor_array_as_int_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_int_rowmajor_array_as_long_array = LIB.get_int_rowmajor_array_as_long_array
get_int_rowmajor_array_as_long_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_int_rowmajor_array_as_float_array = LIB.get_int_rowmajor_array_as_float_array
get_int_rowmajor_array_as_float_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

get_int_rowmajor_array_as_double_array = LIB.get_int_rowmajor_array_as_double_array
get_int_rowmajor_array_as_double_array.argtypes = [\
                             c_char_p, c_int, c_long, c_char,\
                             ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                             c_ulong]

# INT to OTHERS
I2I_cast_and_copy_array = LIB.I2I_cast_and_copy_array
I2I_cast_and_copy_array.argtypes = [\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

I2L_cast_and_copy_array = LIB.I2L_cast_and_copy_array
I2L_cast_and_copy_array.argtypes = [\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

I2F_cast_and_copy_array = LIB.I2F_cast_and_copy_array
I2F_cast_and_copy_array.argtypes = [\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

I2D_cast_and_copy_array = LIB.I2D_cast_and_copy_array
I2D_cast_and_copy_array.argtypes = [\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

# LONG to OTHERS
L2I_cast_and_copy_array = LIB.L2I_cast_and_copy_array
L2I_cast_and_copy_array.argtypes = [\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

L2L_cast_and_copy_array = LIB.L2L_cast_and_copy_array
L2L_cast_and_copy_array.argtypes = [\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

L2F_cast_and_copy_array = LIB.L2F_cast_and_copy_array
L2F_cast_and_copy_array.argtypes = [\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

L2D_cast_and_copy_array = LIB.L2D_cast_and_copy_array
L2D_cast_and_copy_array.argtypes = [\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

# FLOAT to OTHERS
F2I_cast_and_copy_array = LIB.F2I_cast_and_copy_array
F2I_cast_and_copy_array.argtypes = [\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

F2L_cast_and_copy_array = LIB.F2L_cast_and_copy_array
F2L_cast_and_copy_array.argtypes = [\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

F2F_cast_and_copy_array = LIB.F2F_cast_and_copy_array
F2F_cast_and_copy_array.argtypes = [\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

F2D_cast_and_copy_array = LIB.F2D_cast_and_copy_array
F2D_cast_and_copy_array.argtypes = [\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

# DOUBLE to OTHERS
D2I_cast_and_copy_array = LIB.D2I_cast_and_copy_array
D2I_cast_and_copy_array.argtypes = [\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

D2L_cast_and_copy_array = LIB.D2L_cast_and_copy_array
D2L_cast_and_copy_array.argtypes = [\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_long, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

D2F_cast_and_copy_array = LIB.D2F_cast_and_copy_array
D2F_cast_and_copy_array.argtypes = [\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

D2D_cast_and_copy_array = LIB.D2D_cast_and_copy_array
D2D_cast_and_copy_array.argtypes = [\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                                c_ulong]

# --- Frovedis ML Models ---

parallel_float_glm_predict = LIB.parallel_float_glm_predict
parallel_float_glm_predict.argtypes = [c_char_p, c_int, c_int, c_short, c_long,\
                                 ndpointer(c_float, ndim=1,\
                                 flags="C_CONTIGUOUS"),\
                                 c_ulong, c_bool, c_short, c_bool]

parallel_double_glm_predict = LIB.parallel_double_glm_predict
parallel_double_glm_predict.argtypes = [c_char_p, c_int, c_int, c_short, c_long,\
                                 ndpointer(c_double, ndim=1,\
                                 flags="C_CONTIGUOUS"),\
                                 c_ulong, c_bool, c_short, c_bool]

# agglomerative
aca_train = LIB.aca_train
aca_train.argtypes = [c_char_p, c_int, c_long, # host, port, data_proxy
                      c_int, c_char_p, # n_cluster, linkage
                      ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"), c_long,\
                      #ret, ret_size
                      c_int, c_int, # verbose, mid
                      c_short, c_short, c_bool] #dtype, itype, dense

acm_predict = LIB.acm_predict
acm_predict.argtypes = [c_char_p, c_int, #host, port
                        c_int, c_short,  #mid, mtype
                        c_int,           #ncluster
                        ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"), c_long]\
                        #ret, ret_size

# spectral embedding
sea_train = LIB.sea_train
sea_train.argtypes = [c_char_p, #host
                      c_int,  #port
                      c_long,#data
                      c_int, #n_components
                      c_double,#gamma
                      c_bool, #precomputed
                      c_bool, #norm_laplacian
                      c_int, #mode
                      c_bool, #drop_first
                      c_int, #verbose
                      c_int, #mid
                      c_short, #dtype
                      c_short,#itype
                      c_bool #dense
                     ]

get_sem_affinity_matrix = LIB.get_sem_aff_matrix
get_sem_affinity_matrix.argtypes = [c_char_p, c_int, c_int, c_short]
get_sem_affinity_matrix.restype = py_object

get_sem_embedding_matrix = LIB.get_sem_embed_matrix
get_sem_embedding_matrix.argtypes = [c_char_p, c_int, c_int, c_short]
get_sem_embedding_matrix.restype = py_object


# spectral clustering

sca_train = LIB.sca_train
sca_train.argtypes = [c_char_p, #host
                      c_int, #port
                      c_long,#data
                      c_int, #n_clusters
                      c_int, #n_comp
                      c_int, #n_iter
                      c_double, #eps
                      c_double,#gamma
                      c_bool, #precomputed
                      c_bool, #norm_laplacian
                      c_int, #mode
                      c_bool, #drop_first
                      ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),#labels
                      c_long, #labels array length
                      c_int, #verbose
                      c_int, #mid
                      c_short, #dtype
                      c_short, #itype
                      c_bool #dense
                     ]

get_scm_affinity_matrix = LIB.get_scm_aff_matrix
get_scm_affinity_matrix.argtypes = [c_char_p, c_int, c_int, c_short]
get_scm_affinity_matrix.restype = py_object

#DBSCAN 
dbscan_train = LIB.dbscan_train
dbscan_train.argtypes = [c_char_p, #host
                         c_int, #port
                         c_long,#data
                         c_double, #eps
                         c_int, #min_pts
                         ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),#labels
                         c_long, #labels array length
                         c_int, #verbose
                         c_int, #mid
                         c_short, #dtype
                         c_short, #itype
                         c_bool #dense
                        ]

# kmeans predict returns int always:
parallel_kmeans_predict = LIB.parallel_kmeans_predict
parallel_kmeans_predict.argtypes = [c_char_p, c_int, c_int,
                                    c_short, c_long,
                                    ndpointer(c_int, ndim=1,\
                                    flags="C_CONTIGUOUS"),\
                                    c_ulong, c_short, c_bool]

als_float_predict = LIB.als_float_predict
als_float_predict.argtypes = [c_char_p, c_int, c_int,\
                        ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                        ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),\
                        c_ulong]

als_double_predict = LIB.als_double_predict
als_double_predict.argtypes = [c_char_p, c_int, c_int,\
                        ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                        ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),\
                        c_ulong]

als_float_rec_users = LIB.als_float_rec_users
als_float_rec_users.argtypes = [c_char_p, c_int, c_int, c_int, c_int,\
                          ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                          ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS")]

als_double_rec_users = LIB.als_double_rec_users
als_double_rec_users.argtypes = [c_char_p, c_int, c_int, c_int, c_int,\
                          ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                          ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS")]

als_float_rec_prods = LIB.als_float_rec_prods
als_float_rec_prods.argtypes = [c_char_p, c_int, c_int, c_int, c_int,\
                          ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                          ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS")]

als_double_rec_prods = LIB.als_double_rec_prods
als_double_rec_prods.argtypes = [c_char_p, c_int, c_int, c_int, c_int,\
                          ndpointer(c_int, ndim=1, flags="C_CONTIGUOUS"),\
                          ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS")]

release_frovedis_model = LIB.release_frovedis_model
release_frovedis_model.argtypes = [c_char_p, c_int, c_int, c_short, c_short]

show_frovedis_model = LIB.show_frovedis_model
show_frovedis_model.argtypes = [c_char_p, c_int, c_int, c_short, c_short]

load_frovedis_model = LIB.load_frovedis_model
load_frovedis_model.argtypes = [c_char_p, c_int, c_int, c_short, c_short,\
                                c_char_p]

load_frovedis_nbm = LIB.load_frovedis_nbm
load_frovedis_nbm.argtypes = [c_char_p, c_int, c_int, c_short, c_char_p]
load_frovedis_nbm.restype = py_object

# load scm
load_frovedis_scm = LIB.load_frovedis_scm
load_frovedis_scm.argtypes = [c_char_p, c_int, c_int, c_short, c_char_p]
load_frovedis_scm.restype = py_object

#load acm
load_frovedis_acm = LIB.load_frovedis_acm
load_frovedis_acm.argtypes = [c_char_p, c_int, c_int, c_short, c_char_p]
load_frovedis_acm.restype = c_int

load_frovedis_mfm = LIB.load_frovedis_mfm
load_frovedis_mfm.argtypes = [c_char_p, c_int, c_int, c_short, c_char_p]
load_frovedis_mfm.restype = py_object

save_frovedis_model = LIB.save_frovedis_model
save_frovedis_model.argtypes = [c_char_p, c_int, c_int, c_short, c_short,\
                                c_char_p]

get_weight_vector = LIB.get_frovedis_weight_vector
get_weight_vector.argtypes = [c_char_p, c_int, c_int, c_short, c_short]
get_weight_vector.restype = py_object

get_intercept_vector = LIB.get_frovedis_intercept_vector
get_intercept_vector.argtypes = [c_char_p, c_int, c_int, c_short, c_short]
get_intercept_vector.restype = py_object

get_pi_vector = LIB.get_frovedis_pi_vector
get_pi_vector.argtypes = [c_char_p, c_int, c_int, c_short, c_short]
get_pi_vector.restype = py_object

get_theta_vector = LIB.get_frovedis_theta_vector
get_theta_vector.argtypes = [c_char_p, c_int, c_int, c_short, c_short]
get_theta_vector.restype = py_object

get_cls_counts_vector = LIB.get_frovedis_cls_counts_vector
get_cls_counts_vector.argtypes = [c_char_p, c_int, c_int, c_short, c_short]
get_cls_counts_vector.restype = py_object

# --- Frovedis ML Trainers ---
distinct_count = LIB.get_distinct_count
distinct_count.argtypes = [c_char_p, c_int, c_long, c_short] #host, port,\
                           #proxy, dtype
distinct_count.restype = c_int

get_distinct_elements = LIB.distinct_elements
get_distinct_elements.argtypes = [c_char_p, c_int, c_long, c_short] #host, port,\
                                  #proxy, dtype
get_distinct_elements.restype = py_object

encode_frovedis_dvector_zero_based = LIB.encode_frovedis_dvector_zero_based
encode_frovedis_dvector_zero_based.argtypes = [c_char_p, c_int, #host, port
                                               c_long, c_short] #proxy, dtype
encode_frovedis_dvector_zero_based.restype = c_long # out proxy

encode_frovedis_int_dvector = LIB.encode_frovedis_int_dvector
encode_frovedis_int_dvector.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                                        POINTER(c_int), POINTER(c_int), #src, target 
                                        c_long] # size
encode_frovedis_int_dvector.restype = c_long # out proxy

encode_frovedis_long_dvector = LIB.encode_frovedis_long_dvector
encode_frovedis_long_dvector.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                                         POINTER(c_long), POINTER(c_long), #src, target 
                                         c_long] # size
encode_frovedis_long_dvector.restype = c_long # out proxy

encode_frovedis_float_dvector = LIB.encode_frovedis_float_dvector
encode_frovedis_float_dvector.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                                          POINTER(c_float), POINTER(c_float), #src, target 
                                          c_long] # size
encode_frovedis_float_dvector.restype = c_long # out proxy

encode_frovedis_double_dvector = LIB.encode_frovedis_double_dvector
encode_frovedis_double_dvector.argtypes = [c_char_p, c_int, c_long,  #host, port, proxy
                                           POINTER(c_double), POINTER(c_double), #src, target 
                                           c_long] # size
encode_frovedis_double_dvector.restype = c_long # out proxy

lr_sgd = LIB.lr_sgd
lr_sgd.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                   c_int, c_double,                 #iter, lr_rate
                   c_int, c_double, c_bool,         #rtype, rparam, is_mult
                   c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                   c_short, c_short, c_bool]        #dtype, itype, dense

lr_lbfgs = LIB.lr_lbfgs
lr_lbfgs.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                     c_int, c_double,                 #iter, lr_rate
                     c_int, c_double, c_bool,         #rtype, rparam, is_mult
                     c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                     c_short, c_short, c_bool]        #dtype, itype, dense

svm_sgd = LIB.svm_sgd
svm_sgd.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                    c_int, c_double,                 #iter, lr_rate
                    c_int, c_double,                 #rtype, rparam
                    c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                    c_short, c_short, c_bool]        #dtype, itype, dense

svm_lbfgs = LIB.svm_lbfgs
svm_lbfgs.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                      c_int, c_double,                 #iter, lr_rate
                      c_int, c_double,                 #rtype, rparam
                      c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                      c_short, c_short, c_bool]        #dtype, itype, dense

dt_train = LIB.dt_trainer
dt_train.argtypes = [c_char_p, c_int, c_long,
                     c_long, c_char_p, c_char_p,
                     c_int, c_int, c_int, c_int,
                     c_float, c_int, c_int,
                     c_short, c_short, c_bool]

nb_train = LIB.nb_trainer
nb_train.argtypes = [c_char_p, c_int, c_long,
                     c_long, c_double, c_int, c_char_p, c_int,
                     c_short, c_short, c_bool]

lnr_sgd = LIB.lnr_sgd
lnr_sgd.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                    c_int, c_double,                 #iter, lr_rate
                    c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                    c_short, c_short, c_bool]        #dtype, itype, dense

lnr_lbfgs = LIB.lnr_lbfgs
lnr_lbfgs.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                      c_int, c_double,                 #iter, lr_rate
                      c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                      c_short, c_short, c_bool]        #dtype, itype, dense

lasso_sgd = LIB.lasso_sgd
lasso_sgd.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                      c_int, c_double,                 #iter, lr_rate
                      c_double,                        #regparam
                      c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                      c_short, c_short, c_bool]        #dtype, itype, dense

lasso_lbfgs = LIB.lasso_lbfgs
lasso_lbfgs.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                        c_int, c_double,                 #iter, lr_rate
                        c_double,                        #regparam
                        c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                        c_short, c_short, c_bool]        #dtype, itype, dense

ridge_sgd = LIB.ridge_sgd
ridge_sgd.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                      c_int, c_double,                 #iter, lr_rate
                      c_double,                        #regparam
                      c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                      c_short, c_short, c_bool]        #dtype, itype, dense

ridge_lbfgs = LIB.ridge_lbfgs
ridge_lbfgs.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                        c_int, c_double,                 #iter, lr_rate
                        c_double,                        #regparam
                        c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                        c_short, c_short, c_bool]        #dtype, itype, dense

# For SGDClassifier with "squared_loss"
lnr2_sgd = LIB.lnr2_sgd
lnr2_sgd.argtypes = [c_char_p, c_int, c_long, c_long, #host,port,X,y
                     c_int, c_double,                 #iter, lr_rate
                     c_int, c_double,                 #rtype, rparam
                     c_bool, c_double, c_int, c_int,  #fit_icpt, tol, vb, mid
                     c_short, c_short, c_bool]        #dtype, itype, dense

kmeans_train = LIB.kmeans_train
kmeans_train.argtypes = [c_char_p, c_int, c_long, c_int,
                         c_int, c_long, c_double, c_int, c_int,
                         c_short, c_short, c_bool]

# als will always be trained with sparse data
als_train = LIB.als_train
als_train.argtypes = [c_char_p, c_int, c_long, c_int, c_int,
                      c_double, c_double, c_long, c_int, c_int,
                      c_short, c_short]
#fp growth functions
fpgrowth_trainer = LIB.fpgrowth_trainer
fpgrowth_trainer.argtypes = [c_char_p, c_int, c_long, c_int, c_double, c_int]


#fpgrowth_freq_items = lib.fpgrowth_freq_items
#fpgrowth_freq_items.argtypes = [c_char_p, c_int, c_int]

#fpgrowth_rules = lib.fpgrowth_rules
#fpgrowth_rules.argtypes = [c_char_p, c_int, c_int, c_double]

fpgrowth_fpr = LIB.fpgrowth_fpr
fpgrowth_fpr.argtypes = [c_char_p, c_int, c_int, c_int, c_double]

fm_train = LIB.fm_trainer
fm_train.argtypes = [c_char_p, c_int,
                     c_long, c_long,
                     c_double, c_int,
                     c_double, c_char_p,
                     c_bool, c_bool, c_int,
                     c_double, c_double, c_double,
                     c_int, c_int, c_bool, c_int,
                     c_short, c_short]

w2v_build_vocab_and_dump = LIB.w2v_build_vocab_and_dump
w2v_build_vocab_and_dump.argtypes = [c_char_p, c_char_p, #text, encode
                                     c_char_p, c_char_p, #vocab, count
                                     c_int]              #minCount

w2v_train = LIB.w2v_train
w2v_train.argtypes = [c_char_p, c_int,              #host, port
                      c_char_p, c_char_p, c_char_p, #encode, weight, count
                      c_int, c_int, c_float, c_int, #hidden, window, thr, neg
                      c_int, c_float, c_float,      #iter, lr, syncperiod
                      c_int, c_int,                 #syncWords, syncTimes,
                      c_int, c_int]                 #msgSize, nthreads

w2v_save_model = LIB.w2v_save_model
w2v_save_model.argtypes = [c_char_p, c_char_p,  #weight, vocab
                           c_char_p, c_int,     #out, minCount
                           c_bool]              #binary

# --- Frovedis PBLAS Wrappers ---
pswap = LIB.pswap
pswap.argtypes = [c_char_p, c_int, c_long, c_long, c_short]

pcopy = LIB.pcopy
pcopy.argtypes = [c_char_p, c_int, c_long, c_long, c_short]

pscal = LIB.pscal
pscal.argtypes = [c_char_p, c_int, c_long, c_double, c_short]

paxpy = LIB.paxpy
paxpy.argtypes = [c_char_p, c_int, c_long, c_long, c_double, c_short]

pdot = LIB.pdot
pdot.argtypes = [c_char_p, c_int, c_long, c_long, c_short]
pdot.restype = c_double

pnrm2 = LIB.pnrm2
pnrm2.argtypes = [c_char_p, c_int, c_long, c_short]
pnrm2.restype = c_double

pgemv = LIB.pgemv
pgemv.argtypes = [c_char_p, c_int, c_long, c_long,
                  c_bool, c_double, c_double, c_short]
pgemv.restype = py_object

pger = LIB.pger
pger.argtypes = [c_char_p, c_int, c_long, c_long, c_double, c_short]
pger.restype = py_object

pgemm = LIB.pgemm
pgemm.argtypes = [c_char_p, c_int, c_long, c_long,
                  c_bool, c_bool, c_double, c_double, c_short]
pgemm.restype = py_object

pgeadd = LIB.pgeadd
pgeadd.argtypes = [c_char_p, c_int, c_long, c_long,
                   c_bool, c_double, c_double, c_short]

# --- Frovedis SCALAPACK Wrappers ---
pgetrf = LIB.pgetrf
pgetrf.argtypes = [c_char_p, c_int, c_long, c_short]
pgetrf.restype = py_object

pgetri = LIB.pgetri
pgetri.argtypes = [c_char_p, c_int, c_long, c_long, c_short]
pgetri.restype = c_int

pgetrs = LIB.pgetrs
pgetrs.argtypes = [c_char_p, c_int, c_long, c_long, c_long, c_bool, c_short]
pgetrs.restype = c_int

pgesv = LIB.pgesv
pgesv.argtypes = [c_char_p, c_int, c_long, c_long, c_short]
pgesv.restype = c_int

pgels = LIB.pgels
pgels.argtypes = [c_char_p, c_int, c_long, c_long, c_bool, c_short]
pgels.restype = c_int

pgesvd = LIB.pgesvd
pgesvd.argtypes = [c_char_p, c_int, c_long, c_bool, c_bool, c_short]
pgesvd.restype = py_object

# --- Frovedis ARPACK Wrappers ---

compute_var_sum = LIB.sum_of_variance
compute_var_sum.argtypes = [c_char_p, c_int, # host, port
                            c_long, c_bool,  # mptr, sample_variance
                            c_bool, c_short] # isdense, dtype
compute_var_sum.restype = c_double

compute_truncated_svd = LIB.compute_truncated_svd
compute_truncated_svd.argtypes = [c_char_p, c_int, c_long, c_int,
                                  c_short, c_short, c_bool]
compute_truncated_svd.restype = py_object

# --- Scalapack Results ---
release_ipiv = LIB.release_ipiv
release_ipiv.argtypes = [c_char_p, c_int, c_char, c_long]

save_as_diag_matrix = LIB.save_as_diag_matrix
save_as_diag_matrix.argtypes = [c_char_p, c_int, c_long, c_char_p, c_bool,\
                                c_char]

get_svd_results_from_file = LIB.get_svd_results_from_file
get_svd_results_from_file.argtypes = [c_char_p, c_int,
                                      c_char_p, c_char_p, c_char_p,
                                      c_bool, c_bool, c_bool, c_char, c_char]
get_svd_results_from_file.restype = py_object


compute_pca = LIB.compute_pca
compute_pca.argtypes = [ c_char_p, #host
                         c_int,  #port
                         c_long, #data
                         c_int,  #k
                         c_bool, #whiten
                         c_short, #dtype
                         c_bool, #to_copy 
                         c_bool #movable 
                       ]
compute_pca.restype = py_object

# knn - Nearest Neighbors
knn_fit = LIB.knn_fit
knn_fit.argtypes = [  c_char_p, #host
                      c_int,  #port
                      c_long, #data
                      c_int, #k
                      c_float, #radius
                      c_char_p, #algorithm
                      c_char_p, # metric
                      c_float, #chunk_size
                      c_int, #vb
                      c_int, #mid
                      c_short, #dtype
                      c_short, #itype
                      c_bool #dense
                   ]

knc_fit = LIB.knc_fit
knc_fit.argtypes = [  c_char_p, #host
                      c_int,  #port
                      c_long, #data - mat
                      c_long, #data - labels
                      c_int, #k
                      c_char_p, #algorithm
                      c_char_p, # metric
                      c_float, #chunk_size
                      c_int, #vb
                      c_int, #mid
                      c_short, #dtype
                      c_short, #itype
                      c_bool #dense
                   ]

knr_fit = LIB.knr_fit
knr_fit.argtypes = [  c_char_p, #host
                      c_int,  #port
                      c_long, #data - mat
                      c_long, #data - labels
                      c_int, #k
                      c_char_p, #algorithm
                      c_char_p, # metric
                      c_float, #chunk_size
                      c_int, #vb
                      c_int, #mid
                      c_short, #dtype
                      c_short, #itype
                      c_bool #dense
                   ]

knc_kneighbors = LIB.knc_kneighbors
knc_kneighbors.argtypes = [c_char_p, #host
                           c_int,  #port
                           c_long, #data
                           c_int, #k
                           c_int, #mid
                           c_bool, #need distance,
                           c_short #dtype
                          ]
knc_kneighbors.restype = py_object

knr_kneighbors = LIB.knr_kneighbors
knr_kneighbors.argtypes = [ c_char_p, #host
                            c_int,  #port
                            c_long, #data
                            c_int, #k
                            c_int, #mid
                            c_bool, #need distance,
                            c_short #dtype
                          ]
knr_kneighbors.restype = py_object

knc_kneighbors_graph = LIB.knc_kneighbors_graph
knc_kneighbors_graph.argtypes = [ c_char_p, #host
                                  c_int,  #port
                                  c_long, #data
                                  c_int, #k
                                  c_int, #mid
                                  c_char_p, #mode,
                                  c_short #dtype
                                ]
knc_kneighbors_graph.restype = py_object

knr_kneighbors_graph = LIB.knr_kneighbors_graph
knr_kneighbors_graph.argtypes = [ c_char_p, #host
                                  c_int,  #port
                                  c_long, #data
                                  c_int, #k
                                  c_int, #mid
                                  c_char_p, #mode,
                                  c_short #dtype
                                ]
knr_kneighbors_graph.restype = py_object

knc_float_predict = LIB.knc_float_predict
knc_float_predict.argtypes = [ c_char_p, #host
                               c_int,  #port
                               c_long, #data
                               c_int, #mid
                               c_bool, # save_proba
                               ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),
                               c_long # ret length
                             ]

knc_double_predict = LIB.knc_double_predict
knc_double_predict.argtypes = [ c_char_p, #host
                                c_int,  #port
                                c_long, #data
                                c_int, #mid
                                c_bool, # save_proba
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),
                                c_long # ret length
                              ]

knr_float_predict = LIB.knr_float_predict
knr_float_predict.argtypes = [ c_char_p, #host
                               c_int,  #port
                               c_long, #data
                               c_int, #mid
                               ndpointer(c_float, ndim=1, flags="C_CONTIGUOUS"),
                               c_long # ret length
                             ]

knr_double_predict = LIB.knr_double_predict
knr_double_predict.argtypes = [ c_char_p, #host
                                c_int,  #port
                                c_long, #data
                                c_int, #mid
                                ndpointer(c_double, ndim=1, flags="C_CONTIGUOUS"),
                                c_long # ret length
                              ]

knc_predict_proba = LIB.knc_predict_proba
knc_predict_proba.argtypes = [ c_char_p, #host
                               c_int,  #port
                               c_long, #data
                               c_int, #mid
                               c_short #dtype
                             ]
knc_predict_proba.restype = py_object

knr_model_score = LIB.knr_model_score
knr_model_score.argtypes = [ c_char_p, #host
                             c_int,  #port
                             c_long, #data - mat
                             c_long, #data - labels
                             c_int, #mid
                             c_short #dtype
                           ]
knr_model_score.restype = c_float

knc_model_score = LIB.knc_model_score
knc_model_score.argtypes = [ c_char_p, #host
                             c_int,  #port
                             c_long, #data - mat
                             c_long, #data - labels
                             c_int, #mid
                             c_short #dtype
                           ]
knc_model_score.restype = c_float

knn_kneighbors = LIB.knn_kneighbors
knn_kneighbors.argtypes = [c_char_p, #host
                           c_int,  #port
                           c_long, #data
                           c_int, #k
                           c_int, #mid
                           c_bool, #need distance,
                           c_short #dtype
                          ]
knn_kneighbors.restype = py_object

knn_kneighbors_graph = LIB.knn_kneighbors_graph
knn_kneighbors_graph.argtypes = [ c_char_p, #host
                                  c_int,  #port
                                  c_long, #data
                                  c_int, #k
                                  c_int, #mid
                                  c_char_p, #mode,
                                  c_short #dtype
                                ]
knn_kneighbors_graph.restype = py_object

knn_radius_neighbors = LIB.knn_radius_neighbors
knn_radius_neighbors.argtypes = [ c_char_p, c_int, #host ,port
                                  c_long, c_float, c_int, #data, radius, mid
                                  c_bool, c_short  #need_distance, dtype
                                ]
knn_radius_neighbors.restype = py_object

knn_radius_neighbors_graph = LIB.knn_radius_neighbors_graph
knn_radius_neighbors_graph.argtypes = [ c_char_p, c_int, #host ,port
                                        c_long, c_float, c_int, #data, radius, mid
                                        c_char_p, c_short  #mode, dtype
                                      ]
knn_radius_neighbors_graph.restype = py_object

pca_transform = LIB.pca_transform
pca_transform.argtypes = [ c_char_p, #host
                           c_int,  #port
                           c_long, #data
                           c_long, #pca _directions
                           c_long, #explained variance
                           c_long, #mean
                           c_short, #dtype
                           c_bool #whiten
                         ]
pca_transform.restype = py_object

pca_inverse_transform = LIB.pca_inverse_transform
pca_inverse_transform.argtypes = [ c_char_p, #host
                                   c_int,  #port
                                   c_long, #data
                                   c_long, #pca _directions
                                   c_long, #explained variance
                                   c_long, #mean
                                   c_short, #dtype
                                   c_bool #whiten
                                 ]
pca_inverse_transform.restype = py_object

compute_lda_component = LIB.compute_lda_component
compute_lda_component.argtypes = [c_char_p, c_int,\
                                  c_int, c_short]
compute_lda_component.restype = py_object


compute_lda_train = LIB.compute_lda_train
compute_lda_train.argtypes = [c_char_p, c_int,\
                              c_ulong, c_double,\
                              c_double, c_int,\
                              c_int, c_char_p,\
                              c_int, c_int,\
                              c_short, c_short,\
                              c_int, c_int]

compute_lda_transform = LIB.compute_lda_transform
compute_lda_transform.argtypes = [c_char_p, c_int,\
                                  c_ulong, c_double,\
                                  c_double, c_int,\
                                  c_char_p,\
                                  c_int, c_int,\
                                  c_short, c_short]
compute_lda_transform.restype = py_object

call_frovedis_pagerank = LIB.call_frovedis_pagerank
call_frovedis_pagerank.argtypes = [c_char_p, c_int,\
                                  c_long, c_double,\
                                  c_double, c_int]
call_frovedis_pagerank.restype = py_object

set_graph_data= LIB.set_graph_data
set_graph_data.argtypes = [c_char_p, c_int,\
                           c_ulong]
set_graph_data.restype = c_ulong

get_graph_data= LIB.get_graph_data
get_graph_data.argtypes = [c_char_p, c_int,\
                           c_ulong]
get_graph_data.restype = c_ulong

show_graph_py= LIB.show_graph_py
show_graph_py.argtypes = [c_char_p, c_int,\
                           c_ulong]

release_graph_py= LIB.release_graph_py
release_graph_py.argtypes = [c_char_p, c_int,\
                           c_ulong]

save_graph_py= LIB.save_graph_py
save_graph_py.argtypes = [c_char_p, c_int,\
                 c_ulong, c_char_p]

load_graph_from_text_file= LIB.load_graph_from_text_file
load_graph_from_text_file.argtypes = [c_char_p, c_int,\
                                      c_char_p]
load_graph_from_text_file.restype = py_object

copy_graph_py= LIB.copy_graph_py
copy_graph_py.argtypes = [c_char_p, c_int,\
                          c_ulong]
copy_graph_py.restype = c_ulong

call_frovedis_sssp= LIB.call_frovedis_sssp
call_frovedis_sssp.argtypes = [c_char_p, c_int,\
                          c_ulong, \
                          ndpointer(c_int, ndim=1,\
                              flags="C_CONTIGUOUS"), \
                          ndpointer(c_long, ndim=1,\
                              flags="C_CONTIGUOUS"), \
                          c_ulong, c_ulong]


#random forest trainer function
rf_train = LIB.rf_trainer
rf_train.argtypes = [c_char_p, c_int, c_long,         #host,port,X
                     c_long, c_char_p, c_char_p,      #y,algo,criterion
                     c_int, c_int, c_int, c_char_p,   #n_est,max_dep,n_cl,feature_subset_strat
                     c_double, #feature_subset_rate
                     c_int, c_int, c_double, c_long,   #mx_bin ,min_sample_leaf,min_impurity_decrease,seed
                     c_int, c_int, c_short, c_short,  #vb,mid,dtype,itype
                     c_bool]                          #dense


call_frovedis_bfs= LIB.call_frovedis_bfs
call_frovedis_bfs.argtypes = [c_char_p, c_int,\
                          c_ulong, \
                          ndpointer(c_long, ndim=1,\
                              flags="C_CONTIGUOUS"), \
                          ndpointer(c_int, ndim=1,\
                              flags="C_CONTIGUOUS"), \
                          c_ulong]
call_frovedis_bfs.restype = py_object

gbt_train = LIB.gbt_trainer
gbt_train.argtypes = [c_char_p, c_int, # host, port
                     c_long, c_long, # xptr, yptr
                     c_char_p, c_char_p, c_char_p, # algo, loss, impurity
                     c_double, c_int, c_double, # learning_rate, max_depth, min_impurity_decrease
                     c_int, c_double, c_int, # seed, tol, max_bins
                     c_double, c_char_p, c_double, #subsampling rate, strategy, subset_features
                     c_int, c_int, # n_estimators, nclasses 
                     c_int, c_int, # verbose, mid
                     c_short, c_short, c_bool # dtype, itype, dense
                     ]
