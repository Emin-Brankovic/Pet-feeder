# Pet-feeder

## Project Description:

The project is designed to simplify and automate feeding proces for pet owners. The goal of the project is to provide a web application that allows users to control the device, communicating over the internet, regardless of their location.

The project consists of three main components:

1. **Control Panel:** A web application (HTML, Bootstrap and JS) accessible from a browser. It allows users to control the feeder remotely.

2. **Execution Side:** This is implemented using a NodeMCU ESP8266 with a SG90 micro servo motor. The servo motor represents the feeder's food hatch, which opens and releases the pet food.

3. **Real-time Database:** A real-time database is created using Google's Firebase service. It enables communication between the control panel and the execution side.

## Functionality:

### Manual mode:

During manual mode, the user can feed his pet using a button on the web application. The web application sends the opening angle to the realtime database, and the executive side receives the data from the database every 3 seconds. After 1.5 seconds the hatch closes and sends the closing angle to the database.

### Automatic mode:

The user can also select automatic mode via the web application where three cards with time pickers are displayed to select the desired feeding time. After the user selects the times, with the set button, they are sent to the database where they are saved. 

Every 3 seconds, the execution side retrieves the times from the database where it checks if the current time, retrieved from the Internet using NTPClient, is equal to the set feeding time. It also checks if the feeding was done during the day, if not and the previous conditions are met, the hatch is opened and the flag variable for that feeding is set to true. 

After the mentioned checks, it is also checked whether the current time is greater than the set feeding time, if so, the flag variable is set to false to ensure re-feeding at the same time the next day.

