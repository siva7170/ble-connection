const addon = require('./../build/Release/BLEConnectionNodeAddon');

const bleServerInstance = new addon.BLEServer();

// bleServerInstance.Initiate();

// bleServerInstance.StartServer("Hello");

class BLEServer {
    constructor() {
        this.keepAliveInterval = null;
        this.server = new addon.BLEServer();
    }
    
    init() {
        return new Promise((resolve, reject) => {
            try {
                const result = this.server.Initiate();
                resolve(result);
            } catch (err) {
                reject(err);
            }
        });
    }

    start(serviceName) {
        return new Promise((resolve, reject) => {
            try {
                const result = this.server.StartServer(serviceName);

                // Keep event loop alive
                this.keepAliveInterval = setInterval(() => {}, 1000);

                resolve(result);
            } catch (err) {
                reject(err);
            }
        });
    }

    onClientConnected(callback) {
        this.server.OnClientConnected(callback);
    }

    onClientDisconnected(callback) {
        this.server.OnClientDisconnected(callback);
    }

    onDataReceived(callback) {
        this.server.OnData(callback);
    }

    stop() {
        if (this.keepAliveInterval) {
          clearInterval(this.keepAliveInterval);
        }
        this.server.StopServer();
    }

    // ... other methods
}

module.exports = { BLEServer }