#ifndef BEBOP2_TELEOP
#define BEBOP2_TELEOP

#include <geometry_msgs/Twist.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Bool.h>
#include <map>
#include <vector>
#include <termios.h>

using std::map;     using std::vector;

#define MAX_SPEED  0.8
#define MIN_SPEED  0.2
#define BASE_SPEED 0.5
#define BASE_TURN  1.0

geometry_msgs::Twist twist;
std_msgs::Empty      empty_msg;
std_msgs::Bool       bool_msg;

ros::Publisher go;
ros::Publisher takeoff;
ros::Publisher land;
ros::Publisher reset;
ros::Publisher snapshot;
ros::Publisher record;

class message {
private:
    const char *messege = R"(
    Control Your Bebop2.0!
    ----------------------------------------------------

    SpaceBar : Take off / Landing
    Esc      : force stop



        Left hand side              Right hand side

            w(up)                       i(^)

    a(CCW)  s(down)  d(CW)         j(<)  k(v)  l(>)



    + / - : increase / decrease speed ##### Warning #####
    q     : Snapshot
    e / c : Recording On / Off

    CTRL-C to quit
    ----------------------------------------------------
    )";

public:
    void printMsg() const;
    void printValue(float speed, float turn, char key) const;
};

class conturol {
private:
    map<char, vector<float>> Bindings {
            {'i', {1,  0,  0,  0}}, {'k', {-1, 0,  0,  0}}, {'j', {0,  1,  0,  0}}, {'l', {0,  -1, 0,  0}},
            {'w', {0,  0,  1,  0}}, {'s', {0,  0,  -1, 0}}, {'a', {0,  0,  0,  1}}, {'d', {0,  0,  0, -1}}
    };
    map<char, float> speedBindings {{'+', 1.1}, {'-', 0.9},};
    vector<float> linearXYZ_AngularZ = {0, 0, 0, 0};
    bool takeOfforLanding = false;

public:
    char key = ' ';
    float speed = BASE_SPEED;
    float turn = BASE_TURN;
    void move();
};

class input {
private:
    int ch;
    struct termios oldt;
    struct termios newt;

public:
    int get();
};

#endif
