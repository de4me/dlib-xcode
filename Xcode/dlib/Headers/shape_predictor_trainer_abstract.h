// Copyright (C) 2014  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_SHAPE_PREDICToR_TRAINER_ABSTRACT_H_
#ifdef DLIB_SHAPE_PREDICToR_TRAINER_ABSTRACT_H_

#include <dlib/shape_predictor_abstract.h>
#include <dlib/image_dataset_metadata.h>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class shape_predictor_trainer
    {
        /*!
            WHAT THIS OBJECT REPRESENTS
                This object is a tool for training shape_predictors based on annotated training
                images.  Its implementation uses the algorithm described in:
                    One Millisecond Face Alignment with an Ensemble of Regression Trees
                    by Vahid Kazemi and Josephine Sullivan, CVPR 2014

        !*/

    public:

        shape_predictor_trainer (
        );
        /*!
            ensures
                - #get_cascade_depth() == 10
                - #get_tree_depth() == 4
                - #get_num_trees_per_cascade_level() == 500
                - #get_nu() == 0.1
                - #get_oversampling_amount() == 20
                - #get_oversampling_translation_jitter() == 0
                - #get_feature_pool_size() == 400
                - #get_lambda() == 0.1
                - #get_num_test_splits() == 20
                - #get_feature_pool_region_padding() == 0
                - #get_random_seed() == ""
                - #get_num_threads() == 0
                - #get_padding_mode() == landmark_relative 
                - This object will not be verbose
        !*/

        unsigned long get_cascade_depth (
        ) const;
        /*!
            ensures
                - returns the number of cascades created when you train a model.  This
                  means that the total number of trees in the learned model is equal to
                  get_cascade_depth()*get_num_trees_per_cascade_level().
        !*/

        void set_cascade_depth (
            unsigned long depth
        );
        /*!
            requires
                - depth > 0
            ensures
                - #get_cascade_depth() == depth
        !*/

        unsigned long get_tree_depth (
        ) const; 
        /*!
            ensures
                - returns the depth of the trees used in the cascade.  In particular, there
                  are pow(2,get_tree_depth()) leaves in each tree.
        !*/

        void set_tree_depth (
            unsigned long depth
        );
        /*!
            requires
                - depth > 0
            ensures
                - #get_tree_depth() == depth
        !*/

        unsigned long get_num_trees_per_cascade_level (
        ) const;
        /*!
            ensures
                - returns the number of trees created for each cascade.  This means that
                  the total number of trees in the learned model is equal to
                  get_cascade_depth()*get_num_trees_per_cascade_level().  
        !*/

        void set_num_trees_per_cascade_level (
            unsigned long num
        );
        /*!
            requires
                - num > 0
            ensures
                - #get_num_trees_per_cascade_level() == num
        !*/

        double get_nu (
        ) const; 
        /*!
            ensures
                - returns the regularization parameter.  Larger values of this parameter
                  will cause the algorithm to fit the training data better but may also
                  cause overfitting.
        !*/

        void set_nu (
            double nu
        );
        /*!
            requires
                - 0 < nu <= 1
            ensures
                - #get_nu() == nu
        !*/

        std::string get_random_seed (
        ) const;
        /*!
            ensures
                - returns the random seed used by the internal random number generator.
                  Since this algorithm is a random forest style algorithm it relies on a
                  random number generator for generating the trees.  So each setting of the
                  random seed will produce slightly different outputs.  
        !*/

        void set_random_seed (
            const std::string& seed
        );
        /*!
            ensures
                - #get_random_seed() == seed
        !*/

        unsigned long get_oversampling_amount (
        ) const;
        /*!
            ensures
                - You give annotated images to this object as training examples.  You
                  can effectively increase the amount of training data by adding in each
                  training example multiple times but with a randomly selected deformation
                  applied to it.  That is what this parameter controls.  That is, if you
                  supply N training samples to train() then the algorithm runs internally
                  with N*get_oversampling_amount() training samples.  So the bigger this
                  parameter the better (excepting that larger values make training take
                  longer).  In terms of the Kazemi paper, this parameter is the number of
                  randomly selected initial starting points sampled for each training
                  example.
        !*/

        void set_oversampling_amount (
            unsigned long amount
        );
        /*!
            requires
                - amount > 0
            ensures
                - #get_oversampling_amount() == amount
        !*/

        double get_oversampling_translation_jitter (
        ) const; 
        /*!
            ensures
                - When generating the get_oversampling_amount() factor of extra training
                  samples you can also jitter the bounding box by adding random small
                  translational shifts.  You can tell the shape_predictor_trainer to do
                  this by setting get_oversampling_translation_jitter() to some non-zero
                  value.  For instance, if you set it to 0.1 then it would randomly
                  translate the bounding boxes by between 0% and 10% their width and
                  height in the x and y directions respectively.  Doing this is essentially
                  equivalent to randomly jittering the bounding boxes in the training data
                  (i.e. the boxes given by full_object_detection::get_rect()).  This is
                  useful because the seed shape is determined by the bounding box position,
                  so doing this kind of jittering can help make the learned model more
                  robust against slightly misplaced bounding boxes.
        !*/

        void set_oversampling_translation_jitter (
            double amount
        );
        /*!
            requires
                - amount >= 0
            ensures
                - #get_oversampling_translation_jitter() == amount
        !*/

        unsigned long get_feature_pool_size (
        ) const;
        /*!
            ensures
                - At each level of the cascade we randomly sample get_feature_pool_size()
                  pixels from the image.  These pixels are used to generate features for
                  the random trees.  So in general larger settings of this parameter give
                  better accuracy but make the algorithm run slower.  
        !*/

        void set_feature_pool_size (
            unsigned long size
        );
        /*!
            requires
                - size > 1
            ensures
                - #get_feature_pool_size() == size
        !*/

        enum padding_mode_t
        {
            bounding_box_relative,
            landmark_relative 
        };

        padding_mode_t get_padding_mode (
        ) const; 
        /*!
            ensures
                - returns the current padding mode.  See get_feature_pool_region_padding()
                  for a discussion of the modes.
        !*/

        void set_padding_mode (
            padding_mode_t mode
        );
        /*!
            ensures
                - #get_padding_mode() == mode
        !*/

        double get_feature_pool_region_padding (
        ) const; 
        /*!
            ensures
                - This algorithm works by comparing the relative intensity of pairs of
                  pixels in the input image.  To decide which pixels to look at, the
                  training algorithm randomly selects pixels from a box roughly centered
                  around the object of interest.  We call this box the feature pool region
                  box.  
                  
                  Each object of interest is defined by a full_object_detection, which
                  contains a bounding box and a list of landmarks.  If
                  get_padding_mode()==landmark_relative then the feature pool region box is
                  the tightest box that contains the landmarks inside the
                  full_object_detection.  In this mode the full_object_detection's bounding
                  box is ignored.  Otherwise, if the padding mode is bounding_box_relative
                  then the feature pool region box is the tightest box that contains BOTH
                  the landmarks and the full_object_detection's bounding box.

                  Additionally, you can adjust the size of the feature pool padding region
                  by setting get_feature_pool_region_padding() to some value.  If
                  get_feature_pool_region_padding()==0 then the feature pool region box is
                  unmodified and defined exactly as stated above. However, you can expand
                  the size of the box by setting the padding > 0 or shrink it by setting it
                  to something < 0.

                  To explain this precisely, for a padding of 0 we say that the pixels are
                  sampled from a box of size 1x1.  The padding value is added to each side
                  of the box.  So a padding of 0.5 would cause the algorithm to sample
                  pixels from a box that was 2x2, effectively multiplying the area pixels
                  are sampled from by 4.  Similarly, setting the padding to -0.2 would
                  cause it to sample from a box 0.6x0.6 in size.
        !*/

        void set_feature_pool_region_padding (
            double padding 
        );
        /*!
            requires
                - padding > -0.5
            ensures
                - #get_feature_pool_region_padding() == padding
        !*/

        double get_lambda (
        ) const;
        /*!
            ensures
                - To decide how to split nodes in the regression trees the algorithm looks
                  at pairs of pixels in the image.  These pixel pairs are sampled randomly
                  but with a preference for selecting pixels that are near each other.
                  get_lambda() controls this "nearness" preference.  In particular, smaller
                  values of get_lambda() will make the algorithm prefer to select pixels
                  close together and larger values of get_lambda() will make it care less
                  about picking nearby pixel pairs.  

                  Note that this is the inverse of how it is defined in the Kazemi paper.
                  For this object, you should think of lambda as "the fraction of the
                  bounding box will we traverse to find a neighboring pixel".  Nominally,
                  this is normalized between 0 and 1.  So reasonable settings of lambda are
                  values in the range 0 < lambda < 1.
        !*/

        void set_lambda (
            double lambda
        );
        /*!
            requires
                - lambda > 0
            ensures
                - #get_lambda() == lambda
        !*/

        unsigned long get_num_test_splits (
        ) const;
        /*!
            ensures
                - When generating the random trees we randomly sample get_num_test_splits()
                  possible split features at each node and pick the one that gives the best
                  split.  Larger values of this parameter will usually give more accurate
                  outputs but take longer to train.
        !*/

        void set_num_test_splits (
            unsigned long num
        );
        /*!
            requires
                - num > 0
            ensures
                - #get_num_test_splits() == num
        !*/

        unsigned long get_num_threads (
        ) const;
        /*!
            ensures
                - When running training process, it is possible to make some parts of it parallel
                  using CPU threads with #parallel_for() extension and creating #thread_pool internally
                  When get_num_threads() == 0, trainer will not create threads and all processing will
                  be done in the calling thread
        !*/

        void set_num_threads (
            unsigned long num
        );
        /*!
            requires
                - num >= 0
            ensures
                - #get_num_threads() == num
        !*/

        void be_verbose (
        );
        /*!
            ensures
                - This object will print status messages to standard out so that a 
                  user can observe the progress of the algorithm.
        !*/

        void be_quiet (
        );
        /*!
            ensures
                - This object will not print anything to standard out
        !*/

        template <typename image_array>
        shape_predictor train (
            const image_array& images,
            const std::vector<std::vector<full_object_detection> >& objects
        ) const;
        /*!
            requires
                - image_array is a dlib::array of image objects where each image object
                  implements the interface defined in dlib/image_processing/generic_image.h 
                - images.size() == objects.size()
                - images.size() > 0
                - for some i: objects[i].size() != 0
                  (i.e. there has to be at least one full_object_detection in the training set)
                - for all valid p, there must exist i and j such that: 
                  objects[i][j].part(p) != OBJECT_PART_NOT_PRESENT.
                  (i.e. You can't define a part that is always set to OBJECT_PART_NOT_PRESENT.)
                - for all valid i,j,k,l:
                    - objects[i][j].num_parts() == objects[k][l].num_parts()
                      (i.e. all objects must agree on the number of parts)
                    - objects[i][j].num_parts() > 0
            ensures
                - This object will try to learn to predict the locations of an object's parts 
                  based on the object bounding box (i.e.  full_object_detection::get_rect()) 
                  and the image pixels in that box.  That is, we will try to learn a
                  shape_predictor, SP, such that:
                    SP(images[i], objects[i][j].get_rect()) == objects[i][j]
                  This learned SP object is then returned.
                - Not all parts are required to be observed for all objects.  So if you
                  have training instances with missing parts then set the part positions
                  equal to OBJECT_PART_NOT_PRESENT and this algorithm will basically ignore
                  those missing parts.
        !*/
    };

