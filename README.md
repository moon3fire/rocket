# Rocket engine

Rocket is an early stage game engine. It's being developed slowly but confidently.

## Description

Rocket's architecture allows to support multiple rendering API's at once(only OpenGL for now)
Currently supported platforom is x64 Windows.

## Features

Rocket currently support these features:

- Basic lighting techniques
- Textures, Subtextures(divinding textures into many by pixels, useful for spritesheets)
- GUI(Editor camera, useful panels)
- Basic postprocessing(Color correction, bloom).
- Scene management
- ECS
- Build System(premake)
## Installation

```bash
# Windows: Installation steps
git clone https://github.com/moon3fire/rocket.git --recursive
cd scripts
run Win-GenProjects.bat
```

# Usage
run rocket.sln in VS Studio 2022(if you have other version, change version in builder .bat script for premake)

# License
This project is licensed under the Apache License 2.0 - see the LICENSE file for details.

# Acknowledgements
This project is based on the architecture shown in game engine series of github.com/TheCherno
I am very grateful to him for the tutorial, starting at some point I'm doing things here by myself, to make this project useful for newcomers both in game development and game engine development.

# Contact
See my README
