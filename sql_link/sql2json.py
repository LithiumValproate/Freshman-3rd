import mysql.connector as mysql
import json
import os
from decimal import Decimal

class DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            return float(obj)
        return super(DecimalEncoder, self).default(obj)

db_config = {
    'host': 'localhost',
    'user': 'kasugano',
    'password': os.getenv('PASSWORD'),
    'database': 'school_management',
}

output_file_path = 'db/school_database_cpp_compatible.json'
all_tables_data = {}
tables_to_export = ['students', 'teachers', 'administrators', 'courses', 'grades', 'bots']
db = None

try:
    db = mysql.connect(**db_config)
    cursor = db.cursor(dictionary=True)

    print("Starting database export to C++ compatible JSON...")
    for table_name in tables_to_export:
        try:
            cursor.execute(f"SELECT * FROM {table_name}")
            records = cursor.fetchall()

            if table_name == 'students':
                transformed_records = []
                for record in records:
                    # Convert date string back to a Date object
                    birthdate_obj = {}
                    if record.get('birthdate'):
                        bdate = record['birthdate']
                        birthdate_obj = {"year": bdate.year, "month": bdate.month, "day": bdate.day}

                    transformed_record = {
                        "id": record.get('id'),
                        "name": record.get('name'),
                        "sex": record.get('sex'),
                        "birthdate": birthdate_obj,
                        "age": record.get('age'),
                        "enrollYear": record.get('enrollYear'),
                        "major": record.get('major'),
                        "class": record.get('class'),
                        "contact": {
                            "phone": record.get('phone'),
                            "email": record.get('email')
                        },
                        "address": {
                            "province": record.get('province'),
                            "city": record.get('city')
                        },
                        "status": record.get('status'),
                        "familyMembers": [],  # This field exists in C++ but not in the DB
                        "password": record.get('password')
                    }
                    transformed_records.append(transformed_record)
                all_tables_data[table_name] = transformed_records
            else:
                all_tables_data[table_name] = records
            
            print(f"- Exported and transformed {len(records)} records from '{table_name}' table.")

        except mysql.Error as table_err:
            print(f"Could not export table '{table_name}': {table_err}")
            continue
    
    try:
        with open(output_file_path, 'w', encoding='utf-8') as f:
            json.dump(all_tables_data, f, ensure_ascii=False, indent=4, cls=DecimalEncoder)
        print(f"\nDatabase export complete. All data saved to '{output_file_path}'")
    except IOError as e:
        print(f"Error writing to file '{output_file_path}': {e}")

except mysql.Error as err:
    print(f"Database Connection Error: {err}")

finally:
    if db and db.is_connected():
        cursor.close()
        db.close()
        print("MySQL connection is closed.")