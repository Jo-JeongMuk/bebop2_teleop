#include "ros/ros.h"
#include "bebop2_teleop.h"
#include <iostream>

using std::cout;    using std::endl;

void message::printMsg() const {
    cout << this->messege << endl;
}

void message::printValue(float speed, float turn, char key) const {
    cout << "\rCurrent: speed " << speed << "\tturn " << turn << " | Invalid command! " << key;
}

int input::get() {
    tcgetattr(STDIN_FILENO, &this->oldt); // 터미널 속성을 얻는다.
    this->newt = this->oldt;
    this->newt.c_lflag &= ~(ICANON | ECHO); //로컬모드 &= 입력문자의 정식입력처리 | 에코
    this->newt.c_iflag |= IGNBRK; //입력 모드 |= 브레이크 조건 무시
    this->newt.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF); // 입력모드 &= NL을 CR로 변경 | CR을 NL로 변경 |
    this->newt.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN); // 로컬모드 &= 캐노니컬 입력 모드 | 에코 | ?? | 삭제문자 다시 출력 | NL문자 다시 출력 | ??
    this->newt.c_cc[VMIN] = 1; // 지원 가능한 기능에 대응되는 문자들 포함
    this->newt.c_cc[VTIME] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &this->newt); // 터미널 속성 설정(속성바로 변경)

    this->ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &this->oldt);

    return this->ch;
}
void conturol::move() {
    input teleopKey;
    message msg;
    this->key = teleopKey.get();

    if (this->Bindings.count(this->key) == 1) {
        for(int i = 0; i < 4; ++i)
            this->linearXYZ_AngularZ[i] = this->Bindings[this->key][i];

        msg.printValue(this->speed, this->turn, this->key);
    }
    else if (this->speedBindings.count(this->key) == 1) {
        (this->speed > MAX_SPEED) ? this->speed = MAX_SPEED : (this->speed < MIN_SPEED) ?
            this->speed = MIN_SPEED : this->speed = this->speed * this->speedBindings[this->key];
        msg.printValue(this->speed, this->turn, this->key);
    }

    else if (this->key == '\x20')
        if (this->takeOfforLanding == false) {
            takeoff.publish(empty_msg);
            this->takeOfforLanding = true;
        }
        else {
            land.publish(empty_msg);
            this->takeOfforLanding = false;
        }

    else if (this->key == '\x1B')
        reset.publish(empty_msg);

    else if (this->key == 'q')
        snapshot.publish(empty_msg);

    else if (this->key == 'e') {
        bool_msg.data = true;
        record.publish(bool_msg);
    }

    else if (this->key == 'c') {
        bool_msg.data = false;
        record.publish(bool_msg);
    }

    else {
        this->linearXYZ_AngularZ = {0, 0, 0, 0};
        msg.printValue(this->speed, this->turn, this->key);
    }

    twist.linear.x = this->linearXYZ_AngularZ[0] * this->speed;
    twist.linear.y = this->linearXYZ_AngularZ[1] * this->speed;
    twist.linear.z = this->linearXYZ_AngularZ[2] * this->speed;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = this->linearXYZ_AngularZ[3] * this->turn;

    go.publish(twist);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "teleop_twist_keyboard");
    ros::NodeHandle nh;
    go = nh.advertise<geometry_msgs::Twist>("bebop/cmd_vel", 1);
    takeoff = nh.advertise<std_msgs::Empty>("bebop/takeoff", 1);
    land = nh.advertise<std_msgs::Empty>("bebop/land", 1);
    reset = nh.advertise<std_msgs::Empty>("bebop/reset", 1);
    snapshot = nh.advertise<std_msgs::Empty>("bebop/snapshot", 1);
    record = nh.advertise<std_msgs::Bool>("bebop/record", 1);

    message msg;
    msg.printMsg();
    conturol move;

    cout << "\rCurrent: speed " << move.speed << "\tturn " << move.turn << " | Awaiting command...\r";

    while(true){
        move.move();
        if (move.key == '\x03')
            break;
        try {
            ros::spinOnce();
        } catch (...) {
            ROS_ERROR("--- ERROR IN spin(), shutting down! ---");
            ros::shutdown();
        }
    }
    return 0;
}