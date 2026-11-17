# asterisk-kernel

The Asterisk kernel, boots using legacy boot (for now no UEFI support). This is a side project of mine that I made for tinkering with hardware. You can use the drivers I made for the kernel if you want to make your own kernel if it also complies with it being open-source and other things mentioned in the [GPL-3.0 Liscense](https://github.com/maerkfinished/asterisk-kernel?tab=GPL-3.0-1-ov-file).

# Building from source

To build from source, you will need the following utilities:  
**NASM (for assembly)**, **GCC (for all the C code)**, **LD (for linking the object files)** and **QEMU (for running the kernel, this is something like VirtualBox, but you can run this in the terminal)**.

## Installing these tools on Ubuntu:

To install these tools mentioned [here](#building-from-source), run this in your terminal:  

```
sudo apt update  
sudo apt install nasm gcc ld
```

This will install these tools that are neccesary for building the **Asterisk kernel**.

## Cloning [this](https://github.com/maerkfinished/asterisk-kernel/) Github repo & building the kernel:

Here are the commands you will run in your terminal:

```
git clone https://github.com/maerkfinished/asterisk-kernel/
cd asterisk-kernel
make
```

If you have done everything correctly, these commands will run **suscesfully** and you will run the kernel **automatically** in a virtual machine.


### One last thing:
I am sorry if this README file is bad or my english is bad here, this is my first readme file that I have made and it may look *wonky* to you.
