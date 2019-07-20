# License Manager

This license manager implements an authorization mechanism for a projector and its controller application so that an user can only access the application with a specific pc. In order to access the projector and the application, an user must enter a valid license for each device.

## Usage ##

```shell=
git clone https://github.com/minghsu0107/license_manager.git
```

Set permission:

```shell=
cd license_manager
chmod 700 make.sh clean.sh
```

Usage:

```shell=
./make.sh [Host] [Mode] [Options]
	Host
	    -c          for pc (on windows git bash)
	    -p          for projector (on linux)
        Mode
	    -u          using universal key
	    -t          for general cases
	Options
	    -d          debug mode
```
For example, the command below tests the general cases for pc licensing in debug mode.

```shell=
./make.sh -c -t -d
```

**debug_pc.json** or **debug_projector.json** will be created in debug mode, which is the unencrypted content of the generated license.

## Process ##

1. Gather hardware information of the client's device
    - for projector
        - projector serial number
        - mac address
    - for pc
        - hard disk serial number
        - mac address
2. Encrypt the hardware information and send a license request by email
3. Decrypt the data, set service status and registration time, and encrypt the entire data as a license
4. Email the license back to the client
5. Validate the license
    - check if the hardware information matches
    - check expiration
    - Activate corresponding functions according to the service status mentioned in the license

Note that this license_manager is designed to generate a single license from multiple requests(if any).
