# ProgLang2

ProgLang2 is a simple proof-of-concept programming language designed to replace [ProgLang](https://github.com/JoCraft2010/proglang). It is still in the early stages of development and is intended for experimental purposes.

> [!WARNING]
> Do not use ProgLang2 in any production environment as it is very early in development and lacks many crucial features.

## Installation
To use ProgLang2 on your Computer you need to install the required dependencies.

### Dependencies
| Dependency | Version  | Description               | Notes                                |
|------------|----------|---------------------------|--------------------------------------|
| GCC/G++    | >= 10.0  | GNU Compiler Collection   | May be preinstalled on many machines |
| LLVM (llc) | ~= 18.0  | Low Level Virtual Machine |                                      |
| GIT        | >= 1.6.6 | Version Control System    | Optional; recommended for cloning    |

### Installing Dependencies

#### Arch Linux
```sh
sudo pacman -Syu gcc llvm
```

#### Debian and Ubuntu-based systems
```sh
sudo apt update
sudo apt upgrade
sudo apt install build-essential llvm
```

#### Fedora Linux
> [!NOTE]
> The installation process and the compiler itself have not been tested on Fedora Linux.
```sh
sudo dfn update
sudo dnf install gcc gcc-c++ llvm llvm-tools
```

#### OpenSUSE
> [!NOTE]
> The installation process and the compiler itself have not been tested on OpenSUSE.
```sh
sudo zypper refresh
sudo zypper install gcc gcc-c++ llvm llvm-tools
```

### Installing Git (Optional)

If you prefer to clone the repository, you’ll need Git. Here’s how to install it on various distributions:

#### Arch Linux
```sh
sudo pacman -Syu git
```

#### Debian and Ubuntu-based systems
```sh
sudo apt install git
```

#### Fedora Linux
```sh
sudo dnf install git
```

#### OpenSUSE
```sh
sudo zypper install git
```

### Cloning the git repository
This step requires Git. If you choose not to use Git, please use an alternative method to obtain the project files.
```sh
git clone --depth 1 https://github.com/JoCraft2010/ProgLang2.git
cd ProgLang2
```

### Compiling

> [!CAUTION]
> The code produced by the compiler, as well as the compiler itself, is unstable. The developer does not take any responsibility for any damages caused by the malfunction of the compiler, including but not limited to uncontrolled memory leaks and corruption of system resources.

After entering the directory, you can use make to compile both the compiler and the ProgLang2 source code:
```sh
make
```

## Conclusion

Feel free to contribute to the development of ProgLang2. Your feedback and suggestions are welcome as we work towards enhancing the capabilities of this proof-of-concept language.
