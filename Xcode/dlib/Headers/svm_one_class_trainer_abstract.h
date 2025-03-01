// Copyright (C) 2010  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_SVm_ONE_CLASS_TRAINER_ABSTRACT_
#ifdef DLIB_SVm_ONE_CLASS_TRAINER_ABSTRACT_

#include <cmath>
#include <limits>
#include <dlib/matrix_abstract.h>
#include <dlib/algs.h>
#include <dlib/function_abstract.h>
#include <dlib/kernel_abstract.h>
#include <dlib/optimization_solve_qp3_using_smo_abstract.h>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <
        typename K 
        >
    class svm_one_class_trainer
    {
        /*!
            REQUIREMENTS ON K 
                is a kernel function object as defined in dlib/svm/kernel_abstract.h 

            WHAT THIS OBJECT REPRESENTS
                This object implements a trainer for a support vector machine for 
                solving one-class classification problems.  It is implemented using the SMO
                algorithm.

                The implementation of the training algorithm used by this object is based
                on the following excellent paper:
                    - Chih-Chung Chang and Chih-Jen Lin, LIBSVM : a library for support vector 
                      machines, 2001. Software available at http://www.csie.ntu.edu.tw/~cjlin/libsvm

        !*/

    public:
        typedef K kernel_type;
        typedef typename kernel_type::scalar_type scalar_type;
        typedef typename kernel_type::sample_type sample_type;
        typedef typename kernel_type::mem_manager_type mem_manager_type;
        typedef decision_function<kernel_type> trained_function_type;

        svm_one_class_trainer (
        );
        /*!
            ensures
                - This object is properly initialized and ready to be used
                  to train a support vector machine.
                - #get_nu() == 0.1 
                - #get_cache_size() == 200
                - #get_epsilon() == 0.001
        !*/

        svm_one_class_trainer (
            const kernel_type& kernel, 
            const scalar_type& nu
        );
        /*!
            requires
                - 0 < nu <= 1
            ensures
                - This object is properly initialized and ready to be used
                  to train a support vector machine.
                - #get_kernel() == kernel
                - #get_nu() == nu
                - #get_cache_size() == 200
                - #get_epsilon() == 0.001
        !*/

        void set_cache_size (
            long cache_size
        );
        /*!
            requires
                - cache_size > 0
            ensures
                - #get_cache_size() == cache_size 
        !*/

        const long get_cache_size (
        ) const;
        /*!
            ensures
                - returns the number of megabytes of cache this object will use
                  when it performs training via the this->train() function.
                  (bigger values of this may make training go faster but won't affect 
                  the result.  However, too big a value will cause you to run out of 
                  memory, obviously.)
        !*/

        void set_epsilon (
            scalar_type eps
        );
        /*!
            requires
                - eps > 0
            ensures
                - #get_epsilon() == eps 
        !*/

        const scalar_type get_epsilon (
        ) const;
        /*!
            ensures
                - returns the error epsilon that determines when training should stop.
                  Generally a good value for this is 0.001.  Smaller values may result
                  in a more accurate solution but take longer to execute.
        !*/

        void set_kernel (
            const kernel_type& k
        );
        /*!
            ensures
                - #get_kernel() == k 
        !*/

        const kernel_type& get_kernel (
        ) const;
        /*!
            ensures
                - returns a copy of the kernel function in use by this object
        !*/

        void set_nu (
            scalar_type nu
        );
        /*!
            requires
                - 0 < nu <= 1
            ensures
                - #get_nu() == nu
        !*/

        const scalar_type get_nu (
        ) const;
        /*!
            ensures
                - returns the nu svm parameter.  This is a value between 0 and
                  1.  It is the parameter that determines the trade off between
                  trying to fit the training data exactly or allowing more errors 
                  but hopefully improving the generalization ability of the 
                  resulting classifier.  Smaller values encourage exact fitting 
                  while larger values of nu may encourage better generalization. 
                  For more information you should consult the papers referenced 
                  above.
        !*/

        template <
            typename in_sample_vector_type
            >
        const decision_function<kernel_type> train (
            const in_sample_vector_type& x
        ) const;
        /*!
            requires
                - x.size() > 0
                - is_col_vector(x) == true
                - x == a matrix or something convertible to a matrix via mat().
                  Also, x should contain sample_type objects.
            ensures
                - trains a one-class support vector classifier given the training samples in x.
                  Training is done when the error is less than get_epsilon().
                - returns a decision function F with the following properties:
                    - if (new_x is a sample predicted to arise from the distribution
                      which generated the training samples) then
                        - F(new_x) >= 0
                    - else
                        - F(new_x) < 0
        !*/

        void swap (
            svm_one_class_trainer& item
        );
        /*!
            ensures
                - swaps *this and item
        !*/
    }; 

    template <typename K>
    void swap (
        svm_one_class_trainer<K>& a,
        svm_one_class_trainer<K>& b
    ) { a.swap(b); }
    /*!
        provides a global swap
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_SVm_ONE_CLASS_TRAINER_ABSTRACT_



