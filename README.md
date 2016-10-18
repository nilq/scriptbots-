Script Bots 2.0
---

An approach on making a new and better version of the already quite nice evolutionary artificial life simulation of predator-prey dynamics.

####Building

---

Dependencies

- CMake >= 2.8
- OpenGL/GLUT

On Ubuntu/Debian, all dependencies can be `apt-get`'ed like this:

```bash
$ apt-get install cmake build-essential libopenmpi-dev libglut32-dev libxi-dev libxmu-dev
```

On Linux the project can be built like this:

```bash
$ mkdir builds
$ cd builds/
$ cmake ../
$ make
```

This will create an executable `scriptbots2` that can be run like this:

```bash
$ ./scriptbots2
```
