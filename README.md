# Virtual Tennis

> A multiplayer AR tennis game.

Components:

- `app`.
- `libs/libtennis`.
- `libs/libtennis/tests`.
- `libs/librendering`.
- `libs/libvision`.

## Usage

Clone the project:
```bash
git clone https://gitlab.com/mcmikecreations/virtualtennis.git
cd virtualtennis/
git submodule update --init --recursive
```

Create the `build` directory and configure the build system:

```bash
cmake -B build # Or `cmake -D BUILD_TESTING=OFF -B build` to skip tests. 
```

Build the project:

```bash
cmake --build build
```

Inspect the `build` directory to find the application and the tests.

Optionally, run the tests with `ctest` by typing:

```bash
cmake -E chdir build ctest
```