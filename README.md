## memctl-kext-core

<!-- Brandon Azad -->

memctl-kext-core is a [memctl] core for macOS that uses a custom kernel extension to access the
kernel task port. Since it relies on loading an unsigned kernel extension, System Integrity
Protection must be disabled first.

[memctl]: https://github.com/bazad/memctl

It has been tested with macOS 10.12.4 and 10.12.6.

### Building

To set up memctl to use this core:

	$ git clone https://github.com/bazad/memctl
	$ cd memctl
	$ git clone https://github.com/bazad/memctl-kext-core
	$ ln -s memctl-kext-core/core core

To build the core:

	$ cd memctl-kext-core/core
	$ ln -s ../.. memctl
	$ make
	$ cd ../..

To build and load the kernel extension:

	$ cd memctl-kext-core/kext
	$ xcodebuild
	$ sudo cp -R ./build/Release/KernelTask.kext /tmp/
	$ sudo kextload /tmp/KernelTask.kext
	$ cd ../..

To build memctl with this core:

	$ make ARCH=x86_64 SDK=macosx

### Running

After memctl has been successfully built and the kernel extension has been loaded, you can run
memctl without arguments to drop into a REPL:

	$ bin/memctl
	memctl> r _vm_kernel_slide
	setting up kernel function call...
	ffffff802528b750:  0000000024800000
	memctl> 

### License

memctl-kext-core is released under the MIT license.
