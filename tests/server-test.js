const { BLEServer } = require('./server');

const server = new BLEServer();

server.onClientConnected(() => {
    console.log('ooops! Client connected!');
});

server.onClientDisconnected(() => {
    console.log('ooops! Client disconnected!');
});

server.onDataReceived((data) => {
    console.log('> Data Received: ',data);
});



server.init()
    .then(() => {
        console.log('Server initiated');
        server.start("hello")
        .then(() => console.log('Server started'))
        .catch(err => console.error('Error:', err));
    })
    .catch(err => console.error('Error:', err));

