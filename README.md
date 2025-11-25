# Smart-Al-Test (IoT project) 

## Brief Project Overview

Smart Al-Test is a handheld IoT breathalyzer using an MQ-3 sensor and FireBeetle ESP32-C6 to process readings and display them on an OLED screen. Data is sent via Bluetooth Low Energy to a Vercel web UI and stored in a Supabase backend.

_For the more project details see sections below._'

### Jira Task Breakdown and Difficulty

Project tasks are managed in Jira, each with a difficulty rating to help estimate effort and plan timelines.

Scale: 1 = easy, 3 = medium, 5 = hard

You can edit a task Points by opening it and selecting the edit icon in the details panel.

### Conventional Commits

In this project, **Conventional Commits** are used, which is a simple set of rules to structure and create an explicit commit history that is easy to read both for humans and machines if needed.

Learn more about describing the features, fixes, and breaking changes made in commit messages here: [Click Me](https://www.conventionalcommits.org/en/v1.0.0/#specification)

#### The commit message should be structured as follows:
```<type>(scope): <description>```

#### The commit contains the following structural elements:

- **fix**: a commit of the ```type``` ___fix___ patches a bug in your codebase.

- **feat**: a commit of the ```type``` ___feat___ introduces a new feature to the codebase.

- **!**: a commit that appends a ___!___ after the ```scope```, introduces a ___breaking change___. A breaking change can be part of commits of any type.

#### Examples:

Commit message with ```!``` to draw attention to a ___breaking change___:

- ```feat(api)!: send an email to the customer when a product is shipped```

Commit message with ```scope``` lang:

- ```feat(lang): add Polish language```

Commit message with ```type``` fix:

- ```fix(ui): css syntax error```

### Branching Strategy

A **branching strategy** is a set of rules or guidelines that development teams use to manage the process of writing, merging, and deploying code with the help of a version control system like **Git**. It defines how developers interact with a shared codebase.

Strategies like these are essential as they help in keeping project repositories organized, error-free, and **avoid the unwanted merge conflicts** when multiple developers simultaneously push and pull code from the same repository.

To learn more about branching strategies: [Click Me](https://www.geeksforgeeks.org/git/branching-strategies-in-git/)

A _modified_ version of the **GitLab Flow** is used in this project.

#### GitLab Flow (modified):

The types of branches that can be present in GitFlow are:

- **master/main**: Main production branch housing stable release ready code.

- **dev**: Contains new features and bug fixes. This branch is used to combine new features and bug fixes for the release. Upon completion, developers merge the changes from the ```dev``` branch into the ```main/master``` branches.

- **feature/<scope<d>>**: Developers initiate ```feature/*``` branch from the ```dev``` branch to implement new features or address bugs. Upon completion, they integrate the changes from the ```feature/*``` branch into the ```dev``` branch. **For example:**
    - ```feature/ui```: To work on the UI of the project
    - ```feature/api```: To work on the API calls of the project

## Knowing more about Smart-Al-Test 

### Typical Sytem Flow   

In a typical use, the user performs a breath test using the Smart Al-Test device. The ESP32-C6 processes the reading locally and displays the result on the OLED screen. The device then sends the measurement via Bluetooth Low Energy to the browser-based interface hosted on Vercel, where the user is logged in. Through this interface, the data is securely stored, retrieved, and managed using the Supabase backend, allowing the user to view their history and monitor trends over time. 

### Main Technologies

- **Main Hardware**
  - [FireBeetle DFR1075 microcontroller board](https://fi.farnell.com/dfrobot/dfr1075/development-board-iot-risc-v-32bit/dp/4457347)  
  - [MQ-3 alcohol gas sensor](https://fi.farnell.com/dfrobot/sen0128/analogue-alcohol-sensor-arduino/dp/3517899)  
  - [OLED display for local feedback](https://www.partco.fi/fi/arduino/arduino-leikkikenttae/27138-sbc-oled01v2.html)
  - [Li-Po AKKU 3,7V 1350mAh](https://www.partco.fi/fi/akut-ja-paristot/akkukennot/li-po-akut/20365-zlipo-l503759.html)  


- **Connectivity**
  - Bluetooth Low Energy (BLE) between device and browser  

- **Cloud & Web**
  - **Vercel** – hosting and deployment platform for the web app [Homepage](https://vercel.com/)  
  - **Supabase** – PostgreSQL database + user authentication + APIs [Homepage](https://supabase.com/)  

### Features

- Breath alcohol measurement using MQ-3 sensor  
- Local visualization of results on the OLED
- BLE transmission between device and browser  
- User login and authentication via Supabase  
- Cloud storage of historical measurements  
- Web dashboard for viewing results as well as short and longterm 