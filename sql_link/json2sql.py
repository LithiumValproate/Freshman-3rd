import sqlite3
import json
import os
from datetime import datetime

# This script now assumes the input JSON file is structured to be compatible
# with the C++ structs in `student.h`.

# Path to the SQLite database file
app_data_path = os.path.expanduser('~/Library/Application Support/QtWebSchoolSys')
db_path = os.path.join(app_data_path, 'school_management.sqlite')

# This is the source file, expected to be in the C++-compatible format.
file_path = os.path.join(app_data_path, 'student_data.json')

try:
    os.makedirs(app_data_path, exist_ok=True)
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
except FileNotFoundError:
    print(f"Error: The file was not found at {file_path}")
    exit()
except json.JSONDecodeError:
    print(f"Error: Could not decode JSON from the file {file_path}")
    exit()

db = None
try:
    db = sqlite3.connect(db_path)
    cursor = db.cursor()

    # --- Drop and Recreate Tables with SQLite Compatible Schema ---
    # The new `students` schema matches the C++ struct.
    table_creation_query = """
    -- Drop tables that depend on students or teachers to avoid foreign key errors
    DROP TABLE IF EXISTS grades;
    DROP TABLE IF EXISTS students;
    DROP TABLE IF EXISTS courses;
    DROP TABLE IF EXISTS teachers;
    DROP TABLE IF EXISTS administrators;
    DROP TABLE IF EXISTS bots;

    CREATE TABLE students (
        student_id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        sex TEXT,
        birthdate TEXT,
        age INTEGER,
        enroll_year INTEGER,
        major TEXT,
        class_id INTEGER,
        contact_info TEXT, -- Storing as JSON string
        address TEXT,      -- Storing as JSON string
        status TEXT,
        password TEXT
    );

    CREATE TABLE teachers (
        teacher_id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        password TEXT NOT NULL
    );

    CREATE TABLE administrators (
        admin_id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL UNIQUE,
        password TEXT NOT NULL
    );

    CREATE TABLE courses (
        course_id INTEGER PRIMARY KEY AUTOINCREMENT,
        course_name TEXT NOT NULL UNIQUE,
        credit REAL NOT NULL CHECK (credit > 0),
        teacher_id INTEGER NOT NULL,
        FOREIGN KEY (teacher_id) REFERENCES teachers(teacher_id) ON DELETE CASCADE
    );

    CREATE TABLE grades (
        grade_id INTEGER PRIMARY KEY AUTOINCREMENT,
        student_id INTEGER NOT NULL,
        course_id INTEGER NOT NULL,
        score REAL CHECK (score BETWEEN 0 AND 100),
        term TEXT NOT NULL,
        FOREIGN KEY (student_id) REFERENCES students(student_id) ON DELETE CASCADE,
        FOREIGN KEY (course_id) REFERENCES courses(course_id) ON DELETE CASCADE,
        UNIQUE (student_id, course_id, term)
    );

    CREATE TABLE bots (
        user_id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL UNIQUE,
        password TEXT NOT NULL
    );
    """

    cursor.executescript(table_creation_query)
    print("Database and tables recreated with SQLite compatible schema.")

    # --- Insert Data into the New `students` Table ---
    if isinstance(data, list):
        # Use SQLite's ON CONFLICT clause for "upsert" behavior
        insert_query = """
        INSERT INTO students (
            student_id, name, sex, birthdate, age, enroll_year, major, class_id,
            contact_info, address, status, password
        )
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ON CONFLICT(student_id) DO UPDATE SET
            name=excluded.name, sex=excluded.sex, birthdate=excluded.birthdate, age=excluded.age,
            enroll_year=excluded.enroll_year, major=excluded.major, class_id=excluded.class_id,
            contact_info=excluded.contact_info, address=excluded.address, status=excluded.status,
            password=excluded.password;
        """

        insert_count = 0
        for student in data:
            try:
                bdate = student.get('birthdate', {})
                birthdate_str = f"{bdate.get('year', 2000)}-{bdate.get('month', 1)}-{bdate.get('day', 1)}"

                # Calculate age based on birthdate
                birth_date_obj = datetime.strptime(birthdate_str, "%Y-%m-%d")
                today = datetime.today()
                age = today.year - birth_date_obj.year - ((today.month, today.day) < (birth_date_obj.month, birth_date_obj.day))

                contact_json = json.dumps(student.get('contact', {}))
                address_json = json.dumps(student.get('address', {}))

                student_tuple = (
                    student.get('id'),
                    student.get('name'),
                    student.get('sex'),
                    birthdate_str,
                    age,
                    student.get('enrollYear'),
                    student.get('major'),
                    student.get('class'),
                    contact_json,
                    address_json,
                    student.get('status'),
                    student.get('password', 'default_password')
                )

                # Check that first 8 required fields are not None
                if all(student_tuple[i] is not None for i in [0, 1, 2, 3, 4, 5, 6, 7]):
                    cursor.execute(insert_query, student_tuple)
                    insert_count += 1
                else:
                    print(f"Skipping record due to missing essential data: {student}")

            except Exception as e:
                print(f"Skipping record due to an error: {e}. Record: {student}")

        db.commit()
        print(f"Successfully inserted or updated {insert_count} student records.")

    else:
        # Handle if the top-level JSON is a dict (like from the other script)
        if isinstance(data.get('students'), list):
             print("Found 'students' key, but this script expects a list of students at the root.")
        else:
            print("Warning: JSON data is not a list. No data was inserted.")


except sqlite3.Error as err:
    print(f"Database Error: {err}")

finally:
    if db:
        db.close()
        print("SQLite connection is closed.")

