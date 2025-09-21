# C++ Inliner & Minifier

A powerful Python utility to bundle C++ header files directly into a
source file. It is designed to simplify project submissions for
competitive programming, online judges, or for creating single-file
distributions of a project.

The script offers advanced, configurable minification options to produce
clean and compact code.

## Features

-   **Header Inlining**: Replaces `#include "..."` directives with the
    actual header content. Only headers with a `//@` marker at the end
    of the line are processed.
-   **Comment Stripping**: Automatically removes both `//` and
    `/* ... */` comments from inlined headers.
-   **Code Minification**: Optional, aggressive minification with
    multiple styles:
    -   `none`: Strips comments and blank lines only.
    -   `multiline`: Minifies code and wraps it intelligently at a max
        character limit.
    -   `single_line`: Minifies all code into a single line, correctly
        handling preprocessor directives.
-   **Flexible Configuration**: Control all features through a simple
    JSON config file.
-   **Smart Config Path**: Automatically searches for a config file in
    the command-line path, current directory, or a global user
    directory.
-   **Easy Installation**: A simple `--install` flag to copy the script
    to a user's local bin directory for system-wide access.
-   **Simple CLI**: Easy-to-use command-line interface.

## Prerequisites

-   Python 3.x
-   No external Python libraries are required.

## Installation (Recommended)

To make the script available as a command (`inliner`) from anywhere in
your terminal, you can use the `--install` flag. This action does not
require sudo permissions.

``` bash
python3 inliner.py --install
```

This will copy the script to `~/.local/bin/inliner` and make it
executable. The script will guide you to ensure `~/.local/bin` is in
your shell's PATH for the command to be recognized.

Once installed, you can run the tool simply by typing `inliner`.

## Usage

If you have installed the script, you can use `inliner` instead of
`python inliner.py`.

### Basic Usage

``` bash
inliner <input_file.cpp>
```

### Specifying a Configuration File

Use the `-c` or `--config` flag to provide a path to a specific
configuration file.

``` bash
inliner <input_file.cpp> -c /path/to/my_config.json
```

## Configuration

The script is controlled by a configuration file named
`inliner_config.json`. The script searches for this file in the
following order: 1. The path provided via the `--config` command-line
flag. 2. In the current working directory (`./inliner_config.json`). 3.
In the global user config directory (`~/.config/inliner/config.json`).

If no file is found, it uses safe default settings.

### Example `inliner_config.json`

``` json
{
  "output_dir": "build",
  "minify_style": "single_line",
  "multiline_max_chars": 120
}
```

### Configuration Options

| Option | Type | Description |
|---|---:|---|
| `output_dir` | string | Directory where the processed output file will be saved (relative to the location where you run the script). Default: `build`. |
| `minify_style` | string | Minification strategy: `none` — strips comments and blank lines; `multiline` — minifies and wraps lines at `multiline_max_chars`; `single_line` — minifies all code into a single line and correctly handles preprocessor directives. Default: `none`. |
| `multiline_max_chars` | integer | Maximum number of characters per line when using the `multiline` style. Ignored for other styles. Default: `120`. |

## Example Workflow

### 1. File Structure

    my_project/
    ├── main.cpp
    ├── helpers.h
    ├── inliner.py
    └── inliner_config.json

### 2. Header File (`helpers.h`)

``` cpp
#pragma once
#include <string>

// A simple function
std::string get_greeting() {
    return "Hello, Inliner!";
}

#define VERSION "1.0"
```

### 3. Source File (`main.cpp`)

To mark a header for inlining, add the `//@` marker at the end of the
line. This gives you precise control over which headers are included.

``` cpp
#include <iostream>
#include "helpers.h" //@

int main() {
    std::cout << get_greeting() << std::endl;
    std::cout << "Version: " << VERSION << std::endl;
    return 0;
}
```

### 4. Run the Script

``` bash
inliner main.cpp
```

### 5. Result

Assuming `minify_style` was set to `"single_line"`, the script will
generate `build/main.cpp`:

``` cpp
#include <iostream>
// --- Start of inlined file: helpers.h ---
#pragma once
#include <string>
#define VERSION "1.0"
std::string get_greeting(){return "Hello, Inliner!";}
// --- End of inlined file: helpers.h ---

int main() {
    std::cout << get_greeting() << std::endl;
    std::cout << "Version: " << VERSION << std::endl;
    return 0;
}
```

## License

This project is licensed under the [MIT License](LICENSE).
