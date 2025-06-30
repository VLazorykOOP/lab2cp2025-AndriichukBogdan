#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

#define _USE_MATH_DEFINES
#include <math.h> 

std::mutex print_mutex;

class Ant {
protected:
    double x, y;
    double speed;
    string name;
public:
    Ant(string name, double x, double y, double speed)
        : name(name), x(x), y(y), speed(speed) {}

    virtual void move() = 0;

    virtual ~Ant() {}
};

class WorkerAnt : public Ant {
    double start_x, start_y;
public:
    WorkerAnt(string name, double x, double y, double speed)
        : Ant(name, x, y, speed), start_x(x), start_y(y) {}

    void move() override {

        while (abs(x) > 0.1 || abs(y) > 0.1) {
            double dx = -x;
            double dy = -y;
            double length = sqrt(dx * dx + dy * dy);
            double step_x = speed * dx / length;
            double step_y = speed * dy / length;

            x += step_x;
            y += step_y;
            {
                lock_guard<mutex> lock(print_mutex);
                cout << name << " Go to [0,0]: (" << x << ", " << y << ")\n";
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }


        while (abs(x - start_x) > 0.1 || abs(y - start_y) > 0.1) {
            double dx = start_x - x;
            double dy = start_y - y;
            double length = sqrt(dx * dx + dy * dy);
            double step_x = speed * dx / length;
            double step_y = speed * dy / length;

            x += step_x;
            y += step_y;
            {
                lock_guard<mutex> lock(print_mutex);
                cout << name << " Return to spawn: (" << x << ", " << y << ")\n";
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        {
            lock_guard<mutex> lock(print_mutex);
            cout << name << " Done.\n";
        }
    }
};

class WarriorAnt : public Ant {
    double radius;
public:
    WarriorAnt(string name, double radius, double speed)
        : Ant(name, radius, 0, speed), radius(radius) {}

    void move() override {
        double angle = 0;
        while (true) {

            x = radius * cos(angle);
            y = radius * sin(angle);
            {
                lock_guard<mutex> lock(print_mutex);
                cout << name << " Defending: (" << x << ", " << y << ")\n";
            }
            angle += speed / radius;
            if (angle > 2 * M_PI) angle -= 2 * M_PI;
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        {
            lock_guard<mutex> lock(print_mutex);
            cout << name << " Done.\n";
        }
    }
};

int main() {
    vector<thread> threads;

    threads.emplace_back(&WorkerAnt::move, WorkerAnt("Worker 1", 10, 5, 0.5));
    threads.emplace_back(&WorkerAnt::move, WorkerAnt("Worker 2", 8, 4, 0.4));
    threads.emplace_back(&WarriorAnt::move, WarriorAnt("Warrior 1", 5.0, 0.2));
    threads.emplace_back(&WarriorAnt::move, WarriorAnt("Warrior 2", 6.0, 0.3));

    for (auto& t : threads) {
        t.join();
    }

    cout << "End of simulation.\n";
    return 0;
}
