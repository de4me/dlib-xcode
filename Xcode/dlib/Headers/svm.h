// Copyright (C) 2007  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.

#ifdef DLIB_ALL_SOURCE_END
#include "dlib_basic_cpp_build_tutorial.txt"
#endif

#ifndef DLIB_SVm_HEADER
#define DLIB_SVm_HEADER

#include <dlib/svm_rank_trainer.h>
#include <dlib/svm_svm.h>
#include <dlib/krls.h>
#include <dlib/rls.h>
#include <dlib/kcentroid.h>
#include <dlib/kcentroid_overloads.h>
#include <dlib/kkmeans.h>
#include <dlib/feature_ranking.h>
#include <dlib/rbf_network.h>
#include <dlib/linearly_independent_subset_finder.h>
#include <dlib/reduced.h>
#include <dlib/rvm.h>
#include <dlib/pegasos.h>
#include <dlib/sparse_kernel.h>
#include <dlib/null_trainer.h>
#include <dlib/roc_trainer.h>
#include <dlib/kernel_matrix.h>
#include <dlib/empirical_kernel_map.h>
#include <dlib/svm_c_linear_trainer.h>
#include <dlib/svm_c_linear_dcd_trainer.h>
#include <dlib/svm_c_ekm_trainer.h>
#include <dlib/simplify_linear_decision_function.h>
#include <dlib/krr_trainer.h>
#include <dlib/sort_basis_vectors.h>
#include <dlib/svm_c_trainer.h>
#include <dlib/svm_one_class_trainer.h>
#include <dlib/svr_trainer.h>

#include <dlib/one_vs_one_decision_function.h>
#include <dlib/multiclass_tools.h>
#include <dlib/cross_validate_multiclass_trainer.h>
#include <dlib/cross_validate_regression_trainer.h>
#include <dlib/cross_validate_object_detection_trainer.h>
#include <dlib/cross_validate_sequence_labeler.h>
#include <dlib/cross_validate_sequence_segmenter.h>
#include <dlib/cross_validate_assignment_trainer.h>

#include <dlib/one_vs_all_decision_function.h>

#include <dlib/structural_svm_problem.h>
#include <dlib/sequence_labeler.h>
#include <dlib/assignment_function.h>
#include <dlib/track_association_function.h>
#include <dlib/active_learning.h>
#include <dlib/svr_linear_trainer.h>
#include <dlib/sequence_segmenter.h>
#include <dlib/auto.h>

#endif // DLIB_SVm_HEADER


