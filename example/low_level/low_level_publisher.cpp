#include <booster/robot/channel/channel_publisher.hpp>
#include <booster/robot/b1/b1_api_const.hpp>
#include <booster/idl/b1/LowCmd.h>
#include <booster/idl/b1/MotorCmd.h>

#include <thread>
#include <chrono>
#include <iostream>

#define SLEEP_TIME 2

using namespace booster::robot;
using namespace booster_interface::msg;

int main() {
    ChannelFactory::Instance()->Init(0);
    ChannelPublisher<LowCmd> channel_publisher(b1::kTopicJointCtrl);
    channel_publisher.InitChannel();
    std::vector<MotorCmd> motor_cmds;
    for (size_t i = 0; i < 22; i++) {
        MotorCmd motor_cmd;
        motor_cmds.push_back(motor_cmd);
    }
    while (true) {
        LowCmd low_cmd;
        low_cmd.cmd_type(PARALLEL);
        low_cmd.motor_cmd(motor_cmds);
        for (size_t i = 0; i < b1::kJointCnt; i++) {
            low_cmd.motor_cmd()[i].q(0.0);
            low_cmd.motor_cmd()[i].dq(0.0);
            low_cmd.motor_cmd()[i].tau(0.0);
            low_cmd.motor_cmd()[i].kp(10.0);
            low_cmd.motor_cmd()[i].kd(1.0);
            low_cmd.motor_cmd()[i].weight(0.0);
        }

        channel_publisher.Write(&low_cmd);
        std::cout << "Publish LowCmd" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    }
}