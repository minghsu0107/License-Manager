# License Manager (windows)

## Process ##

1. get computer information
    - hard disk serial number
    - mac address
    - system time (set as registration time)
2. get projector serial number from **projector\\id.txt**
3. encrypt the above information and make a request to cloud(by email); create folder **email_info** and 
   save the encrypted data **email_info\\[host_name]-[user_name].txt**
4. generate license
    - decrypt the information in email_info
    - add function status and license duration to the decrypted package from 
      **service_status\\[host_name]-[user_name].json**
    - encrypt the entire data as a license; create folder **my_license** and save the 
      license under **my_license\\license.txt**
5. authenticate the license
    - decrypt the license and parse the following information
        - hardware info (for validating pc)
        - projector serial number (for validating projector)
        - registration time (for checking expiration)
        - service status
            - activated function
                -  for activating corresponding services
            - license duration
                - for checking expiration
- note that this license_manager is designed to generate a single license from multiple requests(if any).

## usage ##

When using main.cpp for testing, one should enter *hostname* and *whoami* in cmd to attain computer name and user name respectively. Then go to **license_manager\\src\\service_status** and rename MINGHSU-Ming Hsu.json as **[host_name]-[user_name].json**.

Run the following command:

```shell=
g++ -std=gnu++11 main.cpp get_computer_info.cpp license_generator.cpp license_validation.cpp xxtea.cpp
```
Debug mode:
```shell=
g++ -std=gnu++11 -DDEBUG main.cpp get_computer_info.cpp license_generator.cpp license_validation.cpp xxtea.cpp 
```
The above command will generate **get_info.json** and **result.json**, which are results of running get_computer_info.cpp
and license_generator.cpp respectively.
