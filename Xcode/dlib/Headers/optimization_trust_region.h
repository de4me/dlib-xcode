// Copyright (C) 2010  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_OPTIMIZATION_TRUST_REGIoN_Hh_
#define DLIB_OPTIMIZATION_TRUST_REGIoN_Hh_

#include <dlib/matrix.h>
#include <dlib/optimization_trust_region_abstract.h>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <
        typename EXP1,
        typename EXP2,
        typename T, long NR, long NC, typename MM, typename L
        >
    unsigned long solve_trust_region_subproblem ( 
        const matrix_exp<EXP1>& B,
        const matrix_exp<EXP2>& g,
        const typename EXP1::type radius,
        matrix<T,NR,NC,MM,L>& p,
        double eps,
        unsigned long max_iter
    )
    {
        /*
            This is an implementation of algorithm 4.3(Trust Region Subproblem)
            from the book Numerical Optimization by Nocedal and Wright.  Some of
            the details are also from Practical Methods of Optimization by Fletcher.
        */

        // make sure requires clause is not broken
        DLIB_ASSERT(B.nr() == B.nc() && is_col_vector(g) && g.size() == B.nr(),
            "\t unsigned long solve_trust_region_subproblem()"
            << "\n\t invalid arguments were given to this function"
            << "\n\t B.nr():            " << B.nr()
            << "\n\t B.nc():            " << B.nc()
            << "\n\t is_col_vector(g):  " << is_col_vector(g) 
            << "\n\t g.size():          " << g.size() 
            );
        DLIB_ASSERT(radius > 0 && eps > 0 && max_iter > 0,
            "\t unsigned long solve_trust_region_subproblem()"
            << "\n\t invalid arguments were given to this function"
            << "\n\t radius:   " << radius
            << "\n\t eps:      " << eps 
            << "\n\t max_iter: " << max_iter 
            );


        const_temp_matrix<EXP1> BB(B);
        const_temp_matrix<EXP2> gg(g);

        p.set_size(g.nr(),g.nc());
        p = 0;


        const T numeric_eps = max(diag(abs(BB)))*std::numeric_limits<T>::epsilon();

        matrix<T,EXP1::NR,EXP2::NR,MM,L> R;

        T lambda = 0;

        // We need to put a bracket around lambda.  It can't go below 0.  We
        // can get an upper bound using Gershgorin disks.  
        // This number is a lower bound on the eigenvalues in BB
        const T BB_min_eigenvalue = min(diag(BB) - (sum_cols(abs(BB)) - abs(diag(BB))));

        const T g_norm = length(gg);

        T lambda_min = 0;
        T lambda_max = put_in_range(0, 
                                    std::numeric_limits<T>::max(), 
                                    g_norm/radius - BB_min_eigenvalue);


        // If we can tell that the minimum is at 0 then don't do anything.  Just return the answer. 
        if (g_norm < numeric_eps && BB_min_eigenvalue > numeric_eps)
        {
            return 0;
        }


        // how much lambda has changed recently
        T lambda_delta = 0;

        for (unsigned long i = 0; i < max_iter; ++i)
        {
            R = chol(BB + lambda*identity_matrix<T>(BB.nr()));

            // if the cholesky decomposition doesn't exist. 
            if (R(R.nr()-1, R.nc()-1) <= 0)
            {
                // If B is indefinite and g is equal to 0 then we should
                // quit this loop and go right to the eigenvalue decomposition method.
                if (g_norm <= numeric_eps)
                    break;

                // narrow the bracket on lambda.  Obviously the current lambda is
                // too small.
                lambda_min = lambda;

                // jump towards the max value.  Eventually there will
                // be a lambda that results in a cholesky decomposition.
                const T alpha = 0.10;
                lambda = (1-alpha)*lambda + alpha*lambda_max;
                continue;
            }

            using namespace blas_bindings;

            p = -gg;
            // Solve RR'*p = -g for p.
            // Solve R*q = -g for q where q = R'*p.
            if (R.nr() == 2)
            {
                p(0) = p(0)/R(0,0);
                p(1) = (p(1)-R(1,0)*p(0))/R(1,1);
            }
            else
            {
                triangular_solver(CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, R, p);
            }
            const T q_norm = length(p);

            // Solve R'*p = q for p.
            if (R.nr() == 2)
            {
                p(1) = p(1)/R(1,1);
                p(0) = (p(0)-R(1,0)*p(1))/R(0,0);
            }
            else
            {
                triangular_solver(CblasLeft, CblasLower, CblasTrans, CblasNonUnit, R, p);
            }
            const T p_norm = length(p);

            // check if we are done.  
            if (lambda == 0)
            {
                if (p_norm < radius)
                {
                    // i will always be 0 in this case.  So we return 1.
                    return i+1;
                }
            }
            else
            {
                // if we are close enough to the solution then terminate
                if (std::abs(p_norm - radius)/radius < eps)
                    return i+1;
            }

            // shrink our bracket on lambda
            if (p_norm < radius)
                lambda_max = lambda;
            else
                lambda_min = lambda;


            if (p_norm <= radius*std::numeric_limits<T>::epsilon())
            {
                const T alpha = 0.01;
                lambda = (1-alpha)*lambda_min + alpha*lambda_max;
                continue;
            }

            const T old_lambda = lambda;

            // figure out which lambda to try next
            lambda = lambda + std::pow(q_norm/p_norm,2)*(p_norm - radius)/radius;

            // make sure the chosen lambda is within our bracket (but not exactly at either end).
            const T gap = (lambda_max-lambda_min)*0.01;
            lambda = put_in_range(lambda_min+gap, lambda_max-gap, lambda);

            // Keep track of how much lambda is thrashing around inside the search bracket.  If it
            // keeps moving around a whole lot then cut the search bracket in half.
            lambda_delta += std::abs(lambda - old_lambda);
            if (lambda_delta > 3*(lambda_max-lambda_min))
            {
                lambda = (lambda_min+lambda_max)/2;
                lambda_delta = 0;
            }
        } // end for loop


        // We are probably in the "hard case".   Use an eigenvalue decomposition to sort things out.
        // Either that or the eps was just set too tight and really we are already done.
        eigenvalue_decomposition<EXP1> ed(make_symmetric(BB));

        matrix<T,NR,NC,MM,L> ev = ed.get_real_eigenvalues();
        const long min_eig_idx = index_of_min(ev);


        ev -= min(ev);
        // zero out any values which are basically zero
        ev = pointwise_multiply(ev, ev > max(abs(ev))*std::numeric_limits<T>::epsilon());
        ev = reciprocal(ev);


        // figure out part of what p should be assuming we are in the hard case.
        matrix<T,NR,NC,MM,L> p_hard;
        p_hard = trans(ed.get_pseudo_v())*gg;
        p_hard = diagm(ev)*p_hard;
        p_hard = ed.get_pseudo_v()*p_hard;


        // If we really are in the hard case then this if will be true.  Otherwise, the p
        // we found in the "easy case" loop up top is the best answer.
        if (length(p_hard) < radius && length(p_hard) >= length(p))
        {
            // adjust the length of p_hard by adding a component along the eigenvector associated with
            // the smallest eigenvalue.  We want to make it the case that length(p) == radius.
            const T tau = std::sqrt(radius*radius - length_squared(p_hard));
            p = p_hard + tau*colm(ed.get_pseudo_v(),min_eig_idx);


            // if we have to do an eigenvalue decomposition then say we did all the iterations
            return max_iter;
        }

        // if we get this far it means we didn't converge to eps accuracy. 
        return max_iter+1;
    }

