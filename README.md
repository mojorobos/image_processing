# Processing
> Processing is an edge detection tool part of a Rembrandt-style robotic arm solution developed by Mojo Robos

### Status
Processing is currently under development, once Mojo Robos is still improving its edge detection algorithms.

### Installing
  - `cd /path/to/image_processing/`
  - `git clone --depth=1 https://github.com/mojorobos/image_processing.git`
  - `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/image_processing/libs/libjpeg/lib/`
  - `make`

### Usage
  - `./processing "path/to/imageIn.jpg" "path/to/imageOut.jpg"`

### Args specs
  - `imageIn`: Image input
<p align="center">
	<img alt="BrainStation" src="./assets/in.jpg" height="500px" />
</p>

  - `imageOut`: Image output
<p align="center">
	<img alt="BrainStation" src="./assets/out.jpg" height="500px" />
</p>

# LICENSE
This project extends GNU GPL v. 3, so be aware of that, regarding copying, modifying and (re)destributing the application.

