import mysql.connector as mysql
import json
import os

db_config = {
    'host': 'localhost',
    'user': 'kasugano',
    'password': os.getenv('PASSWORD'),
    'database': 'school_management',
}

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

    create_table_query = f"""
    CREATE DATABASE IF NOT EXISTS {db_config['database']};
    CREATE TABLE students (
        student_id INT PRIMARY KEY AUTO_INCREMENT,
        name VARCHAR(50) NOT NULL,
        sex ENUM('男', '女') NOT NULL,
        birthdate DATE NOT NULL,
        age INT NOT NULL CHECK (age >= 0),
        enroll_year INT NOT NULL CHECK (enroll_year >= 1900 AND enroll_year <= YEAR(CURDATE())),
        major VARCHAR(50) NOT NULL,
        class_id INT NOT NULL,
        contact_info SET('手机', '邮箱') NOT NULL,
        address SET('省', '市') NOT NULL,
        status ENUM('在读', '休学', '毕业') NOT NULL DEFAULT '在读',
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
        course_name VARCHAR(100) NOT NULL UNIQUE,  -- 如 "数据库原理"
        credit DECIMAL(2,1) NOT NULL CHECK (credit > 0),
        teacher_id INT NOT NULL,
        FOREIGN KEY (teacher_id) REFERENCES teachers(teacher_id)
    );
    CREATE TABLE grades (
        grade_id INT PRIMARY KEY AUTO_INCREMENT,
        student_id INT NOT NULL,
        course_id INT NOT NULL,
        score DECIMAL(5,2) CHECK (score BETWEEN 0 AND 100),
        term VARCHAR(20) NOT NULL,  -- 如 "2023-2024学年第一学期"
        FOREIGN KEY (student_id) REFERENCES students(student_id),
        FOREIGN KEY (course_id) REFERENCES courses(course_id),
        UNIQUE KEY (student_id, course_id, term)  -- 防止重复录入
    );
    CREATE TABLE bots (
        user_id INT PRIMARY KEY AUTO_INCREMENT,
        username VARCHAR(50) NOT NULL UNIQUE,
        password VARCHAR(255) NOT NULL
    )"""
    # The mysql-connector can't execute multiple statements at once by default.
    # We split the DDL string by ';' and execute each statement.
    for statement in create_table_query.split(';'):
        # Ensure we don't execute empty strings
        if statement.strip():
            cursor.execute(statement)
    print("Database and tables created or already exist.")

    # --- Insert Data into Table ---
    # This assumes your JSON is a list of dictionaries.
    # Adjust the keys ('student_id', 'name', etc.) to match your JSON data.
    if isinstance(data, list):
        table_name = "students"
        # The INSERT query must match the table's NOT NULL columns.
        # NOTE: ON DUPLICATE KEY UPDATE requires a UNIQUE index on the columns being
        # inserted (e.g., name, major, class_id) to function as intended.
        # The current schema does not define one, so this may act as a simple INSERT.
        insert_query = f"""
        INSERT INTO {table_name} (
            name, sex, birthdate, age, enroll_year, major, class_id, 
            contact_info, address, status, password
        )
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
        ON DUPLICATE KEY UPDATE 
            sex=VALUES(sex), birthdate=VALUES(birthdate), age=VALUES(age), 
            enroll_year=VALUES(enroll_year), contact_info=VALUES(contact_info), 
            address=VALUES(address), status=VALUES(status), password=VALUES(password);
        """

        insert_count = 0
        for student in data:
            # Extract data for all required fields, providing defaults for NOT NULL columns
            # to prevent insertion errors. You may need to adjust these defaults.
            try:
                # The SET type in MySQL expects a comma-separated string.
                contact_info_str = ','.join(student.get('contact_info', ['手机']))
                address_str = ','.join(student.get('address', ['省']))

                student_tuple = (
                    student.get('name'),
                    student.get('sex', '男'),
                    student.get('birthdate', '2000-01-01'),
                    student.get('age', 20),
                    student.get('enroll_year', 2023),
                    student.get('major'),
                    student.get('class_id') or student.get('class'),
                    contact_info_str,
                    address_str,
                    student.get('status', '在读'),
                    student.get('password', 'default_password_please_change') # It's insecure to have a default password
                )

                # Validate that essential fields are present before inserting
                if student.get('name') and student.get('major') and (student.get('class_id') or student.get('class')):
                    cursor.execute(insert_query, student_tuple)
                    insert_count += 1
                else:
                    print(f"Skipping record due to missing name, major, or class_id: {student}")

            except Exception as e:
                print(f"Skipping record due to an error: {e}. Record: {student}")

        db.commit()
        print(f"Successfully inserted or updated {insert_count} records.")

    else:
        print("Warning: JSON data is not a list. No data was inserted.")


except mysql.Error as err:
    print(f"Database Error: {err}")

finally:
    # --- Close Connection ---
    if 'db' in locals() and db.is_connected():
        cursor.close()
        db.close()
        print("MySQL connection is closed.")