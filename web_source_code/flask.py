from flask import Flask, render_template, request, jsonify
import matplotlib.pyplot as plt
import json
import os
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
from datetime import datetime
from PIL import Image
from zoneinfo import ZoneInfo
import csv


local_tz = ZoneInfo('Europe/Brussels')


app = Flask(__name__)
temperature_data_file = "temperature_data.json"

monitoring_active  = False
start_time = datetime.now(local_tz)
cook_error = False
error_message=""
target_temperature = 0
measured_temperature = 0
actual_phase = 0
number_of_phase = 0
profil_duration = 0
set_point_is_reached = False

# Table variables
profil_name = ""
T1_temp = None
T1_duration = None
nR1 = None
T2_temp = None
T2_duration = None
nR2 = None
T3_temp = None
T3_duration = None
nR3 = None


current_date = datetime.now().strftime("%Y-%m-%d")
csv_file = f"temperature_data_{current_date}.csv"

time_in_hours = None

# timeout timer
inactivity_timer = None

# Load temperature data if file exists
if os.path.exists(temperature_data_file):
    with open(temperature_data_file, 'r') as f:
        temperature_data = json.load(f)
else:
    temperature_data = []

def save_data():
    with open(temperature_data_file, 'w') as f:
        json.dump(temperature_data, f)

def send_email(image_path):
    sender = 'four.aeriane@gmail.com'
    recipients = ["samsonheritafita@gmail.com", "info@aeriane.com", "lamon@aeriane.com", "morize@aeriane.com"]  # List of recipients
    subject = 'Success four' if not cook_error else 'Error four'
    msg = MIMEMultipart()
    msg['From'] = sender
    msg['To'] = ", ".join(recipients)
    msg['Subject'] = subject

     # Add body text

    # Define the email body based on cook_error
    if not cook_error:
        body = "The cooking process completed successfully. Please find the results attached."
    else:
        body = (
            "There was an error during the cooking process.\n\n"
            f"Error Details: {error_message}\n\n"
            "Please find the attached file for more information."
        )

    msg.attach(MIMEText(body, 'plain'))


     # Attach the image file
    with open(image_path, "rb") as attachment:
        part = MIMEBase('application', 'octet-stream')
        part.set_payload(attachment.read())
    encoders.encode_base64(part)
    part.add_header(
        "Content-Disposition",
        f"attachment; filename={image_path}",
    )
    msg.attach(part)

    #attach the csv file
    with open(csv_file, "rb") as attachment:
        part = MIMEBase('application', 'octet-stream')
        part.set_payload(attachment.read())
    encoders.encode_base64(part)
    part.add_header(
        "Content-Disposition",
        f"attachment; filename={csv_file}",
    )
    msg.attach(part)

    # Send the email
    with smtplib.SMTP('smtp.gmail.com', 587) as server:
        server.starttls()
        server.login(sender, 'wags tqzi gaki pryt')  #app password
        server.sendmail(sender, recipients, msg.as_string())

def generate_csv(time, temperature):
    global csv_file
    with open(csv_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Time (hours)", "Temperature (°C)"])
        for time, temp in zip(time, temperature):
            writer.writerow([time, temp])
    print(f"CSV file {csv_file} created successfully.")

def generate_final_plot_and_send():
    global time_in_hours
    # Capture end time and clear figure
    end_time = datetime.now(local_tz)
    plt.clf()
    fig, ax = plt.subplots(figsize=(12, 8))  # Create a larger figure for better layout

    # Generate the temperature plot
    time_in_hours = [i / 60 for i in range(len(temperature_data))]  # Convert minutes to hours
    ax.plot(time_in_hours, temperature_data, label='', color='blue')
    ax.set_xlabel('Time [h]')
    ax.set_ylabel('Temperature (°C)')
    ax.legend()

    #save csv
    generate_csv(time_in_hours, temperature_data)

    # Prepare the table data
    table_data = [
        ["Profile", "T1[°C]", "t1[h]", "nR1", "T2[°C]", "t2[h]", "nR2", "T3[°C]", "t3[h]", "nR3"],
        [profil_name or "N/A", T1_temp or "-", T1_duration or "-", nR1 or "-",
         T2_temp or "-", T2_duration or "-", nR2 or "-", T3_temp or "-", T3_duration or "-", nR3 or "-"]
    ]

    # Add the table above the plot
    table = plt.table(
        cellText=table_data,
        colLabels=None,  # Headers are already included in the first row
        cellLoc='center',
        loc='top',
        colWidths=[0.1] * len(table_data[0])  # Adjust column widths
    )

    # Format the table
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 1.2)  # Adjust table size for better fit

    # Adjust plot layout to make space for the table and additional elements
    plt.subplots_adjust(top=0.86)  # Reserve space for the table and logo

    # Add the title above the plot
    plt.suptitle('Cooking Regulation', fontsize=16)

    # Add the logo in the upper-right corner
    try:
        logo = Image.open('Logo_aériane.jpg')  # Change path as needed
        logo = logo.resize((70, 70))  # Resize logo
        if logo.mode != 'RGBA':
            logo = logo.convert('RGBA')

        fig.figimage(logo, xo=fig.bbox.xmax - 80, yo=fig.bbox.ymax - 80, alpha=1, zorder=10)
    except Exception as e:
        print(f"Error loading logo: {e}")

    # Add additional information as a text box

    Error_occured = 'None' if not cook_error else error_message
    time_difference = end_time - start_time
    hours = time_difference.seconds // 3600
    minutes = (time_difference.seconds % 3600) // 60
    delay_str = f"{hours}h {minutes}m"

    start_str = start_time.strftime("%Y-%m-%d %H:%M")
    end_str = end_time.strftime("%Y-%m-%d %H:%M")

    plt.gcf().text(
        0.13, 0.85,  # Adjust position for better visibility
        f"Error: {Error_occured}\nStart: {start_str}\nEnd: {end_str}\nDuration: {delay_str}",
        transform=plt.gcf().transFigure,
        ha="left",
        va="top",
        fontsize=9,
        color="grey",
        bbox=dict(facecolor='white', alpha=0.8, edgecolor='gray')
    )

    # Save and send the plot image
    try:
        image_path = f'temperature_plot_{current_date}.png'
        plt.savefig(image_path)
        print(f"Plot saved at {image_path}")
    except Exception as e:
        print(f"Error saving plot: {e}")

    # Send email and remove the file
    try:
        send_email(image_path)
        if os.path.exists(image_path):
            os.remove(image_path)
            print(f"File {image_path} deleted.")
    except FileNotFoundError:
        print(f"File not found: {image_path}")
    except Exception as e:
        print(f"Error during email sending or file deletion: {e}")

    global monitoring_active
    monitoring_active = False

