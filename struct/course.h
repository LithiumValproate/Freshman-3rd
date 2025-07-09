#pragma once

#include <string>
#include <vector>

enum class DayOfWeek {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

struct TimeSlot {
    DayOfWeek day;
    std::string startTime; // Format: HH:MM
    std::string endTime;   // Format: HH:MM
};

class Course {
    int courseID;
    std::string courseName;
    std::string instructor;
    std::string location;
    int credits;
    std::vector<TimeSlot> schedule;

public:
    Course() : courseID(0), credits(0) {}

    Course(int id, const std::string& name, const std::string& instr, const std::string& loc, int creds)
        : courseID(id), courseName(name), instructor(instr), location(loc), credits(creds) {}

    auto get_course_id() const -> int { return courseID; }
    void set_course_id(int id) { courseID = id; }

    auto get_course_name() const -> const std::string& { return courseName; }
    void set_course_name(const std::string& name) { courseName = name; }

    auto get_instructor() const -> const std::string& { return instructor; }
    void set_instructor(const std::string& instr) { instructor = instr; }

    auto get_location() const -> const std::string& { return location; }
    void set_location(const std::string& loc) { location = loc; }

    auto get_credits() const -> int { return credits; }
    void set_credits(int creds) { credits = creds; }

    auto get_schedule() const -> const std::vector<TimeSlot>& { return schedule; }
    void set_schedule(const std::vector<TimeSlot>& newSchedule) { schedule = newSchedule; }

    void add_time_slot(const TimeSlot& slot) {
        schedule.push_back(slot);
    }
};
