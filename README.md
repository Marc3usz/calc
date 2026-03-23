# SDL2 Graphing Calculator

A desktop graphing calculator written in C++20 with SDL2 and SDL_ttf.
The application provides an interactive coordinate grid, real-time plotting, expression editing, and basic file import/export workflows.

## Highlights

- Interactive 2D graph rendering (grid, axes, labels, zoom, pan)
- Up to 6 user-defined functions (`a(x)` to `f(x)`)
- Expression parser with arithmetic operators and function composition
- Numerical derivative support via the `'` suffix (for example `sin'(x)`)
- Function import from text file at startup
- Save current functions to `functions.txt`
- Export detected roots to `roots.txt`
- Configurable window size and custom `.ttf` font path

## Technology Stack

- C++20
- SDL2
- SDL2_ttf
- fmt
- cxxopts
- Visual Studio 2022 project (`.sln` / `.vcxproj`) with vcpkg manifest mode

## Requirements

- Windows 10/11
- Visual Studio 2022 (MSVC v143 toolset)
- vcpkg integrated with Visual Studio

Dependencies are declared in `vcpkg.json`.

## Build (Visual Studio)

1. Open `calculator.sln` in Visual Studio 2022.
2. Ensure vcpkg manifest mode is enabled (already configured in the project).
3. Select configuration (`Debug` or `Release`) and platform (`x64` recommended).
4. Build and run the `calculator` project.

## Command-Line Options

The executable accepts the following options:

- `--file <path>`: Load function definitions from a file at startup
- `--font <path>`: Path to a `.ttf` font file
	- Default: `C:\Windows\Fonts\arial.ttf`
- `-w, --width <int>`: Window width (default `800`)
- `-h, --height <int>`: Window height (default `600`)

Example:

```powershell
calculator.exe --file functions.txt --font C:\Windows\Fonts\consola.ttf --width 1280 --height 720
```

## Keyboard Controls

- `m`: Toggle help overlay
- Arrow keys: Pan view
- `+` / `-`: Zoom in / out
- `r`: Reset camera to default range
- `1..6`: Toggle display of a specific function (`a..f`)
- `Shift + 1..6`: Edit selected function
- `Esc`: Exit edit mode
- `a`: Toggle display of all functions
- `Shift + S`: Save function definitions to `functions.txt`
- `Ctrl + Shift + S`: Export roots to `roots.txt`
- `Shift + Esc`: Quit application

## Expression Syntax

### Supported elements

- Variable: `x`
- Numbers: integers and decimals (for example `2`, `3.14`, `.5`)
- Operators: `+`, `-`, `*`, `/`, `^`
- Parentheses: `(`, `)`
- Built-in functions:
	- `sin(x)`
	- `cos(x)`
	- `log(x)`
	- `exp(x)`
	- `logtwo(x)`
	- `tan(x)`
	- `pi(x)` (implemented as `M_PI * x`)

### Derivatives

Use `'` on a function name to evaluate its derivative, for example:

- `sin'(x)`
- `a'(x)` (for a previously defined user function)

### User function dependencies

User functions are named `a` through `f` and must reference only:

- built-in functions, or
- previously defined user functions

This means a function cannot reference itself or a later function identifier.

## File Formats

### Import / save functions

Each line contains a function identifier followed by its expression body:

```text
asin(x)
bcos(x)+a(x)
c(a'(x)^2)
```

- `a...f` are function identifiers.
- Function files can be loaded with `--file`.

### Exported roots

Root export creates a grouped text file (`roots.txt`) with sections per function, for example:

```text
a:
0
3.14159
b:
-1.5
```

## Known Limitations

- Root detection is numerical and does not guarantee all roots are found.
- The current repository setup is targeted at Windows + Visual Studio.
- No packaged cross-platform build pipeline is provided yet.

## Project Structure

- `src/`: Application source files (entry point, rendering, parser, IO)
- `include/`: Header files
- `lib/`, `dlls/`: Local library assets used by the Visual Studio project
- `vcpkg.json`: Dependency manifest

## License

No license file is currently included in this repository.
If you plan to distribute or reuse this project, add an explicit license.