// ----------------------------------------------------------------------------------------

    template <
        typename some_type_of_rectangle
        >
    image_dataset_metadata::dataset make_bounding_box_regression_training_data (
        const image_dataset_metadata::dataset& truth,
        const std::vector<std::vector<some_type_of_rectangle>>& detections
    );
    /*!
        requires
            - truth.images.size() == detections.size()
            - some_type_of_rectangle == rectangle, drectangle, mmod_rect, or any other type
              that is convertible to a rectangle.
        ensures
            - Suppose you have an object detector that can roughly locate objects in an
              image.  This means your detector draws boxes around objects, but these are
              *rough* boxes in the sense that they aren't positioned super accurately.  For
              instance, HOG based detectors usually have a stride of 8 pixels.  So the
              positional accuracy is going to be, at best, +/-8 pixels.  
              
              If you want to get better positional accuracy one easy thing to do is train a
              shape_predictor to give you the location of the object's box.  The
              make_bounding_box_regression_training_data() routine helps you do this by
              creating an appropriate training dataset.  It does this by taking the dataset
              you used to train your detector (given by the truth object), and combining
              that with the output of your detector on each image in the training dataset
              (given by the detections object).  In particular, it will create a new
              annotated dataset where each object box is one of the rectangles from
              detections and that object has 5 part annotations.  These annotations
              identify the sides and middle of the truth rectangle corresponding to the
              detection rectangle.  You can then take the returned dataset and train a
              shape_predictor on it.  The resulting shape_predictor can then be used to do
              bounding box regression.  
              
              As an aside, the reason we create 5 part annotations in this way is because
              it gives the best shape_predictor when trained.  If instead you used the 4
              corners it wouldn't work as well, due to tedious vagaries of the shape_predictor 
              training process.

            - We assume that detections[i] contains object detections corresponding to 
              the image truth.images[i].
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_SHAPE_PREDICToR_TRAINER_ABSTRACT_H_

