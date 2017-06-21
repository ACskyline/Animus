# Animus
## my _graduation_ project
My professor told me to build an animation editor to simulate humen walking. Like Jeremy Clarkson always says, "How _hard_ can it be?"

### Intro
This program is the first step to the animation editor. My current work is to build my own interface to load .obj and .fbx files. 
It is built on Visual Studio 2013. Glew, freeglut and glm are included under the __Dependencies__ folder. 
The intended OpenGL version is 4.5.

### Update
It turns out I am supposed to make a displayer, not an editor and I think I am getting somewhere. Basically I used FBXSDK to read .fbx files and OpenGL to display it.
To simulate large crowd, I applyed instanced rendering and GPU skinning.

### Work to do
I have to figure out a way to decrease CPU overhead when calculating new position for each instance. Currently I am planning to use transform feed back to move the calculation from CPU to GPU. Hope I am right about it.

### Update 2
All finished for now. GIFs added. 6/21/2017

*Crowd Manipulation 1*
![Crowd Scale](https://github.com/ACskyline/Animus/tree/test/img/crowd_scale.gif)

*Crowd Manipulation 2*
![Crowd Move](https://github.com/ACskyline/Animus/tree/test/img/crowd_move.gif)

*Crowd Manipulation 3*
![Crowd Number](https://github.com/ACskyline/Animus/tree/test/img/crowd_number.gif)

*Crowd Manipulation 4*
![Crowd Camera](https://github.com/ACskyline/Animus/tree/test/img/crowd_camera.gif)