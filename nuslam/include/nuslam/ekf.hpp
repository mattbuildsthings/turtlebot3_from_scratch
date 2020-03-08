#ifndef EKF_INCLUDE_GUARD_HPP
#define EKF_INCLUDE_GUARD_HPP
/// \file
/// \brief Library EKF EKF detection and classification.
#include <rigid2d/rigid2d.hpp>
#include <rigid2d/diff_drive.hpp>
#include <nuslam/landmarks.hpp>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <numeric>
#include <functional>
#include <ros/ros.h>
#include <limits>  // set variable to max (inf)
#include<random>  // to seed common random num gen

namespace nuslam
{
    // Used to for prediction stage
    using rigid2d::Twist2D;

    // used for model update
    using rigid2d::Pose2D;

    // used for map update
    using rigid2d::Vector2D;

    // Struct to store Covariance Matrix for EKF
    struct CovarianceMatrix
    {
        // Populates top left of covariance matrix for robot
        std::vector<double> robot_state_cov;

        // holds Landmarks x,y
        std::vector<Vector2D> map_state;

        // Populates bottom right of covariance matrix for landmarks
        std::vector<double> map_state_cov;

        // Covariance Matrix
        Eigen::MatrixXd cov_mtx;

        /// \brief constructor for covariance matrix with robot_state_cov init to zero
        /// and map_state_cov init to infinity but with no recorded landmarks
        CovarianceMatrix();

        /// \brief constructor for covariance matrix with robot_state_cov init to zero
        /// and map_state_cov init to infinity
        CovarianceMatrix(const std::vector<Vector2D> & map_state_);


        /// \brief constructor for covariance matrix with robot_state_cov
        /// and map_state_cov init to user-specified input
        CovarianceMatrix(const std::vector<Vector2D> & map_state_, \
                         const std::vector<double> & robot_state_cov_,\
                         const std::vector<double> & map_state_cov_);

    };

    // Struct to store Process Noise for ERK
    struct ProcessNoise
    {
        // Store Covariance Matrix
        CovarianceMatrix cov_mtx;

        // Contains noise for x,y,theta
        Pose2D xyt_noise;

        // Process Noise of robot
        Eigen::MatrixXd q;

        // Process Noise Matrix
        Eigen::MatrixXd Q;

        /// \brief constructor for process noise matrix with xyt_noise set to zero
        ProcessNoise();

        /// \brief constructor for process noise matrix with xyt_noise set to user input
        ProcessNoise(const Pose2D & xyt_noise_var, const CovarianceMatrix & cov_mtx);
    };

    // Struct to store Measurement Noise for ERK
    struct MeasurementNoise
    {
    };

    /// \brief handles model propagation for EKF SLAM
    class EKF
    {
    public:
        /// \brief the default constructor creates a EKF with zero-initialized pose and no landmarks
        /// Start with guess of robot state (0,0,0) with zero covariance for robot state, indicating
        /// full confidence in initial state, and infinite covariance for ladmarks state, indicating we
        /// know nothing about them.
        EKF();

        /// \brief the default constructor creates a EKF with user-defined number of landmarks
        /// Start with guess of robot state (0,0,0) with zero covariance for robot state, indicating
        /// full confidence in initial state, and infinite covariance for ladmarks state, indicating we
        /// know nothing about them.
        EKF(const Pose2D & robot_state_, const std::vector<Vector2D> & map_state_, const Pose2D & xyt_noise_var);

        /// \brief forward-propagate the nonlinear motion model to get an estimate (prediction, and, using
        /// Taylor-Series expantion, get a linearized state transition model, which is used to propagate uncertainty.
        /// \param
        /// \returns
        void predict(const Twist2D & twist, const Pose2D & xyt_noise_mean);

        /// \brief incorporate sequential landmark measurements to perform a correction of our predicted estimate, 
        /// then, update the EKF parameters for the next ieration. Also initializes new landmarks
        /// \param
        /// \returns
        void update();

    private:
        Pose2D robot_state;
        std::vector<Vector2D> map_state;
        ProcessNoise proc_noise;
        CovarianceMatrix cov_mtx;

    };

    /// \brief create random number generator with common seed
    std::mt19937 & get_random();

    /// \brief sample normal distribution
    double sampleNormalDistribution();

    /// \brief returns noise for each dimension (x,y,theta) extracted from
    /// 3D normal distribution using Cholesky Decomposition
    // This is used to sample noise for the state update function
    std::vector<double> get_3d_noise(const Pose2D & xyt_noise_mean, const ProcessNoise & proc_noise);
}

#endif