import sqlite3
import json
import os
from decimal import Decimal
import datetime

class DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            return float(obj)
        return super(DecimalEncoder, self).default(obj)

# Path to the SQLite database file
# This should match the path used in your C++ application
app_data_path = os.path.expanduser('~/Library/Application Support/QtWebSchoolSys')
db_path = os.path.join(app_data_path, 'school_management.sqlite')

output_file_path = 'db/school_database_cpp_compatible.json'
all_tables_data = {}
tables_to_export = ['students', 'teachers', 'administrators', 'courses', 'grades', 'bots']
db = None

try:
    if not os.path.exists(db_path):
        raise FileNotFoundError(f"Database file not found at {db_path}")

    db = sqlite3.connect(db_path)
    db.row_factory = sqlite3.Row  # Access columns by name
    cursor = db.cursor()

    print("Starting database export to C++ compatible JSON...")
    for table_name in tables_to_export:
        try:
            cursor.execute(f"SELECT * FROM {table_name}")
            records = cursor.fetchall()

            if not records:
                print(f"- No records found in '{table_name}', skipping.")
                all_tables_data[table_name] = []
                continue

            if table_name == 'students':
                transformed_records = []
                for record in records:
                    record_dict = dict(record)
                    # Convert date string back to a Date object
                    birthdate_obj = {}
                    if record_dict.get('birthdate'):
                        try:
                            bdate = datetime.datetime.strptime(record_dict['birthdate'], '%Y-%m-%d').date()
                            birthdate_obj = {"year": bdate.year, "month": bdate.month, "day": bdate.day}
                        except (ValueError, TypeError):
                            birthdate_obj = {} # Handle cases where date is invalid or null

                    # The contact and address info are stored as JSON strings in SQLite
                    contact_info = json.loads(record_dict.get('contact_info', '{}'))
                    address_info = json.loads(record_dict.get('address', '{}'))

                    transformed_record = {
                        "id": record_dict.get('student_id'),
                        "name": record_dict.get('name'),
                        "sex": record_dict.get('sex'),
                        "birthdate": birthdate_obj,
                        "age": record_dict.get('age'),
                        "enrollYear": record_dict.get('enroll_year'),
                        "major": record_dict.get('major'),
                        "class": record_dict.get('class_id'),
                        "contact": {
                            "phone": contact_info.get('phone'),
                            "email": contact_info.get('email')
                        },
                        "address": {
                            "province": address_info.get('province'),
                            "city": address_info.get('city')
                        },
                        "status": record_dict.get('status'),
                        "familyMembers": [],  # This field exists in C++ but not in the DB
                        "password": record_dict.get('password')
                    }
                    transformed_records.append(transformed_record)
                all_tables_data[table_name] = transformed_records
            else:
                # For other tables, just convert Row objects to dictionaries
                all_tables_data[table_name] = [dict(row) for row in records]
            
            print(f"- Exported and transformed {len(records)} records from '{table_name}' table.")

        except sqlite3.Error as table_err:
            # This might happen if a table doesn't exist, which is fine
            print(f"Could not export table '{table_name}': {table_err}")
            all_tables_data[table_name] = []
            continue
    
    try:
        os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
        with open(output_file_path, 'w', encoding='utf-8') as f:
            json.dump(all_tables_data, f, ensure_ascii=False, indent=4, cls=DecimalEncoder)
        print(f"\nDatabase export complete. All data saved to '{output_file_path}'")
    except IOError as e:
        print(f"Error writing to file '{output_file_path}': {e}")

except (sqlite3.Error, FileNotFoundError) as err:
    print(f"An error occurred: {err}")

finally:
    if db:
        db.close()
        print("SQLite connection is closed.")