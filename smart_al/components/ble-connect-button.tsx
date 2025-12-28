// Execute BLE connection on clien side; server cant do it
"use client";

import { useState, useEffect, useRef } from "react";
import { Button } from "@/components/ui/button";

const SERVICE_UUID = "000000ff-0000-1000-8000-00805f9b34fb";
const CHARACTERISTIC_UUID = "0000ff01-0000-1000-8000-00805f9b34fb";

//connecton status
export function BleConnectButton() {
  const [status, setStatus] = useState<"idle" | "connecting" | "connected" | "failed">(
    "idle"
  );
  //storing bluetooth object for retrying
  const deviceRef = useRef<BluetoothDevice | null>(null);
  const serverRef = useRef<BluetoothRemoteGATTServer | null>(null);
  const reconnectAttempts = useRef(0); //reconnect attempt count
  const maxReconnectAttempts = 5; //max reconnect attempt

  //triggered by user click
  async function connect() {
    try {
      setStatus("connecting");
      reconnectAttempts.current = 0;

      //scan for device with the name X and advertising same UUID
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ name: "ESP_GATTS_DEMO" }],
        optionalServices: [SERVICE_UUID],
      });

      deviceRef.current = device; // Store device reference
      await attemptConnection();
    } catch (err) {
      console.error("Initial device selection error:", err);
      setStatus("idle");
    }
  }

  async function disconnect() {
    if (serverRef.current && serverRef.current.connected) {
      serverRef.current.disconnect();
    }
    deviceRef.current = null;
    serverRef.current = null;
    reconnectAttempts.current = 0;
    console.log("Disconnected");
    setStatus("idle");
  }

  async function attemptConnection() {
    try {
      reconnectAttempts.current++;
      const device = deviceRef.current; //get stored device from ref

      //check if user have selected device to connect to
      if (!device) {
        throw new Error("No device selected");
      }

      const server = await device.gatt!.connect();  //establish BLE connection; ! = gatt is not null(TypeScript)
      serverRef.current = server;
      const service = await server.getPrimaryService(SERVICE_UUID);
      const characteristic =
        await service.getCharacteristic(CHARACTERISTIC_UUID); //requiered for notifications or writes

      //subscribe to notifications
      await characteristic.startNotifications();

      //triggered when esp32 sends notification
      characteristic.addEventListener(
        "characteristicvaluechanged",
        (event: Event) => {
          const target =
            event.target as BluetoothRemoteGATTCharacteristic; //event target type so TypeScripts is not bitching
          const value = target.value!; //value is DataView holding raw bytes
          //bytes as 32-bit unsingend int & little endian
          const dv = new DataView(value.buffer);
          const data = dv.getUint32(0, true);
          console.log("Received:", data);
        }
      );

      setStatus("connected"); //update UI
      console.log("BLE connected");
      reconnectAttempts.current = 0; //reset count on success
    } catch (err) {
      console.error("BLE connection attempt error:", err); //log errors
      
      //check if should retry
      if (reconnectAttempts.current < maxReconnectAttempts) {
        console.log(`Retrying connection (${reconnectAttempts.current}/${maxReconnectAttempts})...`);
        setTimeout(() => {
          attemptConnection();
        }, 1000);
      } else {
        console.log("Max reconnection attempts reached");
        setStatus("failed");
        deviceRef.current = null;
        serverRef.current = null;
      }
    }
  }

  return (
    //onClick={connect} = user gesture (required by browser); disabled = prevents double connection attempts
    <Button size="lg" onClick={status === "connected" ? disconnect : connect} disabled={status === "connecting"}>
      {status === "idle" && "Connect device"}
      {status === "connecting" && "Connecting…"}
      {status === "connected" && "Disconnect"}
      {status === "failed" && "Connection failed. Try again."}
    </Button>
  );
}