// ----------------------------------------------------------------------------------------

    namespace impl
    {
        template <
            typename EXP1,
            typename EXP2,
            typename EXP3
            >
        bool bounds_violated (
            const matrix_exp<EXP1>& v,
            const matrix_exp<EXP2>& l,
            const matrix_exp<EXP3>& u
        )
        {
            DLIB_ASSERT(v.nr() == l.nr() && v.nr() == u.nr());
            DLIB_ASSERT(v.nc() == l.nc() && v.nc() == u.nc());
            for (long r = 0; r < v.nr(); ++r)
            {
                for (long c = 0; c < v.nc(); c++)
                {
                    if (!(l(r,c) <= v(r,c) && v(r,c) <= u(r,c)))
                        return true;
                }
            }
            return false;
        }
    }

// ----------------------------------------------------------------------------------------

    template <
        typename EXP1,
        typename EXP2,
        typename T, long NR, long NC, typename MM, typename L,
        typename EXP3
        >
    void solve_trust_region_subproblem_bounded ( 
        const matrix_exp<EXP1>& B_,
        const matrix_exp<EXP2>& g_,
        const typename EXP1::type radius_,
        matrix<T,NR,NC,MM,L>& p_,
        double eps,
        unsigned long max_iter,
        const matrix_exp<EXP3>& lower_,
        const matrix_exp<EXP3>& upper_
    )
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(B_.nr() == B_.nc() && is_col_vector(g_) && g_.size() == B_.nr(),
            "\t unsigned long solve_trust_region_subproblem_bounded()"
            << "\n\t invalid arguments were given to this function"
            << "\n\t B_.nr():            " << B_.nr()
            << "\n\t B_.nc():            " << B_.nc()
            << "\n\t is_col_vector(g_):  " << is_col_vector(g_) 
            << "\n\t g_.size():          " << g_.size() 
            );
        DLIB_ASSERT(radius_ > 0 && eps > 0 && max_iter > 0,
            "\t unsigned long solve_trust_region_subproblem_bounded()"
            << "\n\t invalid arguments were given to this function"
            << "\n\t radius_:   " << radius_
            << "\n\t eps:      " << eps 
            << "\n\t max_iter: " << max_iter 
            );
        DLIB_ASSERT(is_col_vector(lower_) && lower_.size() == g_.size());
        DLIB_ASSERT(is_col_vector(upper_) && upper_.size() == g_.size());
        DLIB_ASSERT(max(upper_-lower_) >= 0);
        // make sure the problem is feasible.  That is, there should be a point inside the
        // lower and upper bounds that has a norm <= radius_
        DLIB_ASSERT(length(clamp(zeros_matrix(lower_),lower_,upper_)) <= radius_, 
            "The lower and upper bounds are incompatible with the radius since there is no point within the bounds with a norm less than the radius.");

        // We are going to solve this by greedily finding the most violated bound constraint,
        // locking that variable to its constrained value, removing it from the problem,
        // and then resolving.  We do that until no more constraint violations are present.

        solve_trust_region_subproblem(B_,g_,radius_,p_,eps,max_iter);


        // just stop here if all the bounds are satisfied.
        if (!impl::bounds_violated(p_, lower_, upper_))
            return;

        matrix<double> B = matrix_cast<double>(B_);
        matrix<double,0,1> g = matrix_cast<double>(g_);
        double radius = radius_;
        matrix<double,0,1> p = matrix_cast<double>(p_);
        matrix<double,0,1> lower = matrix_cast<double>(lower_);
        matrix<double,0,1> upper = matrix_cast<double>(upper_);

        // keep a table that tells us how to map any reduced QP back to the original QP
        std::vector<long> idxs(g.size());
        for (size_t i = 0; i < idxs.size(); ++i)
            idxs[i] = i;


        // while we haven't found a p that satisfies the bounds constraints
        while(impl::bounds_violated(p, lower, upper) )
        {
            // Find the most violated variable and fix its value to a constant (the bound
            // value).
            long most_violated_idx = 0;
            double max_violation = 0;
            double bounded_value = 0;
            for (long i = 0; i < lower.size(); ++i)
            {
                if (!(lower(i) <= p(i) && p(i) <= upper(i)))
                {
                    if (lower(i)-p(i) > max_violation)
                    {
                        max_violation = lower(i)-p(i);
                        most_violated_idx = i;
                        bounded_value = lower(i);
                    }
                    else if (p(i)-upper(i) > max_violation)
                    {
                        max_violation = p(i)-upper(i);
                        most_violated_idx = i;
                        bounded_value = upper(i);
                    }
                }
            }

            // assign this variable to its final value.
            p_(idxs[most_violated_idx]) = bounded_value;


            // now reduce the QP by removing the variable p_(idxs[most_violated_idx]).  
            idxs.erase(idxs.begin()+most_violated_idx);
            // we are out of variables to remove since everything is at bounds.
            if (idxs.size() == 0)
                break;

            lower = remove_row(lower,most_violated_idx);
            upper = remove_row(upper,most_violated_idx);
            g += colm(B,most_violated_idx)*bounded_value;
            g = remove_row(g,most_violated_idx);
            p = remove_row(p,most_violated_idx);
            B = removerc(B,most_violated_idx, most_violated_idx);

            // Removing a variable changes the radius, so we have to subtract the bounded
            // value from the radius so as to not change the effective radius for the whole
            // problem.
            double squared_radius = radius*radius - bounded_value*bounded_value;
            if (squared_radius <= 0)
            {
                p = 0;
                break;
            }
            radius = std::sqrt(squared_radius);


            solve_trust_region_subproblem(B,g,radius,p,eps,max_iter);
        }

        // assign the non-bound-constrained variables to their final values
        for (size_t i = 0; i < idxs.size(); ++i)
            p_(idxs[i]) = p(i);

    }

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

    template <
        typename stop_strategy_type,
        typename funct_model
        >
    double find_min_trust_region (
        stop_strategy_type stop_strategy,
        const funct_model& model, 
        typename funct_model::column_vector& x, 
        double radius = 1
    )
    {
        /*
            This is an implementation of algorithm 4.1(Trust Region)
            from the book Numerical Optimization by Nocedal and Wright.  
        */

        // make sure requires clause is not broken
        DLIB_ASSERT(is_col_vector(x) && radius > 0,
            "\t double find_min_trust_region()"
            << "\n\t invalid arguments were given to this function"
            << "\n\t is_col_vector(x): " << is_col_vector(x) 
            << "\n\t radius:           " << radius
            );

        const double initial_radius = radius;

        typedef typename funct_model::column_vector T;
        typedef typename T::type type;

        typename funct_model::general_matrix h;
        typename funct_model::column_vector g, p, d;
        type f_value = model(x);

        model.get_derivative_and_hessian(x,g,h);

        DLIB_ASSERT(is_finite(x), "The objective function generated non-finite outputs");
        DLIB_ASSERT(is_finite(g), "The objective function generated non-finite outputs");
        DLIB_ASSERT(is_finite(h), "The objective function generated non-finite outputs");

        // Sometimes the loop below won't modify x because the trust region step failed.
        // This bool tells us when we are in that case.
        bool stale_x = false;

        while(stale_x || stop_strategy.should_continue_search(x, f_value, g))
        {
            const unsigned long iter = solve_trust_region_subproblem(h,
                                                                     g,
                                                                     radius,
                                                                     p, 
                                                                     0.1, 
                                                                     20);


            const type new_f_value = model(x+p);
            const type predicted_improvement = -0.5*trans(p)*h*p - trans(g)*p;
            const type measured_improvement = (f_value - new_f_value);

            // If the sub-problem can't find a way to improve then stop.  This only happens when p is essentially 0.
            if (std::abs(predicted_improvement) <= std::abs(measured_improvement)*std::numeric_limits<type>::epsilon())
                break;

            // predicted_improvement shouldn't be negative but it might be if something went
            // wrong in the trust region solver.  So put abs() here to guard against that.  This
            // way the sign of rho is determined only by the sign of measured_improvement.
            const type rho = measured_improvement/std::abs(predicted_improvement);


            if (!is_finite(rho))
                break;
            
            if (rho < 0.25)
            {
                radius *= 0.25;

                // something has gone horribly wrong if the radius has shrunk to zero.  So just
                // give up if that happens.
                if (radius <= initial_radius*std::numeric_limits<double>::epsilon())
                    break;
            }
            else
            {
                // if rho > 0.75 and we are being checked by the radius 
                if (rho > 0.75 && iter > 1)
                {
                    radius = std::min<type>(1000,  2*radius);
                }
            }

            if (rho > 0)
            {
                x = x + p;
                f_value = new_f_value;
                model.get_derivative_and_hessian(x,g,h);
                stale_x = false;
            }
            else
            {
                stale_x = true;
            }

            DLIB_ASSERT(is_finite(x), "The objective function generated non-finite outputs");
            DLIB_ASSERT(is_finite(g), "The objective function generated non-finite outputs");
            DLIB_ASSERT(is_finite(h), "The objective function generated non-finite outputs");
        }

        return f_value;
    }

