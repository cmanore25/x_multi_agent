/*
 * Copyright 2020 California  Institute  of Technology (“Caltech”)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef X_VIO_MSCKF_SLAM_UPDATE_H_
#define X_VIO_MSCKF_SLAM_UPDATE_H_

#include "x/vio/types.h"
#include "x/vio/update.h"
#include "x/vision/triangulation.h"

namespace x {
/**
 * MSCKF-SLAM update and feature state initialization.
 *
 * Implementation of the Multi-State Contraint Kalman Filter update and SLAM
 * features initialization in the state vector and error state covariance
 * matrix. The major difference wrt the standard MSCKF update is that feature
 * is expressed in inverse-depth coordinates, as opposed to cartesian, which
 * affects the nullspace projection. As presented Li 2012 ICRA paper and xVIO
 * tech report.
 */
class MsckfSlamUpdate : public Update {
 public:
  /**
   * Default constructor
   */
  MsckfSlamUpdate(){};

  /**
   * Constructor.
   *
   * Does the full update matrix construction job.
   *
   * @param[in] tkrs Feature tracks in normalized coordinates
   * @param[in] quats Camera quaternion states
   * @param[in] poss Camera position states
   * @param[in] triangulator Feature triangulator
   * @param[in] cov_s Error state covariance
   * @param[in] n_poses_max Maximum number of poses in sliding window.
   * @param[in] sigma_img Standard deviation of feature measurement
   *                      [in normalized coordinates]
   */
  MsckfSlamUpdate(const x::TrackList& trks, const x::AttitudeList& quats,
                  const x::TranslationList& poss,
                  const Triangulation& triangulator,
                  const Eigen::MatrixXd& cov_s, int n_poses_max,
                  double sigma_img);

  /////////////////////////////// Getters //////////////////////////////////

  /**
   * Returns a constant reference to the MSCKF-SLAM initialization matrices.
   */
  [[nodiscard]] const x::MsckfSlamMatrices& getInitMats() const {
    return init_mats_;
  };

  /**
   * Returns a constant reference to the inliers 3D cartesian coodinates.
   */
  [[nodiscard]] const Vector3dArray& getInliers() const { return inliers_; };

  /**
   * Returns a constant reference to the outliers 3D cartesian coodinates.
   */
  [[nodiscard]] const Vector3dArray& getOutliers() const { return outliers_; };

 private:
  /**
   * MSCKF-SLAM initialization matrices
   */
  x::MsckfSlamMatrices init_mats_;

  /**
   * 3D cartesian coordinates prior for MSCKF feature inliers
   */
  Vector3dArray inliers_;

  /**
   * 3D cartesian coordinates prior for MSCKF feature outliers
   */
  Vector3dArray outliers_;

  /**
   * Process one feature track.
   *
   *  @param[in] track Feature track in normalized coordinates
   *  @param[in] C_q_G Camera attitude state list
   *  @param[in] G_p_C Camera position state list
   *  @param[in] triangulator Feature triangulator
   *  @param[in] P State covariance
   *  @param[in] n_poses_max Maximum number of poses in sliding window.
   *  @param[in] var_img Variance of feature measurements
   *                     [in normalized coordinates]
   *  @param[in] j Index of current track in the MSCKF track list
   *  @param[out] row_h Current rows in stacked Jacobian
   *  @param[out] row1 Current rows in stacked MSCKF-SLAM matrices
   */
  void processOneTrack(const x::Track& track, const x::AttitudeList& C_q_G,
                       const x::TranslationList& G_p_C,
                       const Triangulation& triangulator,
                       const Eigen::MatrixXd& P, int n_poses_max,
                       double var_img, int j, int& row_h, int& row1);
};
}  // namespace x

#endif  // X_VIO_MSCKF_SLAM_UPDATE_H_
