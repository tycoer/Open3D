// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <tuple>
#include <vector>

#include "open3d/core/Tensor.h"
#include "open3d/t/pipelines/registration/TransformationEstimation.h"

namespace open3d {
namespace t {

namespace geometry {
class PointCloud;
}

namespace pipelines {
namespace registration {
class Feature;

/// \class ICPConvergenceCriteria
///
/// \brief Class that defines the convergence criteria of ICP.
///
/// ICP algorithm stops if the relative change of fitness and rmse hit
/// \p relative_fitness_ and \p relative_rmse_ individually, or the iteration
/// number exceeds \p max_iteration_.
class ICPConvergenceCriteria {
public:
    /// \brief Parameterized Constructor.
    ///
    /// \param relative_fitness If relative change (difference) of fitness score
    /// is lower than relative_fitness, the iteration stops. \param
    /// relative_rmse If relative change (difference) of inliner RMSE score is
    /// lower than relative_rmse, the iteration stops. \param max_iteration
    /// Maximum iteration before iteration stops.
    ICPConvergenceCriteria(double relative_fitness = 1e-6,
                           double relative_rmse = 1e-6,
                           int max_iteration = 30)
        : relative_fitness_(relative_fitness),
          relative_rmse_(relative_rmse),
          max_iteration_(max_iteration) {}
    ~ICPConvergenceCriteria() {}

public:
    /// If relative change (difference) of fitness score is lower than
    /// `relative_fitness`, the iteration stops.
    double relative_fitness_;
    /// If relative change (difference) of inliner RMSE score is lower than
    /// `relative_rmse`, the iteration stops.
    double relative_rmse_;
    /// Maximum iteration before iteration stops.
    int max_iteration_;
};

/// \class RegistrationResult
///
/// Class that contains the registration results.
class RegistrationResult {
public:
    /// \brief Parameterized Constructor.
    ///
    /// \param transformation The estimated transformation matrix.
    RegistrationResult(const core::Tensor &transformation /*= core::Tensor::Eye(
                               4, core::Dtype::Float64, core::Device("CPU:0"))*/)
        : transformation_(transformation), inlier_rmse_(0.0), fitness_(0.0) {}
    ~RegistrationResult() {}
    bool IsBetterRANSACThan(const RegistrationResult &other) const {
        return fitness_ > other.fitness_ || (fitness_ == other.fitness_ &&
                                             inlier_rmse_ < other.inlier_rmse_);
    }

public:
    /// The estimated transformation matrix.
    core::Tensor transformation_;
    /// Correspondence set between source and target point cloud.
    core::Tensor correspondence_set_;
    /// Bool Indexing Tensor, indicating valid Correspondences in Correspondace
    /// set.
    core::Tensor correspondence_select_bool_;
    /// RMSE of all inlier correspondences. Lower is better.
    double inlier_rmse_;
    /// For ICP: the overlapping area (# of inlier correspondences / # of points
    /// in target). Higher is better.
    /// For RANSAC: inlier ratio (# of inlier correspondences / # of
    /// all correspondences)
    double fitness_;
};

/// \brief Function for evaluating registration between point clouds.
///
/// \param source The source point cloud.
/// \param target The target point cloud.
/// \param max_correspondence_distance Maximum correspondence points-pair
/// distance. \param transformation The 4x4 transformation matrix to transform
/// source to target. Default value: array([[1., 0., 0., 0.], [0., 1., 0., 0.],
/// [0., 0., 1., 0.], [0., 0., 0., 1.]]).
RegistrationResult EvaluateRegistration(
        const geometry::PointCloud &source,
        const geometry::PointCloud &target,
        double max_correspondence_distance,
        const core::Tensor &transformation /*= core::Tensor::Eye(
                4, core::Dtype::Float64, core::Device("CPU:0"))*/);

/// \brief Functions for ICP registration.
///
/// \param source The source point cloud.
/// \param target The target point cloud.
/// \param max_correspondence_distance Maximum correspondence points-pair
/// distance. \param init Initial transformation estimation.
///  Default value: array([[1., 0., 0., 0.], [0., 1., 0., 0.], [0., 0., 1., 0.],
///  [0., 0., 0., 1.]])
/// \param estimation Estimation method.
/// \param criteria Convergence criteria.
RegistrationResult RegistrationICP(
        const geometry::PointCloud &source,
        const geometry::PointCloud &target,
        double max_correspondence_distance,
        const core::Tensor &init /*= core::Tensor::Eye(4,
                                                     core::Dtype::Float64,
                                                     core::Device("CPU:0"))*/
        ,
        const TransformationEstimation &estimation =
                TransformationEstimationPointToPoint(false),
        const ICPConvergenceCriteria &criteria = ICPConvergenceCriteria());

// [TODO: To Be Implemented]
/// \param source The source point cloud.
/// \param target The target point cloud.
/// \param max_correspondence_distance Maximum correspondence points-pair
/// distance. \param transformation The 4x4 transformation matrix to transform
/// `source` to `target`.
// core::Tensor GetInformationMatrixFromPointClouds(
//         const geometry::PointCloud &source,
//         const geometry::PointCloud &target,
//         double max_correspondence_distance,
//         const core::Tensor &transformation);

}  // namespace registration
}  // namespace pipelines
}  // namespace t
}  // namespace open3d
