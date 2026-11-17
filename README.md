# asterisk-kernel

The Asterisk kernel, boots using legacy boot (for now no UEFI support). This is a side project of mine that I made for tinkering with hardware. You can use the drivers I made for the kernel if you want to make your own kernel if it also complies with it being open-source and other things mentioned in the [GPL-3.0 Liscense](https://github.com/maerkfinished/asterisk-kernel?tab=GPL-3.0-1-ov-file).

# Building from source

To build from source, you will need the following utilities:  
**NASM (for assembly)**, **GCC (for all the C code)** and **LD (for linking the object files)**.  

## Installing these tools on Ubuntu:
To install these tools mentioned [here](#Building%20from%20source), run this in your terminal:  

```bash
sudo apt update  
sudo apt install something
```
