#include <cstdio>
#include <string>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include <memory>
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class PlannerNode : public rclcpp::Node
{
public:
  PlannerNode(std::string &data)
  : Node("planner_node"),
  str(data) 
  {
    callback_group_ = create_callback_group(rclcpp::CallbackGroupType::Reentrant);
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
    timer_ = this->create_wall_timer(
      1000ms, std::bind(&PlannerNode::timer_callback, this));
  };

private:

  void timer_callback()
  {
    RCLCPP_INFO(this->get_logger(), "Increasing string");
    str += "a";
    auto msg = std_msgs::msg::String();
    msg.data = str;
    publisher_->publish(msg);
  }

  std::string &str;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::CallbackGroup::SharedPtr callback_group_;
};

class BehaviorNode : public rclcpp::Node
{
public:
  BehaviorNode(std::string &data)
  : Node("behavior_node"),
    str(data) 
  {
    
    callback_group_ = create_callback_group(rclcpp::CallbackGroupType::Reentrant);
    str = data; 
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic2", 10);
    timer_ = this->create_wall_timer(
      100ms, std::bind(&BehaviorNode::timer_callback, this));
  };

private:
  void timer_callback()
  {
    RCLCPP_INFO(this->get_logger(), "Reading string");
    auto msg = std_msgs::msg::String();
    RCLCPP_INFO(this->get_logger(), "Add to string");
    str += "b";
    msg.data = str;
    publisher_->publish(msg);
  }

  std::string &str;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::CallbackGroup::SharedPtr callback_group_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  std::string data = "a";

  rclcpp::Node::SharedPtr node = std::make_shared<PlannerNode>(data);
  rclcpp::Node::SharedPtr node1 = std::make_shared<BehaviorNode>(data);

  // The single threaded executor uses one thread to handle incoming messages and events. 
  // The single threaded exectuor is also used for components. 
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  executor.add_node(node1);
  executor.spin();

  rclcpp::shutdown();

  return 0;
}
