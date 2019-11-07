# IDD-Final-Project

**Team**

Frans Fourie fjf46 <br />
Konstantinos Karras Kallidromitis kk984

**Project Idea**

Create a power management system. The system will have a central server that is connected to the local building wifi. There will then be small modules installed in every DB board thru out a building that measures the power consumption of that DB board as well as be connected to a relay board that can control power access to some of the non critical components connected to the DB.

The modules will send back there measurements to the central unit. The central unit will colllect all the data and can send commands to the modules for them to turn things on and off. The system will also have timer capabilities. The user will be able set a priority list indcating in what order things need to be turned on and can also set a maximum time something is allowed to be off. The PI will also do basic Data analysis that will be displayed to the user.

**Expected parts**

1) Raspberry PI

Each module will contain

1) ESP 32
2) 3 x Non invasive amp clamps
3) arduino relay board

**Interaction plan**

People will interact with the PI setting the the priority list and can set timer lists. The user will also be able to look at the saved data.

**Project Prototype fair**

[Video of Project Prototype](https://youtu.be/rNbgpDsmD3E)<br />
