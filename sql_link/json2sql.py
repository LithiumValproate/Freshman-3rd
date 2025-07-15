import mysql.connector as mysql
import json
import os
from datetime import datetime

# This script now assumes the input JSON file is structured to be compatible
# with the C++ structs in `student.h`.

db_config = {
    'host': 'localhost',
    'user': 'kasugano',
    'password': os.getenv('PASSWORD'),
    'database': 'school_management',
}

# This is the source file, expected to be in the C++-compatible format.
file_path = '../../Library/Application Support/Qt学习项目/Qt Web学生管理系统/student_data.json'

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
except FileNotFoundError:
    print(f"Error: The file was not found at {file_path}")
    exit()
except json.JSONDecodeError:
    print(f"Error: Could not decode JSON from the file {file_path}")
    exit()

try:
    db = mysql.connect(**db_config)
    cursor = db.cursor()

    # --- Drop and Recreate Tables with New Schema ---
    # The new `students` schema matches the C++ struct.
    # We must drop dependent tables (`grades`) first before modifying `students`.
    # Other tables are created with IF NOT EXISTS to preserve their data.
    table_creation_query = f"""
    CREATE DATABASE IF NOT EXISTS {db_config['database']};
    USE {db_config['database']};

    -- Drop tables that depend on students or teachers to avoid foreign key errors
    DROP TABLE IF EXISTS grades;
    DROP TABLE IF EXISTS students;
    DROP TABLE IF EXISTS courses;
    DROP TABLE IF EXISTS teachers;

    CREATE TABLE students (
        id BIGINT PRIMARY KEY,
        name VARCHAR(50) NOT NULL,
        sex VARCHAR(10) NOT NULL,
        birthdate DATE NOT NULL,
        age INT NOT NULL,
        enrollYear INT NOT NULL,
        major VARCHAR(50) NOT NULL,
        class INT NOT NULL,
        phone VARCHAR(50),
        email VARCHAR(100),
        province VARCHAR(50),
        city VARCHAR(50),
        status VARCHAR(20) NOT NULL,
        password VARCHAR(255) NOT NULL
    );

    CREATE TABLE teachers (
        teacher_id INT PRIMARY KEY AUTO_INCREMENT,
        name VARCHAR(50) NOT NULL,
        password VARCHAR(255) NOT NULL
    );

    CREATE TABLE administrators (
        admin_id INT PRIMARY KEY AUTO_INCREMENT,
        username VARCHAR(50) NOT NULL UNIQUE,
        password VARCHAR(255) NOT NULL
    );

    CREATE TABLE courses (
        course_id INT PRIMARY KEY AUTO_INCREMENT,
        course_name VARCHAR(100) NOT NULL UNIQUE,
        credit DECIMAL(2,1) NOT NULL CHECK (credit > 0),
        teacher_id INT NOT NULL,
        FOREIGN KEY (teacher_id) REFERENCES teachers(teacher_id) ON DELETE CASCADE
    );

    CREATE TABLE grades (
        grade_id INT PRIMARY KEY AUTO_INCREMENT,
        student_id BIGINT NOT NULL,
        course_id INT NOT NULL,
        score DECIMAL(5,2) CHECK (score BETWEEN 0 AND 100),
        term VARCHAR(20) NOT NULL,
        FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
        FOREIGN KEY (course_id) REFERENCES courses(course_id) ON DELETE CASCADE,
        UNIQUE KEY (student_id, course_id, term)
    );

    CREATE TABLE bots (
        user_id INT PRIMARY KEY AUTO_INCREMENT,
        username VARCHAR(50) NOT NULL UNIQUE,
        password VARCHAR(255) NOT NULL
    );
    """

    for statement in table_creation_query.split(';'):
        if statement.strip():
            cursor.execute(statement)
    print("Database and tables recreated with C++ compatible schema.")

    # --- Insert Data into the New `students` Table ---
    if isinstance(data, list):
        insert_query = """
        INSERT INTO students (
            id, name, sex, birthdate, age, enrollYear, major, class,
            phone, email, province, city, status, password
        )
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
        ON DUPLICATE KEY UPDATE
            name=VALUES(name), sex=VALUES(sex), birthdate=VALUES(birthdate), age=VALUES(age),
            enrollYear=VALUES(enrollYear), major=VALUES(major), class=VALUES(class),
            phone=VALUES(phone), email=VALUES(email), province=VALUES(province),
            city=VALUES(city), status=VALUES(status), password=VALUES(password);
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

                contact = student.get('contact', {})
                address = student.get('address', {})

                student_tuple = (
                    student.get('id'),
                    student.get('name'),
                    student.get('sex'), # "Male" or "Female"
                    birthdate_str,
                    age,
                    student.get('enrollYear'),
                    student.get('major'),
                    student.get('class'),
                    contact.get('phone'),
                    contact.get('email'),
                    address.get('province'),
                    address.get('city'),
                    student.get('status'), # "Active", "Leave", etc.
                    student.get('password', 'default_password')
                )

                if all(student_tuple[:8]): # Check that first 8 required fields are not None
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


except mysql.Error as err:
    print(f"Database Error: {err}")

finally:
    if 'db' in locals() and db.is_connected():
        cursor.close()
        db.close()
        print("MySQL connection is closed.")
