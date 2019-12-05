/*
 * Link moveit's robot model with Descartes' robot model and plug in our custom IK solver
 */

#ifndef JACO3_MOVEIT_STATE_ADAPTER_H
#define JACO3_MOVEIT_STATE_ADAPTER_H

#include "descartes_moveit/moveit_state_adapter.h"
#include <peanut_kinematics/jaco3_ik.h>
#include <moveit_msgs/PlanningScene.h>

namespace descartes_moveit
{
class Jaco3MoveitStateAdapter : public descartes_moveit::MoveitStateAdapter
{
public:
  virtual ~Jaco3MoveitStateAdapter()
  {
  }

  virtual bool initialize(const std::string& robot_description, const std::string& group_name,
                          const std::string& world_frame, const std::string& tcp_frame);

  virtual bool sampleRedundantJoint(std::vector<double>& sampled_joint_vals) const;

  virtual bool getAllIK(const Eigen::Affine3d& pose, std::vector<std::vector<double> >& joint_poses) const;

  virtual bool getIK(const Eigen::Affine3d& pose, const std::vector<double>& seed_state,
                     std::vector<double>& joint_pose) const;

  virtual bool getFK(const std::vector<double>& joint_pose, Eigen::Affine3d& pose) const;

  virtual bool isValid(const std::vector<double>& joint_pose) const;

  virtual bool updatePlanningScene(const moveit_msgs::PlanningScene &scene);

  /**
   * @brief Sets the internal state of the robot model to the argument. For the IKFast impl,
   * it also recomputes the transformations to/from the IKFast reference frames.
   */
  void setState(const moveit::core::RobotState& state);

  bool hasNaN(const std::vector<double> &joint_pose) const;
  
protected:
  bool computeJaco3Transforms();

  virtual bool isInCollision(const std::vector<double>& joint_pose) const;

  /**
   * Update the collision_arm_links_ and collision_arm_robot_links
   */
  virtual void setCollisionLinks(std::vector<std::string> arm_links, std::vector<std::string> robot_links);

  /**
   * The IKFast implementation commonly solves between 'base_link' of a robot
   * and 'tool0'. We will commonly want to take advantage of an additional
   * fixed transformation from the robot flange, 'tool0', to some user defined
   * tool. This prevents the user from having to manually adjust tool poses to
   * account for this.
   */
  descartes_core::Frame tool0_to_tip_;

  /**
   * Likewise this parameter is used to accomodate transformations between the base
   * of the IKFast solver and the base of the MoveIt move group.
   */
  descartes_core::Frame world_to_base_;

  collision_detection::AllowedCollisionMatrix acm_;
  collision_detection::CollisionRequest collision_request_;
  std::string octomap_link_ = "<octomap>";
  std::vector<std::string> collision_arm_links_ = {"half_arm_2_link", "forearm_link"};
  std::vector<std::string> collision_robot_links_ = {"tower_link", "camera_box"};
};

}  // end namespace 'descartes_moveit'
#endif