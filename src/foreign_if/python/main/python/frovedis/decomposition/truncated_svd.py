"""truncated_svd.py"""

#!/usr/bin/env python

from ..exrpc.server import FrovedisServer
from ..exrpc.rpclib import compute_truncated_svd, compute_var_sum
from ..exrpc.rpclib import check_server_exception
from ..matrix.ml_data import FrovedisFeatureData
from ..matrix.crs import FrovedisCRSMatrix
from ..matrix.dense import FrovedisRowmajorMatrix, FrovedisColmajorMatrix
from ..matrix.results import svdResult
from ..matrix.dtype import TypeUtil
import numpy as np
from scipy.sparse import issparse

class TruncatedSVD(object):
    """Dimensionality reduction using truncated SVD (aka LSA)."""

    def __init__(self, n_components=2, algorithm="arpack", n_iter=5,
                 random_state=None, tol=0.):
        self.algorithm = algorithm
        self.n_components = n_components
        self.n_iter = n_iter
        self.random_state = random_state
        self.tol = tol
        self.svd_res_ = None
        self.var_sum = None
        self._components = None
        self._explained_variance = None
        self._explained_variance_ratio = None
        self._singular_values = None

    def fit(self, X, y=None):
        """Fit LSA model on training data X."""
        (host, port) = FrovedisServer.getServerInstance()
        if self.algorithm != "arpack":
            raise ValueError("algorithm: currently Frovedis supports only \
                              arpack!")
        if isinstance(X, FrovedisCRSMatrix):
            self.var_sum = None
        elif isinstance(X, FrovedisRowmajorMatrix):
            to_sample = False # ddof = 0 in np.var(...)
            isdense = True
            self.var_sum = compute_var_sum(host, port, X.get(),
                                           to_sample, isdense, X.get_dtype())
            excpt = check_server_exception()
            if excpt["status"]:
                raise RuntimeError(excpt["info"])
        elif issparse(X):
            try:
                from sklearn.utils.sparsefuncs import mean_variance_axis
                _, full_var = mean_variance_axis(X, axis=0)
                self.var_sum = full_var.sum()
            except: #for system without sklearn
                self.var_sum = None
        else:
            self.var_sum = np.var(X, axis=0).sum()
        # if X is not a sparse data, it would be converted as rowmajor matrix
        inp_data = FrovedisFeatureData(X, dense_kind='rowmajor')
        X = inp_data.get()
        x_dtype = inp_data.get_dtype()
        x_itype = inp_data.get_itype()
        dense = inp_data.is_dense()
        res = compute_truncated_svd(host, port, X.get(),
                                    self.n_components,
                                    x_dtype, x_itype, dense)
        excpt = check_server_exception()
        if excpt["status"]:
            raise RuntimeError(excpt["info"])
        self.svd_res_ = svdResult(res, TypeUtil.to_numpy_dtype(x_dtype))
        return self

    def __set_results(self):
        """ it should be called after fit().
        it sets the ouput properties """
        if self.svd_res_ is None:
            raise ValueError("__set_results is called before fit!")
        (U, s, VT) = self.svd_res_.to_numpy_results()
        self._components = VT
        self._singular_values = s
        X_transformed = U * np.diag(s)
        exp_var = np.var(X_transformed, axis=0)
        self._explained_variance = np.asarray(exp_var)[0]
        if self.var_sum is not None:
            self._explained_variance_ratio = \
                    self._explained_variance / self.var_sum
        return self

    @property
    def components_(self):
        """ returns the output VT """
        if self._components is None:
            self.__set_results()
        return self._components

    @components_.setter
    def components_(self, val):
        """components_ setter"""
        raise AttributeError("attribute 'components_' of TruncatedSVD object \
                              is not writable")

    @property
    def singular_values_(self):
        """ retuns the output singular values """
        if self._singular_values is None:
            self.__set_results()
        return self._singular_values

    @singular_values_.setter
    def singular_values_(self, val):
        """singular_values_ setter"""
        raise AttributeError("attribute 'singular_values_' of TruncatedSVD \
                                object is not writable")

    @property
    def explained_variance_(self):
        """ retuns the output singular explained_variance """
        if self._explained_variance is None:
            self.__set_results()
        return self._explained_variance

    @explained_variance_.setter
    def explained_variance_(self, val):
        """explained_variance_ setter"""
        raise AttributeError("attribute 'explained_variance_' \
                                of TruncatedSVD object is not writable")

    @property
    def explained_variance_ratio_(self):
        """ retuns the output singular explained_variance_ratio in
        case input is python data or scipy sparse matrix"""
        if self.var_sum is None:
            raise ValueError("explained_variance_ratio_: can be \
              obtained only when input is numpy data or scipy sparse data or \
              FrovedisRowmajorMatrix")
        elif self._explained_variance_ratio is None:
            self.__set_results()
        return self._explained_variance_ratio

    @explained_variance_ratio_.setter
    def explained_variance_ratio_(self, val):
        """explained_variance_ratio_ setter"""
        raise AttributeError("attribute 'explained_variance_ratio_' of \
                              TruncatedSVD object is not writable")
