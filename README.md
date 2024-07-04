# BLE Connection

This is a node addon api package used for connecting other bluetooth through Bluethooth SPP. IMPORTANT! It supports for Windows platform. For now, we can only connect to other bluetooth spp. It means, this package does not act as server.

## Getting Started

This library is developed in C++ and support for Node.js.

### Installation

Install this package into your project by below command

```
npm install @siva7170/ble-connection
```

## Usage

Below code is sample for how to use it. Please see methods and its functionalities below sections.

```javascript
const bleConnection = require('@siva7170/ble-connection');

const bleConnInstance = new bleConnection.BLEConnection();
```

## Methods

### Initiate(successCallback,failureCallback)

- **successCallback**:
  - Type: `Function`

- **failureCallback**:
  - Type: `Function`

It initializes the necessary things.

```javascript
bleConnInstance.Initiate(()=>{
	console.log("Initiated!");
	// rest of the code
},()=>{
	console.log("Failed to initiate!");
});
```

### Connect(bluetooth_addr, uuid, successCallback,failureCallback)

- **bluetooth_addr**:
  - Type: `String`

- **uuid**:
  - Type: `String`

- **successCallback**:
  - Type: `Function`

- **failureCallback**:
  - Type: `Function`

This method will try to connect to the given bluetooth address and uuid from bluetooth spp server.

```javascript
// please use valid bluetooth address and UUID
bleConnInstance.Connect("00:00:00:00:00:E0","aaaaaaaa-aaaa-4444-cccc-999888999888",()=>{
	console.log("Connected!");
    // rest of the code
},()=>{
	console.log("Failed to connect!");
});
```

### SendData(data, successCallback, failureCallback) (optional)

- **data**:
  - Type: `String`

- **successCallback**:
  - Type: `Function`

- **failureCallback**:
  - Type: `Function`
  
With this method, you can send data to client

```javascript
bleConnInstance.SendData('Hi server!',(res)=>{
	console.log("Data sent: "+sData);
}, (err)=>{

});
```

### OnReceiveData(onDataRecvCallback)

- **onDataRecvCallback**:
  - Type: `Function`

This method will be triggered when the data sent from bluetooth server

```javascript
bleConnInstance.OnReceiveData((data)=>{
	console.log("Data receivedd: "+data);
});
```

## Full Example
  
Please find full example of implementation

```javascript
const bleConnection = require('@siva7170/ble-connection');

const bleConnInstance = new bleConnection.BLEConnection();

try{
    bleConnInstance.Initiate(()=>{
        console.log("Initiated!");

        bleConnInstance.Connect("00:00:00:00:00:E0","aaaaaaaa-aaaa-4444-cccc-999888999888",()=>{
            console.log("Connected!");
            let sData="Hi client";
            bleConnInstance.SendData(sData,(res)=>{
                console.log("Data sent: "+sData);
            }, (err)=>{

            });

            bleConnInstance.OnReceiveData((data)=>{
                console.log("Data receivedd: "+data);
            });
            
            sData="How are you?";
            bleConnInstance.SendData(sData,(res)=>{
                console.log("Data sent: "+sData);
            }, (err)=>{

            });
        },()=>{
            console.log("Failed to connect!");
        });
    },()=>{
        console.log("Failed to initiate!");
    });
}catch(e){
    console.error(e.toString());
}
```


# Version 2


## Usage

Below code is sample for how to use it. Please see methods and its functionalities below sections.

```javascript
const bleConnection = require('@siva7170/ble-connection');

const bleConnInstance = new bleConnection.BLEConnection();
```

## Methods

### SetBtInfo(bluetooth_addr, uuid, timeout_for_reconnect, no_of_attempt_to_reconnect)

- **bluetooth_addr**:
  - Type: `String`

- **uuid**:
  - Type: `String`

- **timeout_for_reconnect**:
  - Type: `Integer`

- **no_of_attempt_to_reconnect**:
  - Type: `Integer`

It sets necessary things to BLE Connection before it initialize. 

```javascript
bleConnInstance.SetBtInfo("00:00:00:00:00:E0","aaaaaaaa-aaaa-4444-cccc-999888999888",5000,0);
```

### GetStatus(statusCallback) (optional)

- **statusCallback**:
  - Type: `Function`

This method will give current state of BLE Connection when it is changed from one to another.

```javascript
bleConnInstance.GetStatus((res)=>{
  console.log("Status: "+res);
});
```

### MakeConnection(successCallback, failureCallback) 

- **successCallback**:
  - Type: `Function`

- **failureCallback**:
  - Type: `Function`
  
It will make connection to device which is defined in SetBtInfo()

```javascript
bleConnInstance.MakeConnection(()=>{
    console.log("Connected...!");
},
()=>{
    console.log("Failed to connect!");
});
```

### IsConnected(successCallback, failureCallback) (optional)

- **successCallback**:
  - Type: `Function`

- **failureCallback**:
  - Type: `Function`

This method will be triggered when connection made to Bluetooth Server.

```javascript
    bleConnInstance.IsConnected(()=>{
        console.log("Connection status: Connected!");
    },
    ()=>{
        console.log("Failed to connect!");
    });
```

### SendDataToServer(data, successCallback, failureCallback) (optional)

- **data**:
  - Type: `String`

- **successCallback**:
  - Type: `Function`

- **failureCallback**:
  - Type: `Function`
  
With this method, you can send data to client

```javascript
bleConnInstance.SendDataToServer('Hi server!',(res)=>{
	console.log("Data sent: "+sData);
}, (err)=>{

});
```

### OnReceiveDataFromServer(onDataRecvCallback)

- **onDataRecvCallback**:
  - Type: `Function`

This method will be triggered when the data sent from bluetooth server

```javascript
bleConnInstance.OnReceiveDataFromServer((data)=>{
	console.log("Data receivedd: "+data);
});
```


## Full Example
  
Please find full example of implementation

```javascript
const bleConnection = require('@siva7170/ble-connection');

const bleConnInstance = new bleConnection.BLEConnection();

try{
    bleConnInstance.SetBtInfo("00:00:00:00:00:E0","aaaaaaaa-aaaa-4444-cccc-999888999888",5000,0);

    
    bleConnInstance.GetStatus((res)=>{
        console.log("Status: "+res);
    });


    bleConnInstance.MakeConnection(()=>{
        console.log("Connected...!");
    },
    ()=>{
        console.log("Failed to connect!");
    });

    bleConnInstance.IsConnected(()=>{
        console.log("Connection status: Connected!");

       // INITIATE
      bleConnInstance.SendDataToServer("INITIATE",(res)=>{
        console.log("Initiating...!");
      }, (err)=>{

      });

      bleConnInstance.OnReceiveDataFromServer((data)=>{
        bleConnInstance.SendDataToServer("READY_SCAN",(res)=>{
          console.log("Ready scan...!");
        }, (err)=>{
                    
        });
      });
    },
    ()=>{
        console.log("Failed to connect!");
    });

}catch(e){
    console.error(e.toString());
}
```

## TODO

- [x] Bluetooth SPP Client (Windows)
- [ ] Bluetooth SPP Client (Other platform)
- [ ] Bluetooth SPP Server (for all platform)
- [ ] Modify all code more efficient

## Contribution

I created this package for my own usage. I welcome contribution for this package improvement.