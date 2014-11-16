File structure : 
**************************************

Files below are from an existing hand tracking project as our lib. Slightly modified.
======================================
main : main() functin modified. Call of our solution added. Frame time calculation added.

handGesture : bRect is made public

myImage

roi
**************************************

Project File Description: 
======================================
GCManager : manage all the gesture command
            new gesture should be added here in order to be working

GestureCommandList : list all gesture command types

GestureCommandBase : a base class for all types of commands.
                     new added gesture command should inherit this class

    Method start(): the detection of particular gesture, transformation of command and execution of command should not be carried out until this start() is called.
    
    Method stop() : all operation should be stopped if this method is called.

    Method update() : giving the instance of HandGesture and time tick as parameters, detect gesture and generate command.

    Method execute() : execute generated command.

TanslationCommand : detect hand movement and give command to the device for moving or turning

