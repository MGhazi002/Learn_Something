import cx_Oracle
import smtplib
from email.mime.text import MIMEText

# Configuration variables (modify as needed)
db_user = "your_username"
db_password = "your_password"
db_connect_string = "your_database_connect_string"
email_sender = "your_email_address"
email_recipients = ["recipient1@example.com", "recipient2@example.com"]

# Thresholds (easily modifiable)
tablespace_threshold = 95
archive_threshold = 80
long_running_threshold = 60 * 5  # 5 minutes in seconds
asm_threshold = 80

# Database connection
try:
    connection = cx_Oracle.connect(db_user, db_password, db_connect_string)
except cx_Oracle.Error as error:
    print("Database connection error:", error)
    exit(1)

# Functions for checking various alerts
def check_tablespace_alerts(cursor):
    cursor.execute("""
        SELECT tablespace_name,
               round(used_percent, 2) AS used_percent
          FROM dba_tablespace_usage_metrics
         WHERE used_percent >= :threshold
    """, threshold=tablespace_threshold)
    return cursor.fetchall()

def check_archive_space_alerts(cursor):
    cursor.execute("""
        SELECT destination,
               round(space_used / space_limit * 100, 2) AS used_percent
          FROM v$archive_dest
         WHERE space_used / space_limit * 100 >= :threshold
    """, threshold=archive_threshold)
    return cursor.fetchall()

# ... (implement other check functions)

# Helper functions for formatting and sending emails
def format_alert_message(alerts):
    message = "Oracle Database Alerts:\n\n"
    for alert_type, alert_data in alerts.items():
        message += f"{alert_type}:\n"
        for row in alert_data:
            message += f"- {', '.join(str(value) for value in row)}\n"
        message += "\n"
    return message

def send_email_alert(message):
    msg = MIMEText(message)
    msg['Subject'] = "Oracle Database Alert"
    msg['From'] = email_sender
    msg['To'] = ", ".join(email_recipients)

    with smtplib.SMTP("your_smtp_server", your_smtp_port) as server:
        server.starttls()
        server.login(email_sender, "your_email_password")
        server.sendmail(email_sender, email_recipients, msg.as_string())

# Main logic for checking alerts and sending emails
cursor = connection.cursor()

alerts = {}

# Call each alert check function and store results in alerts dictionary
alerts["Tablespace Alerts"] = check_tablespace_alerts(cursor)
alerts["Archive Space Alerts"] = check_archive_space_alerts(cursor)
# ... (call other alert check functions)

if any(alerts.values()):
    message = format_alert_message(alerts)
    send_email_alert(message)

cursor.close()
connection.close()
