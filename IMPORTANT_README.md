General Advice for system setup:
The Project solution should function regardless of system setup, however, in the case the system fails to load due to missing files, this may be due to visual studio being unaware of the file locations on an external drive. to fix this run the './buildVS2022Project.bat' script and try again.
To run the project, press the local windows debugger, with release mode enabled rather than debug. the debug version of the system has numerous tools enabled for system debugging and is apropriately used only for development 
Once the project is running the ui should be displayed. To load a file enter any of the following file names into the load scene box.
Test.eph,
MechScene.eph,
MechScene2.eph.
The Assets avaliable to load are listed in the Tutorial Information section and can be loaded via the file tab
The scale of the mech model is large so id reccommend scaling the mech model down to 0.1 in xyz so that it fits more appropriately into a scene.
Press 'Tab' to switch to the Scene View and 'Alt' to switch to back to the UI.
A known error with the system is that if a file size exceeds the avaliable internal memory buffer size which is based on your systems internal ram, certain files may fail to load. 
The internal assets should all be within the parameters of any normal system, but in the case of the engine being run on a system too slow to support this, a video file has been attached to showcase this system.
The file is called: SystemFunctionalityTest.mp4