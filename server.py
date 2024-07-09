# works with both python 2 and 3
from __future__ import print_function
#my code
from flask import Flask, request
import threading 
import paho.mqtt.client as mqtt
import requests
import json
from mqtt import start_mqtt_subscriber
app = Flask(__name__)
import os




import africastalking


# current_temperature = ''

@app.route('/call', methods=['POST', 'GET'])
def making_a_call():
    global data
    data = request.form.get('data', '') 
    try:
        VOICE().call()
        return "<p>Call has been made!</p>"
    except:
        return "<p>Error making call!</p>"
    

    
@app.route('/', methods=['POST'])
def call_back_endpoint():
    # Read in a couple of POST variables passed in with the request
    session_id = request.form.get('sessionId')
    is_active = request.form.get('isActive')

    if is_active == '1':
        # Read in the caller's number. The format will contain the + in the beginning
        caller_number = request.form.get('callerNumber')

      

        text = "Hello, my name is Pendo from air monitoring station. I want to inform you that the air levels in your region are past the threshold. Kindly evacuate or come up with a solution immediately, Thank you. Be safe."
        # text = f"Sensor reading is {current_temperature}"
        # if current_temperature:
        #     text = f"Sensor reading is {current_temperature}"
        # else:
        #     text = f"There is no reading"
            
        # Compose the response
        response = f'''<?xml version="1.0" encoding="UTF-8"?>
                      <Response>
                          <Say>{text}</Say>
                      </Response>'''

        # Print the response onto the page so that our gateway can read it
        return response, 200, {'Content-Type': 'text/xml'}
    else:
        # Read in call details (duration, cost). This flag is set once the call is completed.
        # Note that the gateway does not expect a response in this case
        duration = request.form.get('durationInSeconds')
        currency_code = request.form.get('currencyCode')
        amount = request.form.get('amount')

        # You can then store this information in the database for your records
        return '', 204

class VOICE:
    
    def __init__(self):
		# Set your app credentials
        self.username = "USERNAME"
        self.api_key = "API KEY"
		# Initialize the SDK
        africastalking.initialize(self.username, self.api_key)
		# Get the voice service
        self.voice = africastalking.Voice

    def call(self):
        # Set your Africa's Talking phone number in international format
        callFrom = "CALL FROM"
        # Set the numbers you want to call to in a comma-separated list
        callTo   = ["CALL NUMBER"]
        try:
			# Make the call
            result = self.voice.call(callFrom, callTo)
            print (result)

            
        
         
            
        except Exception as e:
            print ("Encountered an error while making the call:%s" %str(e))

if __name__ == '__main__':
    mqtt_thread = threading.Thread(target=start_mqtt_subscriber)
 
    mqtt_thread.daemon = True
    # Daemonize the thread so it exits when the main thread exits
    mqtt_thread.start()
    app.run(host="0.0.0.0", port=os.environ.get('PORT'))