# Function to call after inactivity
def reportInactivityTimeout():
    print("No activity detected for 5 minutes. Executing report...")
    global cook_error, error_message
    cook_error = True
    error_message = "No activity detected from the oven for 5 minutes"  # Default to empty string
    generate_final_plot_and_send()





@app.route('/')
def index():
    return render_template('index.html')

@app.route('/start_monitoring', methods=['POST'])
def start_monitoring():
    global monitoring_active, start_time
    #empty temperature data
    temperature_data.clear()
    save_data()
    #empty csv file
    with open(csv_file, 'w') as file:
            file.truncate()
    monitoring_active = True
    start_time = datetime.now(local_tz) # Record the start time
    print(f"Monitoring started at: {start_time}")
    return jsonify(success=True, message="Monitoring started")

@app.route('/update_profile_data', methods=['POST'])
def update_profile_data():
    data = request.get_json()  # Parse JSON payload
    if data is None:
        return {"error": "Invalid JSON payload"}, 400

    global profil_name, target_temperature, measured_temperature, actual_phase, number_of_phase, profil_duration, set_point_is_reached
    global T1_temp, T1_duration, nR1, T2_temp, T2_duration, nR2, T3_temp, T3_duration, nR3

    # Update profile variables based on received JSON data
    profil_name = data.get('profil_name', profil_name)
    target_temperature = data.get('target_temperature', target_temperature)
    measured_temperature = data.get('measured_temperature', measured_temperature)
    actual_phase = data.get('actual_phase', actual_phase)
    number_of_phase = data.get('number_of_phase', number_of_phase)
    profil_duration = data.get('profil_duration', profil_duration)
    set_point_is_reached = data.get('set_point_is_reached', set_point_is_reached)

    # Update table variables (assuming you receive these in the request)
    T1_temp = data.get('T1_temp', T1_temp)
    T1_duration = data.get('T1_duration', T1_duration)
    nR1 = data.get('nR1', nR1)
    T2_temp = data.get('T2_temp', T2_temp)
    T2_duration = data.get('T2_duration', T2_duration)
    nR2 = data.get('nR2', nR2)
    T3_temp = data.get('T3_temp', T3_temp)
    T3_duration = data.get('T3_duration', T3_duration)
    nR3 = data.get('nR3', nR3)


    # Update temperature for chart emailing
    temperature_data.append(measured_temperature)
    save_data()

    # Update temperature for chart monitoring
    global time_in_hours
    time_in_hours = [round(i / 60, 2) for i in range(len(temperature_data))]



    # global inactivity_timer
    # # Reset the inactivity timer
    # if inactivity_timer is not None and monitoring_active:
    #     inactivity_timer.cancel()  # Cancel the previous timer

    # Start a new timer for 5 minutes
   # inactivity_timer = Timer(300, reportInactivityTimeout)
   # inactivity_timer.start()


    return {"message": "Profile data updated successfully"}, 200


@app.route('/update_monitored_profil_data', methods=['POST'])
def update_web_display_profil_data():
    # Prepare the data to send back to the front-end, including table variables
    data = {
        "start_time": start_time.isoformat() if start_time and monitoring_active else None,
        "is_active": monitoring_active,
        "target_temperature": target_temperature,
        "measured_temperature": measured_temperature,
        "actual_phase": actual_phase,
        "number_of_phase": number_of_phase,
        "profil_duration": profil_duration,
        "set_point_is_reached": set_point_is_reached,
        # Table variables to update the table in the front-end
        "profil_name": profil_name,
        "T1_temp": T1_temp,
        "T1_duration": T1_duration,
        "nR1": nR1,
        "T2_temp": T2_temp,
        "T2_duration": T2_duration,
        "nR2": nR2,
        "T3_temp": T3_temp,
        "T3_duration": T3_duration,
        "nR3": nR3,
        # Chart data for front-end
        "time_in_hours": time_in_hours,
        "temperature_data": temperature_data
    }

    # Print the JSON data to the console for debugging
    #print(data)
    print("Data sent successfuly for web monitoring")

    # Return the JSON response with all the updated variables
    return jsonify(data)

@app.route('/send_email', methods=['POST'])
def send_email_route():
    global cook_error, error_message
    cook_error = request.json.get('cook_error', False)  # Default to False
    error_message = request.json.get('error_message', "")  # Default to empty string
    generate_final_plot_and_send()
    return jsonify(success=True, message="Email sent successfully")

@app.route('/data')
def data():
    return jsonify(temperature_data)

if __name__ == '__main__':
    app.run(debug=True)

