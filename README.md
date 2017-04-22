# Animus
## my _graduation_ project
My professor told me to build an animation editor to simulate humen walking. Like Jeremy Clarkson always says, "How _hard_ can it be?"

This program is the first step to the animation editor. My current work is to build my own interface to load .obj and .fbx files. 
It is built on Visual Studio 2013. Glew, freeglut and glm are included under the __Dependencies__ folder. 
The intended OpenGL version is 4.5.

### Update
It turns out I am supposed to make a displayer, not an editor and I think I am getting somewhere. Basically I used FBXSDK to read .fbx files and use OpenGL to display it.
To simulate large crowd, I applyed instanced rendering and GPU skinning.

### Work to do
I have to figure out a way to decrease CPU overhead when calculating new position for each instance. Currently I am planning to use transform feed back to move the calculation from CPU to GPU. Hope I am right about it.