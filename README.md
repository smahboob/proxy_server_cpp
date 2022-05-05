## Custom Proxy Server By Saad Mahboob and Omar Khater

### Getting Started
There are two requirements before getting started on this project: a C++11 compiler (likely the case for any computer < 5 years old) and [CMake](https://cmake.org/download/) (3.0 or greater). Follow the instructions on the website to install CMake. Afterwards, when opening a new terminal, typing cmake should prompt you with a usage message.

After installing these prerequisites, you should be able to compile the code provided in this repositoy. First, you need to clone this to your machine:

```
git clone 
cd 
```

Now, we need to set up a `build/` directory for CMake:

```
mkdir build
cd build
```

Inside the build directory we can run CMake to generate a Makefile for the project.
```
cmake ..
```

Now, we can run make as usual to generate our executable (`proxy` and `client` in this case). Afterwards, we can run `proxy` and `client` in two seperate terminals from inside the build directory.
```
make
./proxy (one terminal) - Must start proxy server before sending requests from client. 
./client (other terminal) - User interaface to send requests, handle filtering keywords
```

Assuming all of that worked, you should see the proxy server saying `Waiting for Client Request...` and client displaying a menu to choose from. Choose different options and play around. 
