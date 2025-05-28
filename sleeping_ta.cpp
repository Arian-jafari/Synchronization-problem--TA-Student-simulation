#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <semaphore>
#include <mutex>
#include <vector>

#define MAX_STUDENTS 10
#define NUM_CHAIRS 5

std::counting_semaphore<MAX_STUDENTS + 1> ta_sem(0);
std::counting_semaphore<MAX_STUDENTS + 1> st_sem(0);
std::mutex mtx;
std::vector<int> waiting_students;

std::random_device rd;  
std::mt19937 gen(rd());  
std::uniform_int_distribution<> dist(1, 5);



void teacher() {
    int iter_count = 0;
    while (iter_count < MAX_STUDENTS) {
        if (waiting_students.size() == 0)
            std::cout << "TA is sleeping..." << std::endl;
        ta_sem.acquire();
        mtx.lock();
        if (waiting_students.size()) {
            int st = waiting_students[0];
            waiting_students.erase(waiting_students.begin());
            std::cout << "TA is teaching for student " << st << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(dist(gen)));
            mtx.unlock();
            std::cout << "Teaching finishes for student " << st << std::endl;
            st_sem.release();  
        }
        else
            mtx.unlock();
        
        iter_count++;
        std::cout << iter_count << std::endl;
    }
}

void student(int index) {
    std::this_thread::sleep_for(std::chrono::seconds(dist(gen)));
    mtx.lock();
    if (waiting_students.size() < NUM_CHAIRS) {
        waiting_students.push_back(index);
        std::cout << "Student "<< index << " is waiting" << std::endl;
        mtx.unlock();
        ta_sem.release();
        st_sem.acquire();
    }
    else {
        mtx.unlock();
        std::cout << "Student " << index << " leaves for not having enough chairs in room"<< std::endl;
    }
    
}

int main() {
    std::thread ta(teacher);

    std::vector<std::thread> student_threads;
    for (int i = 1; i < MAX_STUDENTS + 1; ++i) {
        student_threads.emplace_back(student, i);
    }

    for (auto& t : student_threads) {
        t.join();  
    }

}
