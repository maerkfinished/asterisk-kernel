# asterisk-kernel

The Asterisk kernel, boots using legacy boot (for now no UEFI support). This is a side project of mine that I made for tinkering with hardware. You can use the drivers I made for the kernel if you want to make your own kernel if it also complies with it being open-source and other things mentioned in the [GPL-3.0 Liscense](https://github.com/maerkfinished/asterisk-kernel?tab=GPL-3.0-1-ov-file).

# Building from source

To build from source, you will need the following utilities:  
**NASM (for assembly)**, **GCC (for all the C code)**, **LD (for linking the object files)** and **QEMU (for running the kernel, this is something like VirtualBox, but you can run this in the terminal)**.

## Important thing before building (on Linux):

This project relies on **i686-elf-gcc** and **i686-elf-ld**, which are harder to obtain packages.  
To obtain these packages, you have **2** options:

1. Building these packages from source (this option is more complicated and takes a LONG time)
2. Install [Homebrew](https://brew.sh/) and then install the pre-compiled packages (much faster and the preferred option overall)

I don't have a tutorial for the **first option**, since I couldn't get that working myself, so here is the tutorial for the **second option**:  

## Installing these tools on Ubuntu:

To first install Homebrew, run this command:

`/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`

*If it says that curl is not installed or something simmilar, please install **curl** with the following command:*

`sudo apt install curl`

Then, it will tell you to also run some commands to add Homebrew to your system's **PATH**.  
Here are the commands for the PATH:  



To install these tools mentioned [here](#building-from-source), run this in your terminal:  

```
sudo apt update  
sudo apt install nasm gcc binutils
```

This will install these tools that are neccesary for building the **Asterisk kernel**.

## Cloning [this](https://github.com/maerkfinished/asterisk-kernel/) Github repo & building the kernel:

Here are the commands you will run in your terminal:

```
git clone https://github.com/maerkfinished/asterisk-kernel/
cd asterisk-kernel
mkdir build
make
```

If you have done everything correctly, these commands will run **suscesfully** and you will run the kernel **automatically** in a virtual machine.


### One last thing:
I am sorry if this README file is bad or my english is bad here, this is my first readme file that I have made and it may look *wonky* to you.
