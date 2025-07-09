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

enum class Repetition {
    Weekly,
    BiWeeklyOdd,
    BiWeeklyEven
};

struct Time {
    int hour;
    int minute;
    Time(const int h, const int m) : hour(h), minute(m) {}
};

struct TimeSlot {
    DayOfWeek day;
    Time startTime;
    Time endTime;
    Repetition repetition;

    TimeSlot(DayOfWeek d, const Time& start, const Time& end, Repetition rep = Repetition::Weekly)
        : day(d), startTime(start), endTime(end), repetition(rep) {}
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

#ifdef USE_QTJSON

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

inline auto day_of_week_to_qjson_string(DayOfWeek d) -> QString {
    switch (d) {
        case DayOfWeek::Monday: return "Monday";
        case DayOfWeek::Tuesday: return "Tuesday";
        case DayOfWeek::Wednesday: return "Wednesday";
        case DayOfWeek::Thursday: return "Thursday";
        case DayOfWeek::Friday: return "Friday";
        case DayOfWeek::Saturday: return "Saturday";
        case DayOfWeek::Sunday: return "Sunday";
        default: return "Monday";
    }
}

inline auto day_of_week_from_qjson_string(const QString& str) -> DayOfWeek {
    if (str == "Tuesday") return DayOfWeek::Tuesday;
    if (str == "Wednesday") return DayOfWeek::Wednesday;
    if (str == "Thursday") return DayOfWeek::Thursday;
    if (str == "Friday") return DayOfWeek::Friday;
    if (str == "Saturday") return DayOfWeek::Saturday;
    if (str == "Sunday") return DayOfWeek::Sunday;
    return DayOfWeek::Monday;
}

// Repetition
inline auto repetition_to_qjson_string(Repetition r) -> QString {
    switch (r) {
        case Repetition::Weekly: return "Weekly";
        case Repetition::BiWeeklyOdd: return "BiWeeklyOdd";
        case Repetition::BiWeeklyEven: return "BiWeeklyEven";
        default: return "Weekly";
    }
}

inline auto repetition_from_qjson_string(const QString& str) -> Repetition {
    if (str == "BiWeeklyOdd") return Repetition::BiWeeklyOdd;
    if (str == "BiWeeklyEven") return Repetition::BiWeeklyEven;
    return Repetition::Weekly;
}

// Time
inline auto time_to_qjson(const Time& t) -> QJsonObject {
    QJsonObject obj;
    obj["hour"] = t.hour;
    obj["minute"] = t.minute;
    return obj;
}

inline auto time_from_qjson(const QJsonObject& obj) -> Time {
    return Time(obj["hour"].toInt(), obj["minute"].toInt());
}

// TimeSlot
inline auto time_slot_to_qjson(const TimeSlot& ts) -> QJsonObject {
    QJsonObject obj;
    obj["day"] = day_of_week_to_qjson_string(ts.day);
    obj["startTime"] = time_to_qjson(ts.startTime);
    obj["endTime"] = time_to_qjson(ts.endTime);
    obj["repetition"] = repetition_to_qjson_string(ts.repetition);
    return obj;
}

inline auto time_slot_from_qjson(const QJsonObject& obj) -> TimeSlot {
    return TimeSlot(
        day_of_week_from_qjson_string(obj["day"].toString()),
        time_from_qjson(obj["startTime"].toObject()),
        time_from_qjson(obj["endTime"].toObject()),
        repetition_from_qjson_string(obj["repetition"].toString())
    );
}

// Course
inline auto course_to_qjson(const Course& c) -> QJsonObject {
    QJsonObject obj;
    obj["courseID"] = c.get_course_id();
    obj["courseName"] = QString::fromStdString(c.get_course_name());
    obj["instructor"] = QString::fromStdString(c.get_instructor());
    obj["location"] = QString::fromStdString(c.get_location());
    obj["credits"] = c.get_credits();
    QJsonArray scheduleArray;
    for (const auto& ts : c.get_schedule()) {
        scheduleArray.append(time_slot_to_qjson(ts));
    }
    obj["schedule"] = scheduleArray;
    return obj;
}

inline auto course_from_qjson(const QJsonObject& obj) -> Course {
    Course c(
        obj["courseID"].toInt(),
        obj["courseName"].toString().toStdString(),
        obj["instructor"].toString().toStdString(),
        obj["location"].toString().toStdString(),
        obj["credits"].toInt()
    );
    QJsonArray scheduleArray = obj["schedule"].toArray();
    std::vector<TimeSlot> schedule;
    for (const auto& val : scheduleArray) {
        schedule.push_back(time_slot_from_qjson(val.toObject()));
    }
    c.set_schedule(schedule);
    return c;
}

#endif