// ----------------------------------------------------------------------------------------

    template <typename funct_model>
    struct negate_tr_model 
    {
        negate_tr_model( const funct_model& m) : model(m) {}

        const funct_model& model;

        typedef typename funct_model::column_vector column_vector;
        typedef typename funct_model::general_matrix general_matrix;

        template <typename T>
        typename T::type operator() (const T& x) const
        {
            return -model(x);
        }

        template <typename T, typename U>
        void get_derivative_and_hessian (
            const T& x,
            T& d,
            U& h
        ) const 
        {
            model.get_derivative_and_hessian(x,d,h);
            d = -d;
            h = -h;
        }

    };

// ----------------------------------------------------------------------------------------

    template <
        typename stop_strategy_type,
        typename funct_model
        >
    double find_max_trust_region (
        stop_strategy_type stop_strategy,
        const funct_model& model, 
        typename funct_model::column_vector& x, 
        double radius = 1
    )
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(is_col_vector(x) && radius > 0,
            "\t double find_max_trust_region()"
            << "\n\t invalid arguments were given to this function"
            << "\n\t is_col_vector(x): " << is_col_vector(x) 
            << "\n\t radius:           " << radius
            );

        return -find_min_trust_region(stop_strategy,
                                      negate_tr_model<funct_model>(model),
                                      x,
                                      radius);
    }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_OPTIMIZATION_TRUST_REGIoN_Hh_

