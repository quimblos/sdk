# Quimblos SDK

![A C++ automation framework for embedded systems.](docs/banner.png)

> This is a work-in-progress. Feel free to open issues and discuss changes.

<!-- vscode-markdown-toc -->
* 1. [Engine, Devices and Runners](#EngineDevicesandRunners)
	* 1.1. [Devices](#Devices)
	* 1.2. [Constants](#Constants)
	* 1.3. [Runners](#Runners)
* 2. [Script](#Script)
	* 2.1. [Types](#Types)
	* 2.2. [Commands](#Commands)
		* 2.2.1. [[0x0*] Parser commands](#0x0Parsercommands)
		* 2.2.2. [[0x1*] Register manipulation commands](#0x1Registermanipulationcommands)
		* 2.2.3. [[0x2*] Flow control commands](#0x2Flowcontrolcommands)
		* 2.2.4. [[0x3*] Arithmetic commands](#0x3Arithmeticcommands)
		* 2.2.5. [[0xD*] Log commands](#0xDLogcommands)
		* 2.2.6. [[0xE*] Runner commands](#0xERunnercommands)
		* 2.2.7. [[0xF*] Engine commands](#0xFEnginecommands)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

##  1. <a name='EngineDevicesandRunners'></a>Engine, Devices and Runners

You start by setting up an `Engine`, which allows controlling multiple _devices_ through _runners_.

```cpp
qb::Engine engine;
```

###  1.1. <a name='Devices'></a>Devices

In order to actually interact with hardware, you must declare a device, with a given set of registers.

```cpp
class MyDevice : public qb::Device {
    public:    
    Device(): qb::Device("my_device") {
        this->addRegister(qb::Data::u8());
        this->addRegister(qb::Data::u32());
        this->addRegister(qb::Data::string());
    }
};
```

Then, you instantiate your `Device` and add it to ehe engine:

```cpp
MyDevice device;
engine.putDevice(device);
```

###  1.2. <a name='Constants'></a>Constants

You can also add `Constants` to the engine, which can be referenced by _runners_.

```cpp
engine.putConstant("ON", qb::Data::b8(0b11111111));
```

###  1.3. <a name='Runners'></a>Runners

Each project has different requirements on how a _Runner_ should work, so you must implement one to actually run the scripts.

Below is a very simple, synchronous runner:

```cpp
class MyRunner : public qb::Runner {
    
    public:
        MyRunner(qb::Engine& engine, std::string name, const qb::Script* script)
            : qb::Runner(engine, name, script) {};
            
        void run() {
            runner->start();
            while (runner->tick()) {
                if (runner->state == qb::runner::State::SLEEPING) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(runner->sleep));
                    runner->wakeup();
                }
            }
        }
};
```

Then, you instantiate your `Runner` with a `Script`, usually parsed from a _hexcode_:

```cpp
std::string hexcode = "7162000001076c65646c696e651100001b55e0000003e81100001baae0000003e8200001"
auto parser_out = qb::parser::parse(engine, "blink", blink);
if (!parser_out.ok) {
    std::cout << "[error] " << parser_out.message << std::endl;
}

auto runner_out = engine.makeRunner<MyRunner>("blink", parser_out.script);
if (!runner_out.ok) {
    std::cout << "[error] " << runner_out.message << std::endl;
}

qb::Runner* runner = runner_out.runner;
```


##  2. <a name='Script'></a>Script

A `Script` is what defines the behavior of a _runner_.

It's written as a sequence of _Command Bytes_ bytes, each followed by a specific number of _Argument Bytes_ and _Value Bytes_, then parsed into a list of _Commands_.

###  2.1. <a name='Types'></a>Types

Some operations require a _Type Byte_ before the _Value Bytes_.

Below are the valid _Type Bytes_.

```c
// 0 bytes
VOID = 0x01

// 1 byte
UINT8 = 0x10
INT8 = 0x1A
BITMASK8 = 0x1B

// 2 bytes
UINT16 = 0x20
UINT8_XY = 0x21
INT16 = 0x2A
INT8_XY = 0x2B

// 4 bytes
UINT32 = 0x40
UINT16_XY = 0x41
UINT8_XYZW = 0x42
INT32 = 0x4A
INT16_XY = 0x4B
INT8_XYZW = 0x4C

// n bytes
STRING = 0xF0
CONST = 0xFF
```

###  2.2. <a name='Commands'></a>Commands

```c
USE_DEVICE "led_strip"
USE_PORT UINT8

SET 0x00 0x00 UINT8 0x01

GOTO 0x0001
IF_EQ 0x00 0x00 0x0001 0x0000 STRING "red"
IF_GT 0x00 0x00 0x0001 0x0000 STRING "red"
IF_GTEQ 0x00 0x00 0x0001 0x0000 STRING "red"

SUM 0x00 0x00 UINT8 0x01
SUB 0x00 0x00 UINT8 0x01
MULT 0x00 0x00 UINT8 0x01
DIV 0x00 0x00 UINT8 0x01
MOD 0x00 0x00 UINT8 0x01

LOG 0x00 0x10 "Things are going fine"

SLEEP 0x00000001
STOP 0x10 "It's ok"
ERROR 0x12 "Something's wrong"

RESET
REBOOT
```

####  2.2.1. <a name='0x0Parsercommands'></a>[0x0*] Parser commands

##### [0x01] USE_DEVICE

Marks a device to be used by this script.
The device will be assigned an increasing index (starting at 0), which can be used on the script to reference a device register.

> This command is not included on the final bytecode, thus doesn't count on jump indexes. It's recommended to be at the top of the script.

```c
USE_DEVICE {CONST}

/*
    USE_PORT "led_strip"
*/
```

##### [0x02] USE_PORT

Creates a register on the runner.
This register can be references through the _0xFF_ device.

> This command is not included on the final bytecode, thus doesn't count on jump indexes. It's recommended to be at the top of the script.

```c
USE_PORT type

/*
    USE_PORT UINT8
*/
```

####  2.2.2. <a name='0x1Registermanipulationcommands'></a>[0x1*] Register manipulation commands

##### [0x11] SET

Sets the value of a register on a device.

```c
SET device_i port type {*}

/*
    SET 0x00 0x00 UINT8 0x01
*/
```

####  2.2.3. <a name='0x2Flowcontrolcommands'></a>[0x2*] Flow control commands 

##### [0x20] GOTO

Jumps to a specific address of the script.

> If the address 0xFFFF is specified, it jumps to the next address.

```c
GOTO addr[2]

/*
    GOTO 0x0001
*/
```

##### [0x21] IF_EQ

If a given value is equal to a register value, jumps to a specific address (starting at 1) of the script.

> If the address 0xFFFF is specified, it jumps to the next address.

```c
IF_EQ device_i port addr_true[2] addr_false[2] type {*}

/*
    IF_EQ 0x00 0x00 0x0001 0x0000 STRING "red"
*/
```

##### [0x22] IF_GT

If a given value if greater than a register value, jumps to a specific address (starting at 1) of the script.

> If the address 0xFFFF is specified, it jumps to the next address.

```c
IF_GT device_i port addr_true[2] addr_false[2] type {*}

/*
    IF_GT 0x00 0x00 0x0001 0x0000 STRING "red"
*/
```

##### [0x23] IF_GTEQ

If a given value is greater than or equal to a register value, jumps to a specific address (starting at 1) of the script.

> If the address 0xFFFF is specified, it jumps to the next address.

```c
IF_GTEQ device_i port addr_true[2] addr_false[2] type {*}

/*
    IF_GTEQ 0x00 0x00 0x0001 0x0000 STRING "red"
*/
```

####  2.2.4. <a name='0x3Arithmeticcommands'></a>[0x3*] Arithmetic commands
  SUM

Adds a value to a register on a device.

```c
SUM device_i port type {*}

/*
    SUM 0x00 0x00 UINT8 0x01
*/
```
  SUB

Subtracts a value to a register on a device.

```c
SUB device_i port type {*}

/*
    SUB 0x00 0x00 UINT8 0x01
*/
```
  MULT

Multiplies a value to a register on a device.

```c
MULT device_i port type {*}

/*
    MULT 0x00 0x00 UINT8 0x01
*/
```
  DIV

Divides a register on a device by a value.

```c
DIV device_i port type {*}

/*
    DIV 0x00 0x00 UINT8 0x01
*/
```
  MOD

Takes the modulo operation of a register on a device and a value.

```c
MOD device_i port type {*}

/*
    MOD 0x00 0x00 UINT8 0x01
*/
```

####  2.2.5. <a name='0xDLogcommands'></a>[0xD*] Log commands

##### [0xD0] LOG

Outputs a log message to the runner.

```c
LOG device_i log_code {STRING}

/*
    LOG 0x00 0x10 "Things are going fine"
*/
```

####  2.2.6. <a name='0xERunnercommands'></a>[0xE*] Runner commands

##### [0xE0] SLEEP

Holds the runner for a given time (in milliseconds).

```c
SLEEP {UINT32}

/*
    SLEEP 0x00000001
*/
```

##### [0xEE] STOP

Stops the runner with an OK state and a message.

```c
STOP log_code {STRING}

/*
    STOP 0x10 "It's ok"
*/
```

##### [0xEF] ERROR

Stops the runner with an ERROR state and a message.

```c
ERROR log_code {STRING}

/*
    ERROR 0x12 "Something's wrong"
*/
```

####  2.2.7. <a name='0xFEnginecommands'></a>[0xF*] Engine commands

##### [0xF0] RESET

Resets the engine.

```c
RESET

/*
    RESET
*/
```

##### [0xFF] REBOOT

Reboots the engine.

```c
REBOOT

/*
    REBOOT
*/
```